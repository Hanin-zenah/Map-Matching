#include "skip_algorithm.h"

Grid_search gs;
pair<double, double> match(Graph* graph, Trajectory* traj, Grid* grid) {
    unordered_map<int, SP_Tree*> cover_nodes_sp; //stores the sp trees for the reached cover nodes -> to be reused by other sources 
    double global_eps = INFINITY; //optimal eps value for the matching 
    priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps> bigger_eps; //global pq to hold all the non-reachable free space transitions for all the reached source nodes
    stack<struct fs_pq_data*> reachable; //global list to store all the reachable transitions in all the constructed free space graphs

    int m = traj -> length;
    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> source_set = gs.k_nodes(graph, grid, traj->points[0], 1000);
    int number = source_set.size();

    backup_super_edge(source_set, bigger_eps, graph, cover_nodes_sp);
    struct fs_pq_data_* cur_edge = bigger_eps.top(); //pointer to the current edge that is being traversed next 
    global_eps = cur_edge->fsedge->botlneck_val;
    bigger_eps.pop();

    backup_super_edge(source_set, bigger_eps, graph, cover_nodes_sp);

    bool finished = false; 
    double cost = 0.0;
    while(!finished) {
        cur_edge = traversal(graph, traj, &global_eps, bigger_eps, reachable, cur_edge, source_set, cover_nodes_sp);
        //we are finished if we find a path that matches the last point on the trajectory AND is the actual shortest path on the graph
        if(cur_edge -> fsedge -> trg -> tid >= m - 1) {
            //get the matching path cost 
            cost = matching_path_cost(graph, cur_edge->fsedge->trg); //change this to be computed for every transition as it is built (to save time) 
            
            //get the real sp cost/distance value between source and target
            SP_Tree* t = new SP_Tree(cur_edge->fsgraph->source);
            Dijkstra(graph, t, cur_edge->fsedge->trg->vid); //replace this with hub labels
            double real_cost = t->nodes.at(cur_edge->fsedge->trg->vid)->distance;

            /* we are done when we have found a shortest path */
            finished = (cost - real_cost <= 0.3);

            if(!finished) { 
                //tried continuing with the same fsgraph (ie find another mathcing path from the same source) 
                //-> this only caused more computations (space and time) and ended up with same results as ignoring this free space graph as a whole
                cur_edge->fsgraph->done = true;
            }
            // delete cur_edge->fsgraph;
        }
    }
    
    print_path(graph, "shortest_matching_path.dat", cur_edge->fsedge->trg);
    write_sur_graph(cur_edge->fsgraph, graph, "survived_graph.dat");

    return make_pair(global_eps, cost);
}

struct fs_pq_data_* traversal(Graph* graph, Trajectory* traj, double* global_eps, priority_queue<fs_pq_data_*, vector<fs_pq_data_*>, Comp_eps>& bigger_eps,
                    stack<fs_pq_data*>& reachable, struct fs_pq_data_* cur_edge, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, unordered_map<int, SP_Tree*>& cover_nodes_sp) {
    FSnode* fnd = cur_edge->fsedge->trg; 
    vector<int> incidents = get_incident(graph, fnd -> vid);
    build_node(cur_edge, cur_edge->fsgraph, graph, traj, fnd, fnd -> vid, 0, 1, global_eps, cover_nodes_sp, bigger_eps, reachable); //build horizontal node 
    for(int i = 0; i < incidents.size(); i++) {
        int neighbour_id = incidents[i];

        /* if the neighbour is not in the sp DAG rooted at 'root' or if neighbour not settled yet 
            -> continue sp computation until a sp is found to this neighbour */
        if(!cur_edge->tree->node_settled(neighbour_id)) {
            Dijkstra(graph, cur_edge->tree, neighbour_id);
        }

        /* only if incident node is also a neighbour of the current node in the sp DAG 
            do we build the freespace nodes + edges on the diagonal and vertical transition */
        if(cur_edge->tree->is_sp_edge(graph, fnd->vid, neighbour_id)) {
            //if any of the incident nodes is a cover node, dont traverse the other neighbours/ incidents and only build that node/transition in the free space graph 
            bool before = cur_edge->fsgraph->flag;
            build_node(cur_edge, cur_edge->fsgraph, graph, traj, fnd, neighbour_id, 1, 1, global_eps, cover_nodes_sp, bigger_eps, reachable); // build diagonal node //change to return edges 
            build_node(cur_edge,cur_edge->fsgraph, graph, traj, fnd, neighbour_id, 1, 0, global_eps, cover_nodes_sp, bigger_eps, reachable); //build upwards node 

            if(!before) {
                //if we have just passed through a cover node
                if(cur_edge->fsgraph->flag) {
                    break;
                }
            }
        }
    }

    FSnode* next_nd;
    bool visited = true;
    struct fs_pq_data_* next_edge;

    /* only traverse a transition if fsgraph->flag == fsedge-> flag  
        (i.e. dont consider transitions that were built before running into a cover node if fsgraph->flag = true) AND if fsgraph-> done == false */
    while(visited) { 
        if(reachable.empty()) {
            /* case 1: if there are no more readily traversable edges in the freespace graph pq, update the global eps (leash length) */
            next_edge = increase_eps(graph, source_set, bigger_eps, global_eps, cover_nodes_sp); 

            //check for other source nodes reachable using the new global eps value
            while(!source_set.empty() && source_set.top().second <= *global_eps) {
                backup_super_edge(source_set, bigger_eps, graph, cover_nodes_sp);
            }


            //*** if we only want to consider the reachable edges one an fsgraph-> done is true we can add the if check here only 
        }    
        else { 
            /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
                the current cell, might be from the previous cells if there are no reachable nodes in this cell */
            next_edge = (fs_pq_data_*) travel_reachable(reachable);
        }

        //before returning this edge for the next traversal, check if the flags are equal, otherwise ignore this transition
        //ignore if fsgraph is done as well
        if(!next_edge->fsgraph->done) {
            if(next_edge->flag == next_edge->fsgraph->flag) {
                next_nd = next_edge -> fsedge -> trg;
                visited = next_nd -> visited;
            }
        }
    } 
    next_nd -> visited = true;
    return next_edge;
}

void build_node(struct fs_pq_data_* cur_edge, FSgraph_* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right, double* global_eps, unordered_map<int, SP_Tree*>& cover_nodes_sp,
                    priority_queue<fs_pq_data_*, vector<fs_pq_data_*>, Comp_eps>& bigger_eps, stack<fs_pq_data*>& reachable) {
    if(fsgraph->done) {
        return;
    }
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
        FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
        fnd -> vid = pair.first; 
        fnd -> tid = pair.second;
        fnd -> visited = false; 
        fnd -> dist = nodes_dist(graph -> nodes[fnd -> vid], traj -> points[fnd -> tid]); 
        fnd -> parent = fsnd; 

        fsgraph -> fsnodes.push_back(fnd);

        fsgraph -> pair_dict[pair] = fnd;
        fedge -> trg = fnd; 

        if(!fsgraph->flag) {
        //if this is the first time cover node 
            if(graph->nodes[fnd->vid].cover_node) {
                fsgraph->flag = true;
                //check if a tree for this node exists or not 
                if(cover_nodes_sp.find(fnd->vid) == cover_nodes_sp.end()) {
                    //first time passing through this tree 
                    cover_nodes_sp[fnd->vid] = new SP_Tree(fnd->vid); //would it be necessary to change the distance values? no because we compute the cost of the final matching path from the free space graph not the trees
                }

                //destruct current tree (free memory); becasue we will never need it again 
                cur_edge->fsgraph->old_tree_size = cur_edge->tree->nodes.size();
                cur_edge -> tree = cover_nodes_sp.at(fnd->vid);
            }
        }
    }
    else { 
         /* node already exists on free space graph */
        fedge -> trg = fsgraph->pair_dict.at(pair);
    }

    fedge -> src = fsnd; 
    fedge -> botlneck_val = max(fedge -> trg -> dist, *global_eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);

    struct fs_pq_data_* next_edge = (struct fs_pq_data_*) malloc(sizeof(fs_pq_data_));
    next_edge -> fsedge = fedge;
    next_edge -> fsgraph = cur_edge -> fsgraph;
    next_edge -> tree = cur_edge -> tree;
    next_edge ->flag = fsgraph->flag;

    if(fedge->botlneck_val > *global_eps) {
        bigger_eps.push(next_edge);
    }
    else {  
        reachable.push(next_edge);
    }
}

struct fs_pq_data_* increase_eps(Graph* graph, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps, double* global_eps, unordered_map<int, SP_Tree*>& cover_nodes_sp) {
    if(bigger_eps.empty()) {
        //shouldn't reach here
        cerr << "NO MORE UNREACHABLE EDGES (bigger_eps is empty)\n";
    }

    struct fs_pq_data_* next_min_eps = bigger_eps.top();
    bigger_eps.pop(); 
    //only if it is a super edge and has eps larger than the global eps -> add backup super edge 
    if(!next_min_eps->fsedge->src && next_min_eps->fsedge->botlneck_val > *global_eps) {
        backup_super_edge(source_set, bigger_eps, graph, cover_nodes_sp);
    }

    *global_eps = next_min_eps -> fsedge -> botlneck_val;
    return next_min_eps;
}

void backup_super_edge(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps, Graph* graph, unordered_map<int, SP_Tree*>& cover_nodes_sp) {
    if(!source_set.empty()) {
        Gpair s = source_set.top();
        source_set.pop();

        FSgraph_* fsgraph = new FSgraph_();
        SP_Tree* tree;

        if(graph->nodes[s.first].cover_node) {
            //if the root itself IS a cover node 
            fsgraph->flag = true;
            if(cover_nodes_sp.find(s.first) == cover_nodes_sp.end()) {
                //this cover node has not been reached before 
                cover_nodes_sp[s.first] = new SP_Tree(s.first);
            }
            tree = cover_nodes_sp.at(s.first);
        } else {
            tree = new SP_Tree(s.first);
        }

        fsgraph -> source = s.first;

        FSedge* se = (FSedge*) malloc(sizeof(FSedge));
        FSnode* node = (FSnode*) malloc(sizeof(FSnode));
        node -> vid = s.first;
        node -> tid = 0;
        node -> dist = s.second;
        node -> visited = false;
        node -> parent = NULL;
        se -> src = NULL;
        se -> trg = node;
        se -> botlneck_val = s.second;

        FSpair pair = {node -> vid, node -> tid};
        fsgraph->pair_dict[pair] = node;
        fsgraph->fsnodes.push_back(node);

        fs_pq_data_* data_ptr = (fs_pq_data_*) malloc(sizeof(fs_pq_data_));
        data_ptr -> fsedge = se;
        data_ptr -> fsgraph = fsgraph;
        data_ptr -> tree = tree;
        bigger_eps.push(data_ptr);
    }
}

