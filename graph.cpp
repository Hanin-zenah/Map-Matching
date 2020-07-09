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

/* compares the src ids of the edges */
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

void outdeg_offset_array(Graph* graph) {
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
}

void indeg_offset_array(Graph* graph) {
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

}

// int main() {
    // Graph graph = {
        // 0, 0, INT_MAX, INT_MIN, INT_MAX, INT_MIN
    // };
    // read_file("saarland-200601.car.txt", &graph);
    // cout.precision(18);
    // cout << "Max lat: " << graph.max_lat << "\nMin lat: " << graph.min_lat << "\nMax long: " << graph.max_long << "\nMin long: " << graph.min_long << endl;
    // return 0;
// }
