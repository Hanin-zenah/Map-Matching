/*  given a graph that is pre-processed (subsampled, SCC, scale projected), and a pre-processed trajectory: 
    1- define a global eps 
    2- find the closest nodes in the graph to p0 and put them in S
    3- for each s in S create a hashmap that contains all the shortest path tree cpmputations rooted at s (this will be the tree structure), for dijkstra computations, just have a priority queue
    4- build the free space graph for that s and match it to the trajectory
*/
#include "naive.h"

Grid_search gs;
pair<double, double> naive_matching(Graph* graph, Trajectory* traj, Grid* grid) {
    double global_eps = INFINITY;
    double shortest_cost = INFINITY;

    int m = traj -> length;
    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> source_set = gs.k_nodes(graph, grid, traj->points[0], 1000);

    //for each source vertex in ASC order -> we want to compute the SP DAG and free space graph
    int i = 0;
    while(!source_set.empty()) {
        Gpair s = source_set.top();
        source_set.pop();
        if(s.second <= global_eps) {
            i++;
            SP_Tree* tree = new SP_Tree(s.first);
            //start the matching
            FSgraph_* fsgraph = new FSgraph_();
            fsgraph->eps = s.second; //local eps
        
            priority_queue<fs_pq_data*, vector<fs_pq_data*>, Comp_eps> bigger_eps;
            stack<fs_pq_data*> reachable;

            //start the matching
            //create the super edge for the source node to construct the free space graph
            FSedge* super_edge = (FSedge*) malloc(sizeof(FSedge));
            FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
            super_edge -> src = NULL;
            super_edge -> trg = start_nd;
            super_edge -> botlneck_val = s.second;

            start_nd -> vid = s.first;
            start_nd -> tid = 0;
            start_nd -> dist = s.second;
            start_nd -> visited = true;
            start_nd -> parent = NULL;
            
            FSpair pair = {start_nd -> vid, start_nd -> tid};
            fsgraph->pair_dict[pair] = start_nd;
            fsgraph->fsnodes.push_back(start_nd);

            struct fs_pq_data* cur_edge = (fs_pq_data*) malloc(sizeof(fs_pq_data));
            cur_edge->fsedge = super_edge;

            bool finished = false;
            while(!finished) {
                cur_edge = traversal(fsgraph, graph, traj, cur_edge, bigger_eps, reachable, tree, global_eps);
                if(!cur_edge) {
                    //local eps is larger than global eps --> proceed to match next closest node
                    break;
                }
                finished = (cur_edge->fsedge->trg->tid >= m - 1);
            }

            if(!finished) {
                //construction was interrupted; 
                continue;
            }

            //calculate the cost of the path
            double cost = matching_path_cost(graph, cur_edge->fsedge->trg);

            //if a path with a smaller eps is found -> favour this path regardless of cost
            if(fsgraph->eps < global_eps) {
                global_eps = fsgraph->eps;
                shortest_cost = cost; 
                delete fsgraph;
                // delete tree;
                continue;
            }

            //if a path with the same eps is found: check if the path is shorter than the global shortest path
            if(fsgraph->eps == global_eps) {
                if(cost < shortest_cost) {
                    shortest_cost = cost;
                    delete fsgraph;
                    //cleanup tree as well
                }
            }
        } 
        else {
            break;
        }
    }
    return make_pair(global_eps, shortest_cost);
}

fs_pq_data* increase_eps(priority_queue<fs_pq_data*, vector<fs_pq_data*>, Comp_eps>& bigger_eps, FSgraph_* fsgraph){
    if(bigger_eps.empty()) {
        //shouldn't reach here
        cerr << "NO MORE UNREACHABLE EDGES (bigger_eps is empty)\n";
    }
    fs_pq_data* min_eps_fedge = bigger_eps.top();
    bigger_eps.pop(); 
    fsgraph -> eps = min_eps_fedge -> fsedge -> botlneck_val;
    return min_eps_fedge;
}

double build_node(FSgraph_* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right) {
    FSedge* fedge = (FSedge*) malloc(sizeof(FSedge));
    FSpair pair; 

    if(up == 0) {
        pair.first = fsnd -> vid;
    } else {
        pair.first = neighbor_id;
    }
    pair.second = fsnd -> tid + right;

    /* test if the corner/node pair already exists, if not, build a new node, but need to build a new edge regardless */
    if(fsgraph -> pair_dict.find(pair) == fsgraph -> pair_dict.end()) {
        //if pair not in map 
        FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
        fnd -> vid = pair.first; 
        fnd -> tid = pair.second;
        fnd -> visited = false; 
        fnd -> dist = nodes_dist(graph -> nodes[fnd -> vid], traj -> points[fnd -> tid]); //error is here
        fnd -> parent = fsnd; 

        fsgraph -> fsnodes.push_back(fnd);
        fsgraph -> pair_dict[pair] = fnd;

        fedge -> trg = fnd; 
    }
    else { 
         /* pair already exists on graph */
        fedge -> trg = fsgraph->pair_dict.at(pair);
    }

    fedge -> src = fsnd; 
    fedge -> botlneck_val = max(fedge -> trg -> dist, fsgraph -> eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);

    return fedge -> botlneck_val;
}

fs_pq_data* traversal(FSgraph_* fsgraph, Graph* graph, Trajectory* traj, fs_pq_data* cur_edge, priority_queue<fs_pq_data*, vector<fs_pq_data*>, 
                Comp_eps>& bigger_eps, stack <fs_pq_data*>& reachable, SP_Tree* tree, double global_eps) {
    FSnode* fnd = cur_edge->fsedge->trg;
    vector<int> incidents = get_incident(graph, fnd -> vid);
    vector<double> btl_neck_vals; 
    double eps = build_node(fsgraph, graph, traj, fnd, fnd -> vid, 0, 1); //build horizontal node 
    btl_neck_vals.push_back(eps);
    for(int i = 0; i < incidents.size(); i++) {
        int neighbour_id  = incidents[i];
        //if the neighbour is not in the sp DAG rooted at 'root' or if neighbour not settled yet -> continue sp computation until a sp is found to this neighbour
        if(!tree->node_settled(neighbour_id)) {
            Dijkstra(graph, tree, neighbour_id);
        }

        /* only if incident node is also a neighbour of the current node in the sp DAG 
            do we build the freespace nodes + edges on the diagonal and vertical transition */
        if(tree->is_sp_edge(graph, fnd->vid, neighbour_id)) { 
            eps = build_node(fsgraph, graph, traj, fnd, neighbour_id, 1, 1); // build diagonal node //change to return edges 
            btl_neck_vals.push_back(eps);
            eps = build_node(fsgraph, graph, traj, fnd, neighbour_id, 1, 0); //build upwards node 
            btl_neck_vals.push_back(eps);
        }
    }

    int size = fsgraph -> fsedges.size();
    for(int i = 0; i < btl_neck_vals.size(); i++) {
        struct fs_pq_data* next_edge = (struct fs_pq_data*) malloc(sizeof(fs_pq_data));
        next_edge -> fsedge = fsgraph -> fsedges[size - btl_neck_vals.size() + i];
        if(btl_neck_vals[i] > fsgraph -> eps) {
            bigger_eps.push(next_edge);
        }
        else {  
            reachable.push(next_edge);
        }
    }

    FSnode* next_nd;
    bool visited = true;
    struct fs_pq_data* next_edge;
    while(visited) { 
        if(reachable.empty()) {
                /* case 1: if there are no more readily traversable edges in the freespace graph, update the eps (leash length) */
                next_edge = increase_eps(bigger_eps, fsgraph); 
                if(fsgraph->eps > global_eps) {
                    //no need to continue building the free space graph if local eps exceeds the best upper bound (global eps)
                    return NULL;
                }
        }    
        else { 
            /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
                the current cell, might be from the previous cells if there are no reachable nodes in this cell */
            next_edge = travel_reachable(reachable);
        }
        next_nd = next_edge->fsedge->trg;
        visited = next_nd -> visited;
    } 
    next_nd -> visited = true;

    return next_edge;
}
