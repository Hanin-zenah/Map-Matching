#include "shared.h"

// Try longer queries
// Plot outputs

int visit_count = 0;

double nodes_dist(struct node g_nd, Point* t_nd) {
    // if (t_nd == NULL)
    // {

    // }
    assert(t_nd != NULL);
    double dist = sqrt(pow((t_nd -> latitude - g_nd.lat), 2.0) + pow((t_nd -> longitude - g_nd.longitude), 2.0));
    return dist;
}

struct fs_pq_data* travel_reachable(stack <struct fs_pq_data*>& reachable) {
    /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
        the current cell, might be from the previous cells if there are no reachable nodes in this cell */
    struct fs_pq_data* next_edge = reachable.top();
    reachable.pop();
    return next_edge;
}

void Dijkstra(Graph* graph, SP_Tree* tree, int target) {
    while(!tree->sp_pq.empty()) {
        //extract the next min edge (distance to the tagret)
        pair<pair<int, int>, double> cur_edge = tree->sp_pq.top();
        int edge_target = cur_edge.first.second;
        tree->sp_pq.pop();
        if(tree->nodes.at(edge_target)->settled) {
            continue;
        }
        visit_count++;
        tree->nodes.at(edge_target) -> settled = true;

        //traverse all the neighbours of edge_target
        for(int i = 0; i < get_outdeg(graph, edge_target); i++) {
            int out_edge = get_out_edge(graph, edge_target, i);
            double dist = tree->nodes.at(edge_target)->distance + graph->edges[out_edge].cost;
            int neighbour = graph->edges[out_edge].trgtid;
            //check if tree[neighbour] exists
            if(tree->nodes.find(neighbour) == tree->nodes.end()) {
                //malloc space and add it to tree
                tree->nodes[neighbour] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
                // visit_count++;
                tree->nodes.at(neighbour)->settled = false; 
                tree->nodes.at(neighbour)->distance = INFINITY; 
            }
            if(tree->nodes.at(neighbour)->settled) {
                //node is already settled -> no need to traverse it
                continue;
            }
            if(dist < tree->nodes.at(neighbour)->distance) {
                tree->nodes.at(neighbour) -> distance = dist;
                tree->nodes.at(neighbour) -> settled = false;
                tree->sp_pq.push(make_pair(make_pair(edge_target, neighbour), dist));
            }
        }
        if(edge_target == target) {
            return;
        }
    }
}

// Returns the vertex ID at the target Dijkstra rank
int DijkstraUntilRankFound(Graph* graph, SP_Tree* tree, int target_rank, bool* all_vertices_searched) {
    int curr_dijkstra_rank = 0;
    int last_node_visited = -1;

    if (all_vertices_searched != NULL)
    {
        *all_vertices_searched = true;
    }

    while(!tree->sp_pq.empty()) {
        
        //extract the next min edge (distance to the tagret)
        pair<pair<int, int>, double> cur_edge = tree->sp_pq.top();
        int edge_target = cur_edge.first.second;

        last_node_visited = edge_target;
        curr_dijkstra_rank++;
        if (curr_dijkstra_rank == target_rank)
        {
            if (all_vertices_searched != NULL)
            {
                *all_vertices_searched = false;
            }
            break;
        }

        tree->sp_pq.pop();
        if(tree->nodes.at(edge_target)->settled) {
            continue;
        }
        visit_count++;
        tree->nodes.at(edge_target) -> settled = true;

        //traverse all the neighbours of edge_target
        for(int i = 0; i < get_outdeg(graph, edge_target); i++) {
            int out_edge = get_out_edge(graph, edge_target, i);
            double dist = tree->nodes.at(edge_target)->distance + graph->edges[out_edge].cost;
            int neighbour = graph->edges[out_edge].trgtid;
            //check if tree[neighbour] exists
            if(tree->nodes.find(neighbour) == tree->nodes.end()) {
                //malloc space and add it to tree
                tree->nodes[neighbour] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
                // visit_count++;
                tree->nodes.at(neighbour)->settled = false; 
                tree->nodes.at(neighbour)->distance = INFINITY; 
            }
            if(tree->nodes.at(neighbour)->settled) {
                //node is already settled -> no need to traverse it
                continue;
            }
            if(dist < tree->nodes.at(neighbour)->distance) {
                tree->nodes.at(neighbour) -> distance = dist;
                tree->nodes.at(neighbour) -> settled = false;
                tree->sp_pq.push(make_pair(make_pair(edge_target, neighbour), dist));
            }
        }
    }
    return last_node_visited;
}

void hubLabelShortestPath(Graph* graph, SP_Tree* tree, int target, HubLabelOffsetList& hub_labels)
{
    auto it = tree->nodes.find(target);
    if (it != tree->nodes.end())
    {
        return;
    }

    tree->nodes[target] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
    tree->nodes.at(target)->settled = true; 
    tree->nodes.at(target)->distance = hub_labels.getShortestDistance(tree->root, target);
    visit_count++;

    return;

    // int root_node = tree->root;

    // // Add the target node to the tree if it is not already there
    // if(tree->nodes.find(target) == tree->nodes.end()) {
    //     //malloc space and add it to tree
    //     tree->nodes[target] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
    //     tree->nodes.at(target)->settled = true; 
    //     tree->nodes.at(target)->distance = hub_labels.getShortestDistance(root_node, target);
    //     visit_count++;
    // }

    // // Start the search from the target
    // int current_node = target;
    // while (true)
    // {
    //     // Stop the search once the root node is found
    //     if (current_node == root_node)
    //     {
    //         tree->nodes[root_node]->settled = true;
    //         tree->nodes[root_node]->distance = 0;
    //         break;
    //     }
    //     bool break_flag = false;
    //     int n_incoming_neighbours = get_indeg(graph, current_node);
    //     for (int i=0; i<n_incoming_neighbours; i++)
    //     {
            
    //         int edge = get_in_edge(graph, current_node, i);
    //         if (edge == -1) { throw std::logic_error("Invalid edge"); }

    //         int neighbour = graph->edges[edge].srcid;

    //         // The node with this distance is the preceding node to the current node in the SP tree
    //         double shortest_dist_target = tree->nodes[current_node]->distance - graph->edges[edge].cost;

    //         double neighbour_shortest_dist;

    //         // If neighbouring node is not in tree
    //         auto it = tree->nodes.find(neighbour);
    //         if (it == tree->nodes.end()) {
    //             neighbour_shortest_dist = hub_labels.getShortestDistance(root_node, neighbour);
    //             visit_count++;
    //             //malloc space and add it to tree
    //             tree->nodes[neighbour] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
    //             tree->nodes.at(neighbour)->settled = true;
    //             tree->nodes.at(neighbour)->distance = neighbour_shortest_dist;
    //         }
    //         else
    //         {
    //             neighbour_shortest_dist = it->second->distance;
    //         }

    //         if (fabs(neighbour_shortest_dist-shortest_dist_target) < 0.01)
    //         {
    //             current_node = neighbour;
    //             break_flag = true;
    //             break;
    //         }
    //     }

    //     if (!break_flag) { throw std::logic_error("Did not find vertex on shortest path."); }
    // }
}

void hubLabelDistanceSortedShortestPath(Graph* graph, SP_Tree* tree, int target, HubLabelOffsetList& hub_labels, double initial_upper_bound)
{
    // Add root forward labels
    if (tree->root_hubs_added == false)
    // if (tree->nodes.size() <= 1)
    {
        // Add all forward hub labels of the root into the root hub distance map and the SP tree distance map
        int n_labels;
        auto hl_iterator = hub_labels.getVertexHubList(&n_labels, tree->root, true);
        for (int i=0; i<n_labels; i++)
        {
            HubLabel& hl = *hl_iterator;
            tree->nodes[hl.vertex_id] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
            tree->nodes[hl.vertex_id]->settled = true;
            tree->nodes[hl.vertex_id]->distance = hl.distance * hub_labels.EDGE_LENGTH_MULTIPLIER;
            hl_iterator++;
        }

        tree->root_hubs_added = true;
    }

    // If target vertex is already in distance map, return
    auto it = tree->nodes.find(target);
    if (it != tree->nodes.end())
    {
        return;
    }  

    // Iterate through the backward hub labels of the target vertex
    int n_labels;
    auto hl_iterator = hub_labels.getVertexHubList(&n_labels, target, false);
    double distance_upper_bound = initial_upper_bound;

    int i;
    for (i=0; i<n_labels; i++, hl_iterator++)
    {
        HubLabel& hl = *hl_iterator;
        
        // If the backward hub label is not in the distance map, then it cannot be the hub label on the shortest distance
        // path from the root, as all the forward labels of the root have already been added to the distance map
        auto vertex_it = tree->nodes.find(hl.vertex_id);
        if (vertex_it == tree->nodes.end())
        {
            continue;
        }
        double forward_distance = vertex_it->second->distance;
        double backward_distance = hl.distance * hub_labels.EDGE_LENGTH_MULTIPLIER;

        distance_upper_bound = std::min(distance_upper_bound, forward_distance + backward_distance);

        if (backward_distance >= distance_upper_bound)
        {
            break;
        }
    }
    // cout << "Searched through " << i << " out of " << n_labels << " labels" << '\n';
    if (distance_upper_bound == initial_upper_bound)
    {
        // cout << "Initial upper bound returned: adjacent edge is on the shortest path." << '\n';
    }

    tree->nodes[target] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
    tree->nodes[target]->settled = true;
    tree->nodes[target]->distance = distance_upper_bound;
    visit_count++;

}

// double getDistanceSortedDistance(Graph* graph, SP_Tree* tree, int target, HubLabelOffsetList& hub_labels, unordered_map<int,double>& root_hub_distance_map, double initial_upper_bound)
double getDistanceSortedDistance(Graph* graph, SP_Tree* tree, int target, HubLabelOffsetList& hub_labels, unordered_map<int,double>& root_hub_distance_map)
{   
    auto it = tree->nodes.find(target);
    if (it != tree->nodes.end())
    {
        cout << "Searched through 0 labels, target already in distance map" << '\n';
        return it->second->distance;
    }


    // Iterate through the backward hub labels of the target vertex
    int n_labels;
    auto hl_iterator = hub_labels.getVertexHubList(&n_labels, target, false);
    // double distance_upper_bound = initial_upper_bound;
    double distance_upper_bound = 0;

    int i;
    for (i=0; i<n_labels; i++, hl_iterator++)
    {
        HubLabel& hl = *hl_iterator;
        
        // If the backward hub label is not in the distance map, then it cannot be the hub label on the shortest distance
        // path from the root, as all the forward labels of the root have already been added to the distance map
        // auto vertex_it = root_hub_distance_map.find(hl.vertex_id);
        auto vertex_it = tree->nodes.find(hl.vertex_id);
        if (vertex_it == tree->nodes.end())
        {
            continue;
        }
        double forward_distance = vertex_it->second->distance;
        double backward_distance = hl.distance * hub_labels.EDGE_LENGTH_MULTIPLIER;

        distance_upper_bound = std::min(distance_upper_bound, forward_distance + backward_distance);

        if (backward_distance >= distance_upper_bound)
        {
            break;
        }
    }
    cout << "Searched through " << i << " out of " << n_labels << " labels" << '\n';

    tree->nodes[target] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
    tree->nodes[target]->settled = true;
    tree->nodes[target]->distance = distance_upper_bound;

    // if (distance_upper_bound == std::numeric_limits<double>::max())
    // {
    //     throw std::logic_error("Hub label distance not found.");
    // }

    return distance_upper_bound;
}

// When a 
void hubLabelDistanceSortedShortestPath(Graph* graph, SP_Tree* tree, int target, HubLabelOffsetList& hub_labels)
{
    unordered_map<int,double> root_hub_distance_map;

    int root_node = tree->root;

    // Add all forward hub labels of the root into the root hub distance map and the SP tree distance map
    int n_labels;
    auto hl_iterator = hub_labels.getVertexHubList(&n_labels, root_node, true);
    for (int i=0; i<n_labels; i++)
    {
        HubLabel& hl = *hl_iterator;
        tree->nodes[hl.vertex_id] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
        tree->nodes[hl.vertex_id]->settled = true;
        tree->nodes[hl.vertex_id]->distance = hl.distance * hub_labels.EDGE_LENGTH_MULTIPLIER;
        root_hub_distance_map[hl.vertex_id] = hl.distance * hub_labels.EDGE_LENGTH_MULTIPLIER;
        hl_iterator++;
    }

    // Add the target node to the tree
    getDistanceSortedDistance(graph, tree, target, hub_labels, root_hub_distance_map);
    visit_count++;

    // Start the search from the target
    int current_node = target;
    while (true)
    {
        // Stop the search once the root node is found
        if (current_node == root_node)
        {
            tree->nodes[root_node]->settled = true;
            tree->nodes[root_node]->distance = 0;
            break;
        }
        bool break_flag = false;
        int n_incoming_neighbours = get_indeg(graph, current_node);
        for (int i=0; i<n_incoming_neighbours; i++)
        {
            
            int edge = get_in_edge(graph, current_node, i);
            if (edge == -1) { throw std::logic_error("Invalid edge"); }

            int neighbour = graph->edges[edge].srcid;

            // The node with this distance is the preceding node to the current node in the SP tree
            double shortest_dist_target = tree->nodes[current_node]->distance - graph->edges[edge].cost;

            double neighbour_shortest_dist;

            // If neighbouring node is not in tree
            auto it = tree->nodes.find(neighbour);
            if (it == tree->nodes.end()) {
                neighbour_shortest_dist = getDistanceSortedDistance(graph, tree, neighbour, hub_labels, root_hub_distance_map);
                visit_count++;
            }
            else
            {
                neighbour_shortest_dist = it->second->distance;
            }

            if (fabs(neighbour_shortest_dist-shortest_dist_target) < 0.01)
            {
                current_node = neighbour;
                break_flag = true;
                break;
            }
        }

        if (!break_flag) { throw std::logic_error("Did not find vertex on shortest path."); }
    }
}


double matching_path_cost(Graph* graph, FSnode* final_node) {
    FSnode* cur = final_node;
    double path_cost = 0;
    while(cur -> parent) {
        FSnode* cur_parent = cur -> parent;
        int src_id = cur_parent -> vid;
        int trg_id = cur -> vid;
        struct node src_node = graph -> nodes[src_id];
        struct node trg_node = graph -> nodes[trg_id];
        Euc_distance ed;
        double cost = ed.euc_dist(src_node.lat, src_node.longitude, trg_node.lat, trg_node.longitude);
        path_cost += cost;
        cur = cur -> parent;
    }
    return path_cost;
}

void print_path(Graph* graph, string file_name, FSnode* final_node) {
    ofstream file(file_name);
    FSnode* cur = final_node;
    while(cur -> parent) {
        file<< graph -> nodes[cur -> vid].longitude <<" "<<graph -> nodes[cur -> vid].lat
        <<" "<<graph -> nodes[cur -> parent -> vid].longitude <<" "<<graph -> nodes[cur -> parent -> vid].lat<<endl;
        cur = cur -> parent;
    }
    file.close();
    return;
}

void write_sur_graph(FSgraph* fsgraph, Graph* graph, string file_name) {
    ofstream file(file_name);
    for(int i = 0; i < fsgraph -> fsedges.size(); i++) {
        //x y x y 
        int source_vid = fsgraph -> fsedges[i] -> src -> vid;
        int target_vid = fsgraph -> fsedges[i] -> trg -> vid;

        double src_lat, src_lon, trg_lat, trg_lon;
        src_lat = graph -> nodes[source_vid].lat;
        src_lon = graph -> nodes[source_vid].longitude;
        trg_lat = graph -> nodes[target_vid].lat;
        trg_lon = graph -> nodes[target_vid].longitude;

        file << src_lon<< " " << src_lat << " " << trg_lon << " " << trg_lat << endl; // what (Vi, Tj) should looks like
    }
    file.close();
}