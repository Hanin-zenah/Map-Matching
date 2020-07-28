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

//void break_edge(Graph* graph, int edgeID, double threshold){

void split_edge(Graph* graph, int edgeID, vector<int>& in_edge, vector<int>& in_offset, vector<int>& out_edge, vector<int>& out_offset){
    int n_nodes, n_edges;//how many new nodes
    double x1, x2, y1, y2, x_incre, y_incre, num_new, length_new;
    length_new = graph -> edges[edgeID].cost/2;
    graph -> in_edge = in_edge;
    graph -> in_offset = in_offset;
    graph -> out_edge = out_edge;
    graph -> out_offset = out_offset;

    x1 = graph -> nodes[graph -> edges[edgeID].srcid+1].lat; // the node id sta
    y1 = graph -> nodes[graph -> edges[edgeID].srcid+1].longitude;
    x2 = graph -> nodes[graph -> edges[edgeID].trgtid+1].lat;
    y2 = graph -> nodes[graph -> edges[edgeID].trgtid+1].longitude;
    // x_incre=(x2-x1)/2;
    // y_incre=(y2-y1)/2;
    struct node nd;
    struct edge e;
    int ori_trg; //save the original target node
    ori_trg = graph->edges[edgeID].trgtid;
    nd.id = graph -> nodes.size();
    nd.lat= (x1+x2)/2;   
    nd.longitude = (y1+y2)/2;
    graph -> nodes.push_back(nd);
    // graph -> out_offset.push_back(graph -> edges.size() + 1);
    // graph -> in_offset.push_back(graph -> edges.size() + 1);
    graph -> edges[edgeID].trgtid=nd.id;
    graph -> edges[edgeID].cost=length_new;
    e.id = graph -> edges.size();//
    e.srcid = graph -> nodes.size() - 1;//n_nodes; // last created node
    e.trgtid = ori_trg; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);
    cout<<"out_edge pushback: "<<e.id<<endl;
    graph -> out_edge.push_back(e.id);
    pair<bool, int> result = findInVector<int>(graph -> in_edge, edgeID);
    //nt pos_in_inedge = result.second;
    graph -> in_edge[result.second] = e.id;
    graph -> in_edge.push_back(edgeID); //need to do swapping
    //cout<<"out_offset pushback: "<<graph -> edges.size()<<endl;
    graph -> out_offset.push_back(graph -> edges.size());
    graph -> in_offset.push_back(graph -> edges.size());
    return;}

void split_bi_dir_edge(Graph* graph, int edgeID1, int edgeID2, vector<int>& in_edge, vector<int>& in_offset, vector<int>& out_edge, vector<int>& out_offset){
    int n_nodes, n_edges;//how many new nodes
    double x1, x2, y1, y2, x_incre, y_incre, num_new, length_new;
    length_new = graph -> edges[edgeID1].cost/2; // can use either edgeID1 or edgeID2
    graph -> in_edge = in_edge;
    graph -> in_offset = in_offset;
    graph -> out_edge = out_edge;
    graph -> out_offset = out_offset;

    x1 = graph -> nodes[graph -> edges[edgeID1].srcid+1].lat; // can use either edgeID1 or edgeID2
    y1 = graph -> nodes[graph -> edges[edgeID1].srcid+1].longitude;
    x2 = graph -> nodes[graph -> edges[edgeID1].trgtid+1].lat;
    y2 = graph -> nodes[graph -> edges[edgeID1].trgtid+1].longitude;
    struct node nd;
    struct edge e;
    int ori_trg1, ori_trg2; //save the original target node
    ori_trg1 = graph->edges[edgeID1].trgtid;
    ori_trg2 = graph->edges[edgeID2].trgtid;
    nd.id = graph -> nodes.size();
    nd.lat= (x1+x2)/2;   
    nd.longitude = (y1+y2)/2;
    graph -> nodes.push_back(nd);
    graph->edges[edgeID1].trgtid=nd.id;
    graph->edges[edgeID1].cost=length_new;
    e.id = graph -> edges.size();//
    e.srcid = graph -> nodes.size() - 1;//n_nodes; // last created node
    e.trgtid = ori_trg1; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);

    graph -> out_edge.push_back(e.id);
    pair<bool, int> result1 = findInVector<int>(graph -> in_edge, edgeID1);
    graph -> in_edge[result1.second] = e.id;
    graph -> in_edge.push_back(edgeID1); //need to do swapping

    //repeat the same process for the other edge
    graph->edges[edgeID2].trgtid=nd.id;
    graph->edges[edgeID2].cost=length_new;
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
    return;}

int bi_dir(Graph* graph, int edgeID, vector<int>& in_edgeID, vector<int>& in_offset){
        // cout<<"i = "<<i<<" i+1 = "<<i+1<<endl;
    int b_edge = -1;
    int index, start_in_offset, end_in_offset, start_in_edge_array, end_in_edge_array;
    //pair<bool, int> result = findInVector<int>(in_edgeID, edgeID);
    start_in_offset = graph -> edges[edgeID].srcid;
    end_in_offset = start_in_offset + 1; 
    // cout<<"start_pos_in_offset "<<start_in_offset<<endl;
    // cout<<"end_pos_in_offset "<<end_in_offset<<endl;
    start_in_edge_array = in_offset[start_in_offset];
    end_in_edge_array = in_offset[end_in_offset];
    // cout<<"start_pos_in_edge_array "<<start_in_edge_array<<endl;//their starting position in the inEdge array
    // cout<<"end_pos_in_edge_array "<<end_in_edge_array<<endl;//looping until this position
    cout<<" "<<endl;
    for (int j = start_in_edge_array; j < end_in_edge_array; j++){ //finding what nodes goes to the target node in the given edge.id
        int start_in_offset2, end_in_offset2, start_in_edge_array2, end_in_edge_array2;
        //cout<<"j= "<<j<<" in_edgeID[j] "<<in_edgeID[j]<<endl;
        start_in_offset2 = graph->edges[in_edgeID[j]].srcid; //graph->edges[in_edgeID[j]].srcid are the nodes that goes to the same target node in given edge.id
        end_in_offset2 = start_in_offset2 + 1;
        //cout<<"start_in_offset2: "<<start_in_offset2<<" end_in_offset2: "<<end_in_offset2<<endl;
        
        if (graph->edges[edgeID].trgtid == graph->edges[in_edgeID[j]].srcid){
            cout<<"push back: graph->edges[in_edgeID[k]].id: "<<graph->edges[in_edgeID[j]].id<<endl;
            cout<<" "<<endl;
            b_edge = graph->edges[in_edgeID[j]].id;
            }
    }
    return b_edge;
}

void subsampling(Graph* graph, double threshold, vector<int>& in_edge, vector<int>& in_offset, vector<int>& out_edge, vector<int>& out_offset){
    for (int i = 0; i < graph -> edges.size(); i++) {  
        if (graph -> edges[i].cost > threshold) {
            int b_edge = bi_dir(graph, graph -> edges[i].id, in_edge, in_offset);
            if (b_edge != -1){
                for (int k = 0; graph -> edges[i].cost > threshold; k++){
                split_bi_dir_edge(graph, i, b_edge, in_edge, in_offset, out_edge, out_offset);}
   
            } 
            else {for (int k = 0; graph -> edges[i].cost > threshold; k++){
                split_edge(graph, i, in_edge, in_offset, out_edge, out_offset);}
                }
            }
        }
    return;}


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
    subsampled_file<< out_edge[i] << endl;}

    subsampled_file<< "out_offset" << endl;
    for(int i = 0; i < out_offset.size(); i++) {
    subsampled_file<< out_offset[i] << endl;}

    subsampled_file<< "in_edge" << endl;
    for(int i = 0; i < in_edge.size(); i++) {
    subsampled_file<< in_edge[i] << endl;}

    subsampled_file<< "in_offset" << endl;
    for(int i = 0; i < in_offset.size(); i++) {
    subsampled_file<< in_offset[i] << endl;}

    subsampled_file.close();
    return;
    }


