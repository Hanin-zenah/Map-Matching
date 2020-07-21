#include "graph_scc_lola.h"
#include <cmath>

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
    for(int i = 0; i < graph -> n_nodes+1; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct node n;
        vals >> n.id>> n.lat >> n.longitude;
        graph -> nodes.push_back(n);
        //check_boundaries(graph -> nodes[i].lat, graph -> nodes[i].longitude, graph);
    }
    for(int i = 0; i < graph -> n_edges+1; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct edge e;
        vals >> e.id>>e.srcid >> e.trgtid>>e.cost;
        graph -> edges.push_back(e);
    }

    //  bool bi_dir(struct edge edge1, struct edge edge2) {
    //  return (edge1.trgtid == edge2.srcid && edge1.srcid == edge2.trgtid);}

    // void sort_for_subsamp(Graph* graph) {
    // vector<struct edge> all_edges = graph -> edges;
    // sort(all_edges.begin(), all_edges.end(), bi_dir);
    //cout << "oo" << endl;
    for (int i = 0; i < graph -> n_edges; i++) {  // can I do this fater I close the file????
        int num_new, length_new, src_id, trgt_id;//how many new nodes
        double x1, x2, y1, y2, x_incre, y_incre;
        // edge curr_edge;
        // curr_edge = graph -> edges;
        if (graph->edges[i].cost>100){
        num_new=floor(graph->edges[i].cost/100);
        cout<< "long edge id: "<< graph->edges[i].id<<endl;
        num_new=floor(graph->edges[i].cost/100);
        length_new = graph -> edges[i].cost/(num_new+1);
        //cout<<"number of new nodes; "<< num_new<<" new length: "<<length_new<<endl;
        x1 = graph -> nodes[graph -> edges[i].srcid+1].lat; // the node id starts with 0, so need to + 1
        y1 = graph -> nodes[graph -> edges[i].srcid+1].longitude;
        //src_id = graph -> nodes[graph -> edges[i].srcid+1].id;
        x2 = graph -> nodes[graph -> edges[i].trgtid+1].lat;
        y2 = graph -> nodes[graph -> edges[i].trgtid+1].longitude;
        //trgt_id = graph -> nodes[graph -> edges[i].trgtid+1].id;
        x_incre=(x2-x1)/(num_new+1);
        y_incre=(y2-y1)/(num_new+1);
        //cout<<"source id: "<< src_id<< " target id: "<<trgt_id<<endl;
        //cout<< "source x1: " << x1 << "target x2: "<< x2<< " x_incre: "<< x_incre <<"source y1: "<<y1<<"target y2: "<< y2<< " y_incre: "<< y_incre <<endl;
        struct node nd;
        struct edge e;
        int ori_trg; //save the original target node
        ori_trg = graph->edges[i].trgtid;
        for (int j = 0; j < num_new; j++){
            nd.id = graph -> n_nodes;
            cout<<"new nd.id: "<<nd.id<<endl;
            graph -> n_nodes= graph -> n_nodes+1;
            nd.lat= x1 + (1+j)*x_incre;     
            nd.longitude = y1+ (1+j)*y_incre;
            //cout<<"x1: "<< x1 << " x2: "<< x2<<" x_incre: " << x_incre <<" nd.lat:  "<<nd.lat<<endl;
            cout<<"new node: "<<nd.id << " " << nd.lat<<" "<< nd.longitude <<endl;
            graph -> nodes.push_back(nd);
            cout<<"done looping for creating new node(s)"<<endl;
            break;
            if (j==0){
                cout<<"change the target id"<<endl;
                graph->edges[i].trgtid=nd.id;
                }
            else {
                cout<<"intermediate nodes"<<endl;
                e.id = graph -> n_edges;
                graph -> n_edges = graph -> n_edges+1;
                e.trgtid  = nd.id;
                e.srcid = graph -> n_nodes - 1; //last created node will be the new source node graph->edges[i-1].id;
                e.cost = length_new;
                graph -> edges.push_back(e);
                cout<<"new edge: "<<e.id<<" "<<e.srcid<<" "<<e.trgtid<<" "<<e.cost<<endl;

            }}
            //graph -> nodes.push_back(nd);
        // number of new edges = number of new nodes + 1, need to create the last edge after looping through nodes
        e.id = graph -> n_edges;
        graph -> n_edges = graph -> n_edges+1;
        e.srcid = graph -> n_nodes - 1; // last created node
        e.trgtid = ori_trg; //the original target node
        e.cost = length_new ;
        cout<<"new edge: "<<e.id<<" "<<e.srcid<<" "<<e.trgtid <<endl;
        graph -> edges.push_back(e);
        }

}
    file.close();
    return;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name";
        return 1;
    }

    //read file
    Graph graph = GRAPH_INIT; 
    read_file(argv[1], &graph);


    return 0;
}
