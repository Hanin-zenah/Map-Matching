#include "graph_subsampling.h"
#include "graph.h"
#include <cmath>

//mark the split edges and dont do them agaion 
void split_bi_dir_edge(Graph* graph, int edgeID1, int edgeID2) {
    double x1, x2, y1, y2, num_new;
    double length_new = graph -> edges[edgeID1].cost / 2; // can use either edgeID1 or edgeID2

    x1 = graph -> nodes[graph -> edges[edgeID1].srcid].lat; 
    y1 = graph -> nodes[graph -> edges[edgeID1].srcid].longitude;
    x2 = graph -> nodes[graph -> edges[edgeID1].trgtid].lat;
    y2 = graph -> nodes[graph -> edges[edgeID1].trgtid].longitude;

    struct node nd; //new node to add in the middle 
    struct edge e; //new edge 
    int ori_trg1; //save the original target node
    ori_trg1 = graph -> edges[edgeID1].trgtid;
    nd.id = graph -> nodes.size();
    nd.lat = (x1 + x2) / 2;   
    nd.longitude = (y1 + y2) / 2;
    nd.osmid = 999999999; 
    graph -> nodes.push_back(nd);
    graph -> edges[edgeID1].trgtid = nd.id;
    graph -> edges[edgeID1].cost = length_new;
    e.id = graph -> edges.size();
    e.srcid = graph -> nodes.size() - 1; //n_nodes; // new created node
    e.trgtid = ori_trg1; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);

    graph -> out_off_edges.push_back(e.id);
    graph -> out_offsets.push_back(graph -> edges.size()); 


    // pair<bool, int> result1 = findInVector<int>(graph -> in_off_edges, edgeID1);
    auto it = find(graph -> in_off_edges.begin(), graph -> in_off_edges.end(), edgeID1);
    int index = distance(graph -> in_off_edges.begin(), it); 
    //cant we just get the source and do the same for the out edge array??


    graph -> in_off_edges[index] = e.id;
    graph -> in_off_edges.push_back(edgeID1); //need to do swapping
    graph -> in_offsets.push_back(graph -> edges.size());

    //repeat the same process for the other edge
    if(edgeID2 != -1) {
        int ori_trg2 = graph -> edges[edgeID2].trgtid;

        graph -> edges[edgeID2].trgtid = nd.id;
        graph -> edges[edgeID2].cost = length_new;
        e.id = graph -> edges.size(); //
        e.srcid = graph -> nodes.size() - 1; //n_nodes; // last created node
        e.trgtid = ori_trg2; //the original target node
        e.cost = length_new ;
        graph -> edges.push_back(e);

        graph -> out_off_edges.push_back(e.id);
        graph -> out_offsets.push_back(graph -> edges.size()); 

        // pair<bool, int> result2 = findInVector<int>(graph -> in_off_edges, edgeID2);
        auto it2 = find(graph -> in_off_edges.begin(), graph -> in_off_edges.end(), edgeID2);
        int index2 = distance(graph -> in_off_edges.begin(), it2);

        graph -> in_off_edges[index2] = e.id;
        graph -> in_off_edges.push_back(edgeID2); //need to do swapping
        graph -> in_offsets.push_back(graph -> edges.size());
    }

    //edge ID starts with zero, so the sentinal val = size = last edge id +1
    return;
}

// int bi_dir(Graph* graph, int edgeID) {
//     int b_edge = -1;

//     int start_in_offsets = graph -> edges[edgeID].srcid;
//     int end_in_offsets = start_in_offsets + 1; 

//     int start_in_edge_array = graph -> in_offsets[start_in_offsets];
//     int end_in_edge_array = graph -> in_offsets[end_in_offsets];

//     for(int j = start_in_edge_array; j < end_in_edge_array; j++) { //finding what nodes goes to the target node in the given edge.id
//         int start_in_offsets2, end_in_offsets2, start_in_edge_array2, end_in_edge_array2;

//         start_in_offsets2 = graph -> edges[graph -> in_off_edges[j]].srcid; //graph->edges[in_edgeID[j]].srcid are the nodes that goes to the same target node in given edge.id
//         end_in_offsets2 = start_in_offsets2 + 1;

//         if(graph -> edges[edgeID].trgtid == graph -> edges[graph -> in_off_edges[j]].srcid) {
//             b_edge = graph->edges[graph -> in_off_edges[j]].id;
//         }
//     }

//     return b_edge;
// }

int bi_dir(Graph* graph, int edge_id) {
    int not_found = -1;
    int src = graph -> edges[edge_id].srcid;
    int trgt = graph -> edges[edge_id].trgtid;
    //check if target node has any out going edges to the src 
    int start_index = graph -> out_offsets[trgt];
    int end_index = graph -> out_offsets[trgt + 1];
    for(int i = start_index; i < end_index; i++) {
        int edge = graph -> out_off_edges[i];
        if(graph -> edges[edge].trgtid == src) {
            return edge;
        }
    }
    return not_found;
}

void subsampling(Graph* graph, double threshold) {
    for(int i = 0; i < graph -> edges.size(); i++) {  
        if(graph -> edges[i].cost > threshold) {
            while(graph -> edges[i].cost > threshold) {
                split_bi_dir_edge(graph, i, bi_dir(graph, graph -> edges[i].id));        
            } 
        }
    }
    graph -> n_nodes = graph -> nodes.size();
    graph -> n_edges = graph -> edges.size();
    write_graph(graph, "graph_subsampled.dat");
    return;
}







































