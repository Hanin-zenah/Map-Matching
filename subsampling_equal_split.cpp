#include "graph_scc_lola_v3.h"
#include <cmath>
#include <vector> 

void read_file(string file_name, Graph* graph) {
    if(file_name.empty()) {
        return;
    }
    ifstream file; 
    file.open(file_name);
    if(!file) {
        cerr << "Unable to open file";
        return;
    }

    string buffer;
    file >> graph -> n_nodes >> graph -> n_edges;

    /* now read everything
       read line into buffer, scan line number, osmid, lat, long, .. (keep what matters) */
    getline(file, buffer);// or else the first line is: 0 1.4822e-323 0
    for(int i = 0; i < graph -> n_nodes; i++) {
        //cout<<"i: "<<i<<endl;
        getline(file, buffer);
        istringstream vals(buffer);
        struct node n;
        vals >> n.id>> n.lat >> n.longitude;
        //cout<<n.id<<" " << n.lat<<" " << n.longitude<<endl;
        graph -> nodes.push_back(n);
    }
    for(int i = 0; i < graph -> n_edges; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct edge e;
        vals >> e.id >> e.srcid >> e.trgtid>>e.cost;
        //cout << e.id<<" "<<e.srcid<<" " << e.trgtid<<" "<<e.cost<<endl;
        graph -> edges.push_back(e);
    }
    file.close();
    return;
}

void split_edge(Graph* graph, int edgeID) {
    int n_nodes, n_edges; //how many new nodes and edges 
    double x1, x2, y1, y2, x_incre, y_incre, num_new;
    double length_new = graph -> edges[edgeID].cost/2;

    /* dont need the +1 */
    x1 = graph -> nodes[graph -> edges[edgeID].srcid].lat; // the node id sta
    y1 = graph -> nodes[graph -> edges[edgeID].srcid].longitude;
    x2 = graph -> nodes[graph -> edges[edgeID].trgtid].lat;
    y2 = graph -> nodes[graph -> edges[edgeID].trgtid].longitude;

    struct node nd; //new node to be added 
    struct edge e;
    int ori_trg = graph->edges[edgeID].trgtid; //save the original target node
    nd.id = graph -> nodes.size(); 
    nd.lat = (x1 + x2) / 2;   
    nd.longitude = (y1 + y2) / 2;
    graph -> nodes.push_back(nd);

    graph -> edges[edgeID].trgtid = nd.id;
    graph -> edges[edgeID].cost = length_new;
    e.id = graph -> edges.size();
    e.srcid = graph -> nodes.size() - 1;//n_nodes; // last created node
    e.trgtid = ori_trg; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);

    graph -> n_nodes += 1;
    graph -> n_edges += 1;

    cout<<"out_edge pushback: "<<e.id<<endl;
    graph -> out_edge.push_back(e.id);
    pair<bool, int> result = findInVector<int>(graph -> in_edge, edgeID);
    if(result.first) {
        graph -> in_edge[result.second] = e.id;
    }
    graph -> in_edge.push_back(edgeID); //need to do swapping
    graph -> in_offset.push_back(graph -> edges.size());

    graph -> out_offset.push_back(graph -> edges.size()); 

    return;
}

void split_bi_dir_edge(Graph* graph, int edgeID1, int edgeID2) {
    int n_nodes, n_edges; //how many new nodes
    double x1, x2, y1, y2, x_incre, y_incre, num_new, length_new;
    length_new = graph -> edges[edgeID1].cost / 2; // can use either edgeID1 or edgeID2

    x1 = graph -> nodes[graph -> edges[edgeID1].srcid].lat; // can use either edgeID1 or edgeID2
    y1 = graph -> nodes[graph -> edges[edgeID1].srcid].longitude;
    x2 = graph -> nodes[graph -> edges[edgeID1].trgtid].lat;
    y2 = graph -> nodes[graph -> edges[edgeID1].trgtid].longitude;

    struct node nd;
    struct edge e;
    int ori_trg1, ori_trg2; //save the original target node
    ori_trg1 = graph->edges[edgeID1].trgtid;
    ori_trg2 = graph->edges[edgeID2].trgtid;
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

    graph -> out_edge.push_back(e.id);
    pair<bool, int> result1 = findInVector<int>(graph -> in_edge, edgeID1);
    graph -> in_edge[result1.second] = e.id;
    graph -> in_edge.push_back(edgeID1); //need to do swapping

    //repeat the same process for the other edge
    graph->edges[edgeID2].trgtid = nd.id;
    graph->edges[edgeID2].cost = length_new;
    e.id = graph -> edges.size();//
    e.srcid = graph -> nodes.size() - 1;//n_nodes; // last created node
    e.trgtid = ori_trg2; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);

    graph -> out_edge.push_back(e.id);
    pair<bool, int> result2 = findInVector<int>(graph -> in_edge, edgeID2);
    graph -> in_edge[result2.second] = e.id;
    graph -> in_edge.push_back(edgeID2); //need to do swapping

    graph -> out_offset.push_back(graph -> edges.size()); //edge ID starts with zero, so the sentinal val = size = last edge id +1
    graph -> in_offset.push_back(graph -> edges.size());
    return;
}

int bi_dir(Graph* graph, int edgeID) {
    int b_edge = -1;
    int index, start_in_offset, end_in_offset, start_in_edge_array, end_in_edge_array;
    start_in_offset = graph -> edges[edgeID].srcid;
    end_in_offset = start_in_offset + 1; 

    start_in_edge_array = graph -> in_offset[start_in_offset];
    end_in_edge_array = graph -> in_offset[end_in_offset];

    for (int j = start_in_edge_array; j < end_in_edge_array; j++) { //finding what nodes goes to the target node in the given edge.id
        int start_in_offset2, end_in_offset2, start_in_edge_array2, end_in_edge_array2;

        start_in_offset2 = graph -> edges[graph -> in_edge[j]].srcid; //graph->edges[in_edgeID[j]].srcid are the nodes that goes to the same target node in given edge.id
        end_in_offset2 = start_in_offset2 + 1;

        if (graph -> edges[edgeID].trgtid == graph -> edges[graph -> in_edge[j]].srcid) {
            cout<<"push back: graph->edges[in_edgeID[k]].id: "<<graph->edges[graph -> in_edge[j]].id<<endl;
            cout<<" "<<endl;
            b_edge = graph->edges[graph -> in_edge[j]].id;
        }
    }
    return b_edge;
}

void subsampling(Graph* graph, double threshold, vector<int>& in_edge, vector<int>& in_offset, vector<int>& out_edge, vector<int>& out_offset) {
    graph -> in_edge = in_edge;
    graph -> in_offset = in_offset;
    graph -> out_edge = out_edge;
    graph -> out_offset = out_offset;
    for (int i = 0; i < graph -> edges.size(); i++) {  
        if (graph -> edges[i].cost > threshold) {
            int b_edge = bi_dir(graph, graph -> edges[i].id);
            if (b_edge != -1){
                for (int k = 0; graph -> edges[i].cost > threshold; k++) {
                    //split_bi_dir_edge(graph, i, b_edge, graph -> in_edge, graph -> in_offset, graph -> out_edge, graph -> out_offset);}
                    split_bi_dir_edge(graph, i, b_edge);        
                } 
            }
            else {
                for (int k = 0; graph -> edges[i].cost > threshold; k++) {
                    //split_edge(graph, i, graph -> in_edge, graph -> in_offset, graph -> out_edge, graph -> out_offset);}
                    split_edge(graph, i);
                }
            }
        }
    }
    return;
}


void subsampled_graph(Graph* graph, string file_name) {
    cout<<"writing file";
    vector<struct node> all_nodes = graph -> nodes;
    vector<struct edge> all_edges = graph -> edges;
    vector<int> out_edge = graph -> out_edge;
    vector<int> out_offset = graph -> out_offset;
    vector<int> in_edge = graph -> in_edge;
    vector<int> in_offset = graph -> in_offset;

    ofstream subsampled_file(file_name);

    subsampled_file << all_nodes.size() << endl;
    subsampled_file << all_edges.size() << endl;

    for(int i = 0; i < all_nodes.size(); i++) {
        subsampled_file << all_nodes[i].id << " " << all_nodes[i].lat << " " << all_nodes[i].longitude << endl;
    }

    for(int i = 0; i < all_edges.size(); i++) {
        subsampled_file << all_edges[i].id << " " << all_edges[i].srcid << " " << all_edges[i].trgtid << " " << all_edges[i].cost << endl;
    }

    subsampled_file<< "out_edge" << endl;
    for(int i = 0; i < out_edge.size(); i++) {
        subsampled_file<< out_edge[i] << endl;
    }

    subsampled_file<< "out_offset" << endl;
    for(int i = 0; i < out_offset.size(); i++) {
        subsampled_file<< out_offset[i] << endl;
    }

    subsampled_file<< "in_edge" << endl;
    for(int i = 0; i < in_edge.size(); i++) {
        subsampled_file<< in_edge[i] << endl;
    }

    subsampled_file<< "in_offset" << endl;
    for(int i = 0; i < in_offset.size(); i++) {
        subsampled_file<< in_offset[i] << endl;
    }

    subsampled_file.close();
    return;
}


