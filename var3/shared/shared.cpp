#include "shared.h"

double nodes_dist(struct node g_nd, Point* t_nd) {
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