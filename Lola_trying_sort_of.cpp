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
    //cout<<"graph -> n_nodes: "<<graph -> n_nodes<<" graph -> n_edges: "<<graph -> n_edges<<endl;

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
    getline(file, buffer);
    for(int i = 0; i < graph -> n_edges; i++) {
        //cout<<"i: "<<i<<endl;
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

void subsampling(Graph* graph, double threshold){
for (int i = 0; i < graph -> n_edges; i++) {  // can I do this fater I close the file????
    int num_new, length_new, n_nodes, n_edges;//how many new nodes
    double x1, x2, y1, y2, x_incre, y_incre;
    if (graph->edges[i].cost>100){
    num_new=floor(graph->edges[i].cost/threshold);
    length_new = graph -> edges[i].cost/(num_new+1);
    x1 = graph -> nodes[graph -> edges[i].srcid+1].lat; // the node id starts with 0, so need to + 1
    y1 = graph -> nodes[graph -> edges[i].srcid+1].longitude;
    x2 = graph -> nodes[graph -> edges[i].trgtid+1].lat;
    y2 = graph -> nodes[graph -> edges[i].trgtid+1].longitude;
    x_incre=(x2-x1)/(num_new+1);
    y_incre=(y2-y1)/(num_new+1);
    struct node nd;
    struct edge e;
    int ori_trg; //save the original target node
    ori_trg = graph->edges[i].trgtid;
    
    for (int j = 0; j < num_new; j++){
        nd.id = graph -> nodes.size();
        cout<<"new nd.id: "<<nd.id<<endl;
        nd.lat= x1 + (1+j)*x_incre;     
        nd.longitude = y1+ (1+j)*y_incre;
        graph -> nodes.push_back(nd);
        if (j==0){
            graph->edges[i].trgtid=nd.id;
            graph->edges[i].cost=length_new;
            }
        else {
            e.id = graph -> edges.size();
            e.srcid = graph -> nodes.size() - 2; //last created node will be the new source node graph->edges[i-1].
            e.trgtid  = graph -> nodes.size() - 1;
            e.cost = length_new;
            graph -> edges.push_back(e);
        }}

    e.id = graph -> edges.size();//
    e.srcid = graph -> nodes.size() - 1;//n_nodes; // last created node
    e.trgtid = ori_trg; //the original target node
    e.cost = length_new ;
    graph -> edges.push_back(e);
    }}}


int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name";
        return 1;
    }

    vector<int> in_offset{0, 2, 3, 4, 6, 7};
    // for(int i = 0; i < in_offset.size(); i++) {
    // cout<< in_offset[i] << endl;}
    vector<int> out_offset{0, 1, 3, 4, 5, 7};

    //read file
    Graph graph = GRAPH_INIT; 
    read_file(argv[1], &graph);
    
    subsampling(&graph, 100);


    return 0;
}
