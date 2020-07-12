#include "graph.h"


/*
    Graph class 
    this will store all the attributes of our graph (nodes, edges, )
*/
void check_boundaries(double latitude, double longitude, Graph* g) {
    if(g -> max_lat <= latitude) {
        g -> max_lat = latitude;
    }
    if( g -> min_lat > latitude) {
        g -> min_lat = latitude;
    }
    if(g -> max_long <= longitude) {
        g -> max_long = longitude;
    }
    if(g -> min_long > longitude) {
        g -> min_long = longitude;
    }
}

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
    for(int i = 0; i < IGNORE_LINES; i++) {
        getline(file, buffer);
    }
    /* read the total number of nodes and edges, store them in graph struct */
    file >> graph -> n_nodes >> graph -> n_edges;

    /* now read everything
       read line into buffer, scan line number, osmid, lat, long, .. (keep what matters) */
    getline(file, buffer);
    for(int i = 0; i < graph -> n_nodes; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct node n;
        vals >> n.id >> n.osmid >> n.lat >> n.longitude;
        graph -> nodes.push_back(n);
        check_boundaries(graph -> nodes[i].lat, graph -> nodes[i].longitude, graph);
    }
    for(int i = 0; i < graph -> n_edges; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct edge e;
        vals >> e.srcid >> e.trgtid;
        e.id = i;

        graph -> edges.push_back(e);
    }
    file.close();
    return;
}


bool compare_outdegree(struct edge edge1, struct edge edge2) {
    if(edge1.srcid == edge2.srcid) {
        return edge1.trgtid < edge2.trgtid;
    }
    return edge1.srcid < edge2.srcid;
}

bool compare_indegree(struct edge edge1, struct edge edge2) {
    if(edge1.trgtid == edge2.trgtid) {
        return edge1.srcid < edge2.srcid;
    }
    return edge1.trgtid < edge2.trgtid;
}

void outdeg_offset_array(Graph* graph, string file_name) {
    vector<struct edge> out_edges = graph -> edges;
    sort(out_edges.begin(), out_edges.end(), compare_outdegree);
    vector<int> offset{0};
    int index = 0;
    int k;
    int i;
    for(i = 0; i < out_edges.size(); i = k) {
        for(k = i; k < out_edges.size(); k++) {
            if(out_edges[k].srcid == index) {
                continue;
            }
            else {
                break;
            }
        }
        offset.push_back(k);
        index++;
    }
    int to_add = (graph -> n_nodes + 1) - offset.size();
    for(int j = 0; j < to_add; j++) {
        offset.push_back(i);
    }
    //write to a file (binary??) the edge id (or the full info for the edge struct?) array and the offset array 
    ofstream outdeg_file(file_name);

    //write the offsets to file
    outdeg_file << offset.size() << endl;
    outdeg_file << out_edges.size() << endl;

    for(i = 0; i < offset.size(); i++) {
        outdeg_file << offset[i] << endl;
    }
    //dont forget the cost as well
    for(i = 0; i < out_edges.size(); i++) {
        outdeg_file << out_edges[i].id << " " << out_edges[i].srcid << " " << out_edges[i].trgtid << " " << out_edges[i].cost << endl;
    }
    outdeg_file.close();
}

void indeg_offset_array(Graph* graph, string file_name) {
    vector<struct edge> in_edges = graph -> edges;
    sort(in_edges.begin(), in_edges.end(), compare_indegree);
    vector<int> offset{0};
    int index = 0;
    int k;
    int i;
    for(i = 0; i < in_edges.size(); i = k) {
        for(k = i; k < in_edges.size(); k++) {
            if(in_edges[k].trgtid == index) {
                continue;
            }
            else {

                break;
            }
        }
        offset.push_back(k);
        index++;
    }
    int to_add = (graph -> n_nodes + 1) - offset.size();
    for(int j = 0; j < to_add; j++) {
        offset.push_back(i);
    }

    ofstream outdeg_file(file_name);

    //write the offsets to file
    outdeg_file << offset.size() << endl;
    outdeg_file << in_edges.size() << endl;

    for(i = 0; i < offset.size(); i++) {
        outdeg_file << offset[i] << endl;
    }
    //dont forget the cost as well
    for(i = 0; i < in_edges.size(); i++) {
        outdeg_file << in_edges[i].id << " " << in_edges[i].srcid << " " << in_edges[i].trgtid << " " << in_edges[i].cost << endl;
    }
    outdeg_file.close();

}

void str_cnctd_cmpnt(Graph* graph) {

}