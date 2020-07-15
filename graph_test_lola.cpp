#include "graph.h"
#include "scale_projection.h"

Euc_distance ed;
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
        // n.lat=ed.lat_mercator_proj(n.lat);
        // n.longitude=ed.lon_mercator_proj(n.lat);
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

    //overwrite the node's coordinates in mercator projection
    for(int i = 0; i < graph -> n_nodes; i++) {
    graph -> nodes[i].lat=ed.lat_mercator_proj(graph -> nodes[i].lat,graph -> min_lat);
    graph -> nodes[i].longitude=ed.lon_mercator_proj(graph -> nodes[i].longitude, graph -> min_long);}
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
    //write to a file (binary??) the edge array and the offset array 
    ofstream outdeg_file(file_name);

    //write the offsets to file
    outdeg_file << offset.size() << endl;
    outdeg_file << out_edges.size() << endl;

    for(i = 0; i < offset.size(); i++) {
        outdeg_file << offset[i] << endl;
    }
    for(i = 0; i < out_edges.size(); i++) {
        outdeg_file << out_edges[i].id << " " << out_edges[i].srcid << " " << out_edges[i].trgtid << " " <<graph -> nodes[graph -> edges[i].srcid].lat<<" " <<graph -> nodes[graph -> edges[i].srcid].longitude<<" "<< graph -> nodes[graph -> edges[i].trgtid].lat<<" " <<graph -> nodes[graph -> edges[i].trgtid].longitude<<" "<<out_edges[i].cost << endl;
    }
    outdeg_file.close();

    graph -> offsets = offset;
    for(int i = 0; i < out_edges.size(); i++) {
        graph -> off_edges.push_back(out_edges[i].id);
    }
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
        outdeg_file << in_edges[i].id << " " << in_edges[i].srcid << " " << in_edges[i].trgtid << " " <<graph -> nodes[graph -> edges[i].srcid].lat<<" " <<graph -> nodes[graph -> edges[i].srcid].longitude<<" "<< graph -> nodes[graph -> edges[i].trgtid].lat<<" " <<graph -> nodes[graph -> edges[i].trgtid].longitude<<" " << in_edges[i].cost << endl;
    }
    outdeg_file.close();

    // graph -> offsets = offset;
    // for(int i = 0; i < in_edges.size(); i++) {
    //     graph -> off_edges[i] = in_edges[i].id;
    // }
}


//void get_incident(struct node node) {
    //get the node id and calculate the number of neighbours
    //offset[node+1] - offset[node]
    //traverse the edges array from edges[offset[node]] to edges[offset[node+1]] and get the target node id and modify its parent
    //return the vector? containg the nodes? or a vector containing the node ids then traverse the nodes vector in graph (((containing the indices of the nodes)))
//}

// void DFS_visit(struct node cur_node, int index, vector<bool> visited, vector<struct node> parent, Graph* graph) {
    // visited[index] = true;
    //visit all the neighbours of cur_node (check https://thispointer.com/c-how-to-find-an-element-in-vector-and-get-its-index/#:~:text=If%20element%20is%20found%20then%20we%20can,index%20from%20the%20iterator%20i.e.&text=int%20index%20%3D%20std%3A%3Adistance,%3D%20std%3A%3Adistance(vecOfNums.) to get the index of an element in a vector)
    //for()
// }
// 
// vector<struct node> DFS(Graph* graph) {
    //initialize visited and parent vectors for DFS
    // vector<bool> visited(graph -> n_nodes, false);
    // vector<struct node> parent(graph -> n_nodes, DEF_NODE);
// 
    // for(int i = 0; i < graph -> n_nodes; i++) {
        // if(!visited[i]) {
            // DFS_visit(graph -> nodes[i], i, visited, parent, graph);
        // }
    // }
    // return parent;
// }
// 

//void strongly_connected_components(Graph* graph) {
    //dfs and add nodes to stack 
    //tanspose graph 
    //pop every node on stack and do dfs again 
    //

//}