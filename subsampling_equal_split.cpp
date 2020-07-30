#include "graph_subsampling.h"
#include "graph.h"
#include <cmath>

void split_bi_dir_edge(Graph* graph, int edgeID1, int edgeID2) {
    double x1, x2, y1, y2, num_new;
    double length_new = graph -> edges[edgeID1].cost / 2; // can use either edgeID1 or edgeID2

    x1 = graph -> nodes[graph -> edges[edgeID1].srcid].lat; // can use either edgeID1 or edgeID2
    y1 = graph -> nodes[graph -> edges[edgeID1].srcid].longitude;
    x2 = graph -> nodes[graph -> edges[edgeID1].trgtid].lat;
    y2 = graph -> nodes[graph -> edges[edgeID1].trgtid].longitude;

    struct node nd;
    struct edge e;
    int ori_trg1, ori_trg2; //save the original target node
    ori_trg1 = graph->edges[edgeID1].trgtid;
    nd.id = graph -> nodes.size();
    nd.lat= (x1+x2)/2;   
    nd.longitude = (y1+y2)/2;
    graph -> nodes.push_back(nd);
    graph -> edges[edgeID1].trgtid = nd.id;
    graph -> edges[edgeID1].cost = length_new;
    e.id = graph -> edges.size();
    e.srcid = graph -> nodes.size() - 1; //n_nodes; // last created node
    e.trgtid = ori_trg1; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);

    graph -> out_off_edges.push_back(e.id);
    pair<bool, int> result1 = findInVector<int>(graph -> in_off_edges, edgeID1);
    graph -> in_off_edges[result1.second] = e.id;
    graph -> in_off_edges.push_back(edgeID1); //need to do swapping

    //repeat the same process for the other edge
    if (edgeID2 != -1){
        ori_trg2 = graph->edges[edgeID2].trgtid;
        graph->edges[edgeID2].trgtid = nd.id;
        graph->edges[edgeID2].cost = length_new;
        e.id = graph -> edges.size();//
        e.srcid = graph -> nodes.size() - 1;//n_nodes; // last created node
        e.trgtid = ori_trg2; //the original target node
        e.cost = length_new ;
        graph -> edges.push_back(e);

        graph -> out_off_edges.push_back(e.id);
        pair<bool, int> result2 = findInVector<int>(graph -> in_off_edges, edgeID2);
        graph -> in_off_edges[result2.second] = e.id;
        graph -> in_off_edges.push_back(edgeID2); //need to do swapping
    }

    graph -> out_offsets.push_back(graph -> edges.size()); //edge ID starts with zero, so the sentinal val = size = last edge id +1
    graph -> in_offsets.push_back(graph -> edges.size());
    return;
}

int bi_dir(Graph* graph, int edgeID) {
    int b_edge = -1;
    int index, start_in_offsets, end_in_offsets, start_in_edge_array, end_in_edge_array;
    start_in_offsets = graph -> edges[edgeID].srcid;
    end_in_offsets = start_in_offsets + 1; 

    start_in_edge_array = graph -> in_offsets[start_in_offsets];
    end_in_edge_array = graph -> in_offsets[end_in_offsets];

    for (int j = start_in_edge_array; j < end_in_edge_array; j++) { //finding what nodes goes to the target node in the given edge.id
        int start_in_offsets2, end_in_offsets2, start_in_edge_array2, end_in_edge_array2;

        start_in_offsets2 = graph -> edges[graph -> in_off_edges[j]].srcid; //graph->edges[in_edgeID[j]].srcid are the nodes that goes to the same target node in given edge.id
        end_in_offsets2 = start_in_offsets2 + 1;

        if (graph -> edges[edgeID].trgtid == graph -> edges[graph -> in_off_edges[j]].srcid) {
            b_edge = graph->edges[graph -> in_off_edges[j]].id;
        }
    }
    return b_edge;
}

void subsampling(Graph* graph, double threshold) { //, vector<int>& in_edge, vector<int>& in_offset, vector<int>& out_edge, vector<int>& out_offset) {
    for (int i = 0; i < graph -> edges.size(); i++) {  
        if (graph -> edges[i].cost > threshold) {
            for (int k = 0; graph -> edges[i].cost > threshold; k++) {
                split_bi_dir_edge(graph, i, bi_dir(graph, graph -> edges[i].id));        
            } 
        }
    }
    write_graph(graph, "graph_subsampled.dat");
    return;
}


void output_graph(Graph* graph, string file_name) {
    vector<struct node> all_nodes = graph -> nodes;
    vector<struct edge> all_edges = graph -> edges;
    vector<int> out_off_edges = graph -> out_off_edges;
    vector<int> out_offsets = graph -> out_offsets;
    vector<int> in_off_edges = graph -> in_off_edges;
    vector<int> in_offsets = graph -> in_offsets;

    ofstream txt_file(file_name);

    txt_file << all_nodes.size() << endl;
    txt_file << all_edges.size() << endl;

    for(int i = 0; i < all_nodes.size(); i++) {
        txt_file << all_nodes[i].id << " " << all_nodes[i].osmid << " " << all_nodes[i].lat << " " << all_nodes[i].longitude << endl;
    }

    for(int i = 0; i < all_edges.size(); i++) {
        txt_file << all_edges[i].id << " " << all_edges[i].srcid << " " << all_edges[i].trgtid << " " << all_edges[i].cost << endl;
    }

    txt_file<< "out_edge" << endl;
    for(int i = 0; i < out_off_edges.size(); i++) {
        txt_file<< out_off_edges[i] << endl;
    }

    txt_file<< "out_offset" << endl;
    for(int i = 0; i < out_offsets.size(); i++) {
        txt_file<< out_offsets[i] << endl;
    }

    txt_file<< "in_edge" << endl;
    for(int i = 0; i < in_off_edges.size(); i++) {
        txt_file<< in_off_edges[i] << endl;
    }

    txt_file<< "in_offset" << endl;
    for(int i = 0; i < in_offsets.size(); i++) {
        txt_file<< in_offsets[i] << endl;
    }

    txt_file.close();
    return;
}


