#include "interleaved.h"


Grid_search gs;
pair<double, double> interleaved_matching(Graph* graph, Trajectory* traj, Grid* grid) {
    double global_eps = INFINITY;

    priority_queue<fs_pq_data_*, vector<fs_pq_data_*>, Comp_eps> bigger_eps; 
    stack<fs_pq_data*> reachable;

    //number of points in the trajectory 
    int m = traj -> length;
    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> source_set = gs.k_nodes(graph, grid, traj->points[0], 1000);
    int number = source_set.size();

    backup_super_edge(source_set, bigger_eps);
    fs_pq_data_* cur_edge = bigger_eps.top(); //pointer to the edge that is being traversed next 
    global_eps = cur_edge->fsedge->botlneck_val;
    bigger_eps.pop();

    backup_super_edge(source_set, bigger_eps);

    bool finished = false; 
    while(!finished) {
        cur_edge = traversal(graph, traj, &global_eps, bigger_eps, reachable, cur_edge, source_set);

        // We are finished if the current free space edge leads to an path with length m-1 in the free space (covers m nodes)
        finished = (cur_edge -> fsedge -> trg -> tid >= m - 1);
    }

    print_path(graph, "saarland_long_matched_path_plot.dat", cur_edge->fsedge->trg);

    // #if USE_HUB_LABELS
    //     print_path(graph, "hub_trajectory.dat", cur_edge->fsedge->trg);
    // #else
    //     print_path(graph, "djikstra_trajectory.dat", cur_edge->fsedge->trg);
    // #endif

    double cost = matching_path_cost(graph, cur_edge->fsedge->trg);
    return make_pair(global_eps, cost);
}

struct fs_pq_data_* increase_eps(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps, double* global_eps){
    if(bigger_eps.empty()) {
        //shouldn't reach here
        cerr << "NO MORE UNREACHABLE EDGES (bigger_eps is empty)\n";
    }

    struct fs_pq_data_* next_min_eps = bigger_eps.top();
    bigger_eps.pop(); 
    //only if it is a super edge and has eps larger than the global eps -> add backup super edge 
    if(!next_min_eps->fsedge->src && next_min_eps->fsedge->botlneck_val > *global_eps) {
        backup_super_edge(source_set, bigger_eps);
    }

    *global_eps = next_min_eps -> fsedge -> botlneck_val;
    return next_min_eps;
}

// Builds an edge from fsnd to another node in the free space and returns the bottleneck value of the new edge
double build_node(FSgraph* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right, double* global_eps) {
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
        fnd -> dist = nodes_dist(graph -> nodes[fnd -> vid], traj -> points[fnd -> tid]); 
        fnd -> parent = fsnd; 

        fsgraph -> fsnodes.push_back(fnd);

        fsgraph -> pair_dict[pair] = fnd;

        fedge -> trg = fnd; 
    }
    else { 
         /* pair already exists on free space graph */
        fedge -> trg = fsgraph->pair_dict.at(pair);
    }

    fedge -> src = fsnd; 
    fedge -> botlneck_val = max(fedge -> trg -> dist, *global_eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);

    return fedge -> botlneck_val;
}


struct fs_pq_data_* traversal(Graph* graph, Trajectory* traj, double* global_eps, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps,
                    stack<struct fs_pq_data*>& reachable, struct fs_pq_data_* cur_edge, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set) {
    
    // Target node of the current free space edge
    FSnode* fnd = cur_edge->fsedge->trg;

    // Get neighbouring vertices in the graph
    vector<int> incidents = get_incident(graph, fnd -> vid);

    vector<double> btl_neck_vals; 

    double eps = build_node(cur_edge->fsgraph, graph, traj, fnd, fnd -> vid, 0, 1, global_eps); //build horizontal node 
    btl_neck_vals.push_back(eps);
    
    for(int i = 0; i < incidents.size(); i++) {
        int neighbour_id = incidents[i];
        //if the neighbour is not in the sp DAG rooted at 'root' or if neighbour not settled yet -> continue sp computation until a sp is found to this neighbour
        if(!cur_edge->tree-> node_settled(neighbour_id)) {
            int tree_nodes_before = cur_edge->tree->nodes.size();
            auto t1 = chrono::high_resolution_clock::now();
            #if USE_HUB_LABELS
                int edge_id = get_out_edge(graph, fnd->vid, i);
                double init_upper_bound = graph->edges[edge_id].cost + cur_edge->tree->nodes[fnd->vid]->distance;
                hubLabelDistanceSortedShortestPath(graph, cur_edge->tree, neighbour_id, hl, init_upper_bound);
                
                // hubLabelShortestPath(graph, cur_edge->tree, neighbour_id, hl);
            #else
                Dijkstra(graph, cur_edge->tree, neighbour_id);
            #endif
            auto t2 = chrono::high_resolution_clock::now(); 
            int duration = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

            int tree_nodes_diff = cur_edge->tree->nodes.size() - tree_nodes_before;
        }

        /* only if incident node is also a neighbour of the current node in the sp DAG 
            do we build the freespace nodes + edges on the diagonal and vertical transition */
        if(cur_edge->tree->is_sp_edge(graph, fnd->vid, neighbour_id)) {
            eps = build_node(cur_edge->fsgraph, graph, traj, fnd, neighbour_id, 1, 1, global_eps); // build diagonal node //change to return edges 
            btl_neck_vals.push_back(eps);
            eps = build_node(cur_edge->fsgraph, graph, traj, fnd, neighbour_id, 1, 0, global_eps); //build upwards node 
            btl_neck_vals.push_back(eps);
        }
    }

    int size = cur_edge->fsgraph->fsedges.size();
    for(int i = 0; i < btl_neck_vals.size(); i++) {
        struct fs_pq_data_* next_edge = (struct fs_pq_data_*) malloc(sizeof(fs_pq_data_));
        next_edge -> fsedge = cur_edge->fsgraph -> fsedges[size - btl_neck_vals.size() + i];
        next_edge -> fsgraph = cur_edge -> fsgraph;
        next_edge -> tree = cur_edge -> tree;
        if(btl_neck_vals[i] > *global_eps) {
            bigger_eps.push(next_edge);
        }
        else {  
            reachable.push(next_edge);
        }
    }

    FSnode* next_nd;
    bool visited = true;
    struct fs_pq_data_* next_edge;
    while(visited) { 
        if(reachable.empty()) {
                /* case 1: if there are no more readily traversable edges in the freespace graph pq, update the eps (leash length) */
                next_edge = increase_eps(source_set, bigger_eps, global_eps); 
                //check for other super edges that have distance to P0 that is within the new eps 
                while(!source_set.empty() && source_set.top().second <= *global_eps) {
                    backup_super_edge(source_set, bigger_eps);
                }
        }    
        else { 
            /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
                the current cell, might be from the previous cells if there are no reachable nodes in this cell */
            next_edge = (fs_pq_data_*) travel_reachable(reachable); //will this work???? (if not, just define travel reachable separately for each implementation)
        }
        next_nd = next_edge -> fsedge -> trg;
        visited = next_nd -> visited;
    } 
    next_nd -> visited = true;
    return next_edge;
}


// Constructs a super edge to the next closest vertex
// source_set: priority queue of source nodes sorted by distance to T
// bigger_eps: priority queue of free space edges sorted by their epsilon value
void backup_super_edge(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps) {
    if(!source_set.empty()) {

        // Get the next closest vertex in the graph
        Gpair s = source_set.top();
        source_set.pop();

        // Instantiate a shortest path tree at the vertex and a new free space graph
        SP_Tree* tree = new SP_Tree(s.first);
        FSgraph* fsgraph = new FSgraph();
        
        // Create a free space node and a super edge to that node
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

        // Add the node to the data structures
        FSpair pair = {node -> vid, node -> tid};
        fsgraph->pair_dict[pair] = node;
        fsgraph->fsnodes.push_back(node);

        // Store the super edge and its corresponding free space graph and SP tree in the priority queue
        fs_pq_data_* data_ptr = (fs_pq_data_*) malloc(sizeof(fs_pq_data_));
        data_ptr -> fsedge = se;
        data_ptr -> fsgraph = fsgraph;
        data_ptr -> tree = tree;
        bigger_eps.push(data_ptr);
    }
}