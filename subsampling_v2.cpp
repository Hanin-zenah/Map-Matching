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
    /* skip the first five lines */
    // for(int i = 0; i < IGNORE_LINES; i++) {
        // getline(file, buffer);
    // }
    /* read the total number of nodes and edges, store them in graph struct */
    file >> graph -> n_nodes >> graph -> n_edges;

    /* now read everything
       read line into buffer, scan line number, osmid, lat, long, .. (keep what matters) */
    for(int i = 0; i < graph -> n_nodes; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct node n;
        vals >> n.id >> n.osmid >> n.lat >> n.longitude;
        // n.lat=ed.lat_mercator_proj(n.lat);
        // n.longitude=ed.lon_mercator_proj(n.lat);
        graph -> nodes.push_back(n);
        //check_boundaries(graph -> nodes[i].lat, graph -> nodes[i].longitude, graph);
    }
    for(int i = 0; i < graph -> n_edges; i++) {
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

    for (int i = 0; i < graph -> n_edges; i++) {  // can I do this fater I close the file????
        int num_new;//how many new nodes
        double x1, x2, y1, y2, x_incre, y_incre;
        // edge curr_edge;
        // curr_edge = graph -> edges;
        if (graph->edges[i].cost>100){
        num_new=floor(graph->edges[i].cost/100);
        x1 = graph -> nodes[graph -> edges[i].srcid].lat;
        x2 = graph -> nodes[graph -> edges[i].trgtid].lat;
        y1 = graph -> nodes[graph -> edges[i].srcid].longitude;
        y2 = graph -> nodes[graph -> edges[i].trgtid].longitude;
        x_incre=(x2-x1)/(num_new+1);
        y_incre=(y2-y1)/(num_new+1);
        struct node nd;
        struct edge e;
        int ori_trg; //save the original target node
        ori_trg = graph->edges[i].trgtid;
        //if (bool bi_dir(graph->edges[i], graph->edges[i+1])){} // discovering bidirectional edges
        //only add edges for the second direction, as we already have the nodes
        for (int j = 0; j < num_new+1; i=j++){
            nd.id = graph -> n_nodes+j;
            graph -> n_nodes= graph -> n_nodes+1;
            nd.lat= x1 + (1+j)*x_incre;     
            nd.longitude = y1+ (1+j)*y_incre;
            graph -> nodes.push_back(nd);
            if (j==0){
                graph->edges[i].trgtid=nd.id;
                }
            // else if (j=num_new){
                // e.id = graph -> n_edges+1;
                // graph -> n_edges = graph -> n_edges+1;
                // e.srcid = nd.id;
                // e.trgtid = graph->nodes[nd.id - 1].id; //last created node will be the new source node
                // graph -> edges.push_back(e);
                // }
            else {
                e.id = graph -> n_edges+1;
                graph -> n_edges = graph -> n_edges+1;
                e.trgtid  = nd.id;
                e.srcid = graph->nodes[nd.id - 1].id; //last created node will be the new source node graph->edges[i-1].id;
                graph -> edges.push_back(e);

            }}
        // number of new edges = number of new nodes + 1, need to create the last edge after looping through nodes
        e.id = graph -> n_edges+1;
        graph -> n_edges = graph -> n_edges+1;
        e.srcid = graph->nodes[graph -> n_nodes].id; // last created node
        e.trgtid = ori_trg; //the original target node
        graph -> edges.push_back(e);
        }
    //graph->edges[graph->edges[i]].srcid;
}
    file.close();
    return;
}

void subsampled_array(Graph* graph, string file_name) {
    //vector<struct node> all_nodes = graph -> nodes;
    vector<struct edge> all_edges = graph -> edges;

//ofstream outdeg_file(file_name);
    ofstream subsampling_file(file_name);
//write the offsets to file
//subsampling_file << all_nodes.size() << endl;
//subsampling_file << all_edges.size() << endl;

    for(int i = 0; i < all_edges.size(); i++) {
     subsampling_file << all_edges[i].id << " " << all_edges[i].srcid << " " << all_edges[i].trgtid << " " << all_edges[i].cost << endl;
    }
    subsampling_file.close();}