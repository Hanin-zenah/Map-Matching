#include "skip_algorithm.h"

int n_fs_edges = 0;
int n_local_sources = 0;
int n_cover_sources = 0;
int n_local_dist_comp = 0;
int n_cover_dist_comp = 0;

int distance_time = 0;
int final_check_time = 0;

// std::ofstream log_output;
extern std::ofstream log_output;

Grid_search gs;
pair<double, double> match(Graph* graph, Trajectory* traj, Grid* grid) {

    n_fs_edges = 0;
    n_local_sources = 0;
    n_cover_sources = 0;
    n_local_dist_comp = 0;
    n_cover_dist_comp = 0;
    final_check_time = 0;
    distance_time = 0;

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

    //always have one back up super edge 
    backup_super_edge(source_set, bigger_eps, graph, cover_nodes_sp);

    bool finished = false; 
    double cost = 0.0;
    while(!finished) {
        cur_edge = traversal(graph, traj, &global_eps, bigger_eps, reachable, cur_edge, source_set, cover_nodes_sp);
        //we are finished if we find a path that matches the last point on the trajectory AND is the actual shortest path on the graph
        if(cur_edge -> fsedge -> trg -> tid >= m - 1) {
            //get the matching path cost 
            cost = matching_path_cost(graph, cur_edge->fsedge->trg); //change this to be computed for every transition as it is being built (to save time) 
            
            // SP_Tree* t2 = new SP_Tree(cur_edge->fsgraph->source);
            // // Dijkstra(graph, t, cur_edge->fsedge->trg->vid); //replace this with hub labels
            // hubLabelShortestPath(graph, t2, cur_edge->fsedge->trg->vid, hl);
            // double hub_real_cost = t2->nodes.at(cur_edge->fsedge->trg->vid)->distance;

            auto t1 = chrono::high_resolution_clock::now();          

            //get the real sp cost/distance value between source and target
            SP_Tree* t = new SP_Tree(cur_edge->fsgraph->source);
            if (use_hub_labels) {
                hubLabelShortestPath(graph, t, cur_edge->fsedge->trg->vid, hl);
            } else {
                Dijkstra(graph, t, cur_edge->fsedge->trg->vid);
            }
            n_cover_dist_comp++;
            double real_cost = t->nodes.at(cur_edge->fsedge->trg->vid)->distance;

            auto t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
            final_check_time += duration;

            // cout << "Final check duration in microseconds: " << duration << endl;

            // cout << "dijkstra_cost: " << real_cost << "hub_cost: " << hub_real_cost << '\n';

            /* we are done when we have found a shortest path */
            // finished = (cost - real_cost <= 0.3);
            finished = (cost - real_cost <= 3); // Increased threshold to 3 to account for rounding errors from different precision in stored hub label distances
        }
    }

    cout << "n_fs_edges: " << n_fs_edges << endl;
    cout << "n_local_sources: " << n_local_sources << endl;
    cout << "n_cover_sources: " << n_cover_sources << endl;
    cout << "n_local_dist_comp: " << n_local_dist_comp << endl;
    cout << "n_cover_dist_comp: " << n_cover_dist_comp << endl;
    cout << "final check total time: " << final_check_time << " microseconds" << endl;
    cout << "distance check total time: " << distance_time << " microseconds" << endl;
    cout << endl;
    cout << n_fs_edges << endl;
    cout << n_local_sources << endl;
    cout << n_cover_sources << endl;
    cout << n_local_dist_comp << endl;
    cout << n_cover_dist_comp << endl;
    cout << final_check_time << endl;
    cout << distance_time << endl;

    #ifdef LOG_OUTPUT    
    log_output << n_fs_edges << ", " << n_local_sources << ", " << n_cover_sources << ", " << n_local_dist_comp << ", " << n_cover_dist_comp << ", " << final_check_time << ", " << distance_time << '\n';
    #endif

    // print_path(graph, "shortest_matching_path.dat", cur_edge->fsedge->trg);
    // write_sur_graph(cur_edge->fsgraph, graph, "survived_graph.dat");

    return make_pair(global_eps, cost);
}

struct fs_pq_data_* traversal(Graph* graph, Trajectory* traj, double* global_eps, priority_queue<fs_pq_data_*, vector<fs_pq_data_*>, Comp_eps>& bigger_eps,
                    stack<fs_pq_data*>& reachable, struct fs_pq_data_* cur_edge, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, unordered_map<int, SP_Tree*>& cover_nodes_sp) {
    FSnode* fnd = cur_edge->fsedge->trg; 
    n_fs_edges++;
    vector<int> incidents = get_incident(graph, fnd -> vid);

    //build horizontal node 
    build_node(cur_edge, cur_edge->fsgraph, graph, traj, fnd, fnd -> vid, 0, 1, global_eps, cover_nodes_sp, bigger_eps, reachable);
    for(int i = 0; i < incidents.size(); i++) {
        int neighbour_id = incidents[i];

        /* if the neighbour is not in the sp DAG rooted at 'root' or if neighbour not settled yet 
            -> continue sp computation until a sp is found to this neighbour */
        if(!cur_edge->tree->node_settled(neighbour_id)) {
            auto t1 = chrono::high_resolution_clock::now();
            if (use_hub_labels) {
                dijkstra_hub(graph, cur_edge->tree, neighbour_id, hl);
            } else {
                Dijkstra(graph, cur_edge->tree, neighbour_id);
            }            
            auto t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
            distance_time += duration;
            n_local_dist_comp++;
        }

        /* only if incident node is also a neighbour of the current node in the sp DAG 
            do we build the freespace nodes + edges on the diagonal and vertical transition */
        if(cur_edge->tree->is_sp_edge(graph, fnd->vid, neighbour_id)) {
            // build diagonal node 
            build_node(cur_edge, cur_edge->fsgraph, graph, traj, fnd, neighbour_id, 1, 1, global_eps, cover_nodes_sp, bigger_eps, reachable); //change to return edges 
            //build vertical node 
            build_node(cur_edge,cur_edge->fsgraph, graph, traj, fnd, neighbour_id, 1, 0, global_eps, cover_nodes_sp, bigger_eps, reachable); 
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
        } else { 
            /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
                the current cell, might be from the previous cells if there are no reachable nodes in this cell */
            next_edge = (fs_pq_data_*) travel_reachable(reachable);
        }
        
        next_nd = next_edge -> fsedge -> trg;
        visited = next_nd -> visited;
    } 
    next_nd -> visited = true;
    return next_edge;
}

void build_node(struct fs_pq_data_* cur_edge, FSgraph_* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right, double* global_eps, unordered_map<int, SP_Tree*>& cover_nodes_sp,
                    priority_queue<fs_pq_data_*, vector<fs_pq_data_*>, Comp_eps>& bigger_eps, stack<fs_pq_data*>& reachable) {
    FSedge* fedge = (FSedge*) malloc(sizeof(FSedge));
    fedge -> src = fsnd; 

    FSpair pair; 
    pair.first = up ? neighbor_id : fsnd -> vid;
    pair.second = fsnd -> tid + right;

    unordered_map<FSpair, FSnode*, KeyPairHash>::const_iterator node_pair = fsgraph -> pair_dict.find(pair);

    /* we check if the corner/node pair already exists, if not, build a new node, but need to build a new edge regardless */
    if(node_pair == fsgraph -> pair_dict.end()) {
        FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
        fnd -> vid = pair.first; 
        fnd -> tid = pair.second;
        fnd -> visited = false; 
        fnd -> dist = nodes_dist(graph -> nodes[fnd -> vid], traj -> points[fnd -> tid]); 
        fnd -> parent = fsnd; 

        fsgraph -> fsnodes.push_back(fnd);
        fsgraph -> pair_dict[pair] = fnd;

        fedge -> trg = fnd; 

        //only change if current sp tree root is not a cover node 
        int cur_root = cur_edge->tree->root;
        if(!graph->nodes[cur_root].cover_node) {
            if(graph->nodes[fnd->vid].cover_node) {
                //check if a tree for this node exists or not 
                if(cover_nodes_sp.find(fnd->vid) == cover_nodes_sp.end()) {
                    //first time passing through this tree 
                    cover_nodes_sp[fnd->vid] = new SP_Tree(fnd->vid); 
                    n_cover_sources++;
                }

                cur_edge->fsgraph->old_tree_size = cur_edge->tree->nodes.size();
                cur_edge -> tree = cover_nodes_sp.at(fnd->vid);
            }
        }
    } else { 
         /* node already exists on free space graph */
        fedge -> trg = node_pair -> second; 
    }

    fedge -> botlneck_val = max(fedge -> trg -> dist, *global_eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);

    struct fs_pq_data_* next_edge = (struct fs_pq_data_*) malloc(sizeof(fs_pq_data_));
    next_edge -> fsedge = fedge;
    next_edge -> fsgraph = cur_edge -> fsgraph;
    next_edge -> tree = cur_edge -> tree;

    if(fedge->botlneck_val > *global_eps) {
        bigger_eps.push(next_edge);
    } else {  
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

/**
 * Adds a super edge to the bigger eps priority queue for the matching by taking a new source node and creating a new free space graph and sp tree accordingly 
*/
void backup_super_edge(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps, Graph* graph, unordered_map<int, SP_Tree*>& cover_nodes_sp) {
    if(!source_set.empty()) {
        Gpair s = source_set.top();
        source_set.pop();

        FSgraph_* fsgraph = new FSgraph_();
        SP_Tree* tree;

        if(graph->nodes[s.first].cover_node) {
            //if the root itself is a cover node 
            if(cover_nodes_sp.find(s.first) == cover_nodes_sp.end()) {
                //this cover node has not been reached before 
                cover_nodes_sp[s.first] = new SP_Tree(s.first);
                n_cover_sources++;
            }
            tree = cover_nodes_sp.at(s.first);
        } else {
            tree = new SP_Tree(s.first);
            n_local_sources++;
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

/**
 * Finds the minimum distance to the target node using a local Dijkstra search to find local
 * skip nodes, then takes the minimum distance path of all possible paths through each skip node.
*/
void dijkstra_hub(Graph* graph, SP_Tree* tree, int target, HubLabelOffsetList& hub_labels)
{
    // cout << "start1" 

    if (tree->nodes.find(target) != tree->nodes.end()) { return; }

    // Perform a Dijkstra search along the backwards edges from the target to get all surrounding skip nodes
    std::set<int> skip_nodes;

    SP_Tree* local_tree = new SP_Tree(target);

    while (!local_tree->sp_pq.empty())
    {
        pair<pair<int, int>, double> cur_edge = local_tree->sp_pq.top();
        int edge_target = cur_edge.first.second;

        local_tree->sp_pq.pop();

        if(local_tree->nodes.at(edge_target)->settled) {
            continue; 
        }

        visit_count++;
        local_tree->nodes.at(edge_target)->settled = true;

        // If the node is a cover node
        if (graph->nodes[edge_target].cover_node)
        {
            // cout << "adding skip node" << edge_target << endl;
            skip_nodes.insert(edge_target);
            continue;
        }

        for(int i=0; i < get_indeg(graph, edge_target); i++)
        {
            int in_edge = get_in_edge(graph, edge_target, i);
            double dist = local_tree->nodes.at(edge_target)->distance + graph->edges[in_edge].cost;
            int neighbour = graph->edges[in_edge].srcid;

            // cout << edge_target << " " << graph->edges[in_edge].trgtid << " " << graph->edges[in_edge].srcid << endl;

            //check if tree[neighbour] exists
            if(local_tree->nodes.find(neighbour) == local_tree->nodes.end()) {
                local_tree->nodes[neighbour] = (spTreeNodeData*)malloc(sizeof(spTreeNodeData));
                // visit_count++;
                local_tree->nodes.at(neighbour)->settled = false; 
                local_tree->nodes.at(neighbour)->distance = INFINITY; 
            }
            if(local_tree->nodes.at(neighbour)->settled) {
                continue;
            }
            if(dist < local_tree->nodes.at(neighbour)->distance) {
                local_tree->nodes.at(neighbour)->distance = dist;
                local_tree->nodes.at(neighbour)->settled = false;
                local_tree->sp_pq.push(make_pair(make_pair(edge_target, neighbour), dist));
            }
        }
    }
    
    cout << "n_skip_nodes: " << skip_nodes.size() << endl;
    if (skip_nodes.empty())
    {
        cout << "SKIP NODES NOT FOUND" << endl;
        exit(0);
    }

    // Find the minimum distance path to the target by taking the minimum distance path of the path
    // passing through each skip node
    double min_distance = std::numeric_limits<double>::infinity();
    for (int skip_node : skip_nodes)
    {        
        // Perform a hub label call if the skip node is not already stored in the shortest path tree
        if (tree->nodes.find(skip_node) == tree->nodes.end())
        {
            hubLabelShortestPath(graph, tree, skip_node, hub_labels);
        }

        double local_dist = local_tree->nodes[skip_node]->distance;
        double global_dist = tree->nodes[skip_node]->distance;
        double total_dist = local_dist + global_dist;
        min_distance = std::min(min_distance, total_dist);
    }
    
    tree->nodes[target] = (spTreeNodeData*)malloc(sizeof(spTreeNodeData));
    tree->nodes[target]->settled = true;
    tree->nodes[target]->distance = min_distance;

    delete local_tree;

    // cout << "min_dist: " << min_distance << endl;
}