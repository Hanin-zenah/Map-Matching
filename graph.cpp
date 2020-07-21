#include "graph.h"
#include "scale_projection.h"





/*
    Graph class 
    this will store all the attributes of our graph (nodes, edges, )
*/

//add get out deg and get outedge from a node id given the wanted k edge 

void write_graph(Graph* graph, string file_name) { 
    ofstream file(file_name);
    for(int i = 0; i < graph -> n_edges; i++) {
        //x y x y 
        int source = graph -> edges[i].srcid;
        int target = graph -> edges[i].trgtid;

        file << graph -> nodes[source].longitude << " " << graph -> nodes[source].lat << " " << graph -> nodes[target].longitude << " " << graph -> nodes[target].lat << endl;
    }
    file.close();
}

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
    for(int i = 0; i < graph -> n_nodes ; i++) {
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

    //write a .dat file containing the graph's longitude and latitude coordinates
    write_graph(graph, "graph_lat_lon.dat");

    Euc_distance ed;

    //overwrite the node's coordinates in mercator projection
    for(int i = 0; i < graph -> n_nodes; i++) {
        graph -> nodes[i].lat = ed.lat_mercator_proj(graph -> nodes[i].lat, graph -> min_lat);
        graph -> nodes[i].longitude = ed.lon_mercator_proj(graph -> nodes[i].longitude, graph -> min_long);
    }

    file.close();

    //compute offset arrays 
    outedge_offset_array(graph);
    inedge_offset_array(graph);
    
    //write another file containing the projected coordinates of the graph 
    write_graph(graph, "graph_x_y.dat");

    return;
}


bool compare_outedge(struct edge edge1, struct edge edge2) {
    if(edge1.srcid == edge2.srcid) {
        return edge1.trgtid < edge2.trgtid;
    }
    return edge1.srcid < edge2.srcid;
}

bool compare_inedge(struct edge edge1, struct edge edge2) { //int , check
    if(edge1.trgtid == edge2.trgtid) {
        return edge1.srcid < edge2.srcid;
    }
    return edge1.trgtid < edge2.trgtid;
}

void outedge_offset_array(Graph* graph) {
    vector<struct edge> out_edges = graph -> edges;
    sort(out_edges.begin(), out_edges.end(), compare_outedge);
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
    
    graph -> out_offsets = offset;
    for(int i = 0; i < out_edges.size(); i++) {
        graph -> out_off_edges.push_back(out_edges[i].id);
    }
}

void inedge_offset_array(Graph* graph) {
    vector<struct edge> in_edges = graph -> edges;
    sort(in_edges.begin(), in_edges.end(), compare_inedge);
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

    graph -> in_offsets = offset;
    for(int i = 0; i < in_edges.size(); i++) {
        graph -> in_off_edges.push_back(in_edges[i].id);
    }
}


int get_outdeg(Graph* graph, int node_id) {
    return graph -> out_offsets[node_id + 1] - graph -> out_offsets[node_id];
}

int get_out_edge(Graph* graph, int node_id, int k) {
    int neighbours = get_outdeg(graph, node_id);
    if(k >= neighbours) {
        //invalid
        return -1;
    }

    int index = graph -> out_offsets[node_id] + k;
    return graph -> out_off_edges[index];

}

vector<int> get_incident(Graph* graph, int node_id) {
    vector<int> incidents;

    int n_neighbours = get_outdeg(graph, node_id);
    int index = graph -> out_offsets[node_id];

    for(int i = index; i < (index + n_neighbours); i++) {
        int edge_id = graph -> out_off_edges[i];
        int neighbour_id = graph -> edges[edge_id].trgtid;
        incidents.push_back(neighbour_id);
    }

    return incidents;
}


void DFS_visit(int node_id, vector<bool> &visited, stack<int> &dfs_order, Graph* graph) {
    stack<int> Stack;

    Stack.push(node_id);
    while(!Stack.empty()) {
        node_id = Stack.top();
        Stack.pop();
        if(!visited[node_id]) {
            dfs_order.push(node_id);
            cout << node_id << " ";
            visited[node_id] = true;
        }
        vector<int> incidents = get_incident(graph, node_id);

        for(int i = incidents.size() - 1; i >= 0; i--) {
            if(!visited[incidents[i]]){
                Stack.push(incidents[i]);
            }
        }
    }
}

stack<int> DFS(Graph* graph) {
    vector<bool> visited(graph -> n_nodes, false);
    stack<int> dfs_order;
    
    // for(int i = 0; i < graph -> n_nodes; i++) {
    //     if(!visited[i]) {
    //         DFS_visit(i, visited, dfs_order, graph);
    //     }
    // }
    DFS_visit(0, visited, dfs_order, graph);
    cout << endl;
    return dfs_order;
}



vector<int> trans_get_incident(Graph* graph, int node_id) {
    vector<int> incidents;

    int n_neighbours = graph -> in_offsets[node_id + 1] - graph -> in_offsets[node_id];
    int index = graph -> in_offsets[node_id];

    for(int i = index; i < index + n_neighbours; i++) {
        int edge_id = graph -> in_off_edges[i];
        int neighbour_id = graph -> edges[edge_id].srcid;
        incidents.push_back(neighbour_id);
    }
    return incidents;
}

void trans_DFS_visit(int node_id, vector<bool> &visited, Graph* graph) {
    stack<int> stack;
    stack.push(node_id);

    while(!stack.empty()) {
        node_id = stack.top();
        stack.pop();
        if(!visited[node_id]) {
            cout << node_id << " ";
            visited[node_id] = true;
        }
        vector<int> incidents = trans_get_incident(graph, node_id);
        for(int i = incidents.size() - 1; i >= 0; i--) {
            if(!visited[incidents[i]]){
                stack.push(incidents[i]);
            }
        }

    }
}

void DFS_transpose(Graph* graph, Graph* SCC_graph, stack<int> Stack) {
    vector<bool> visited(graph -> n_nodes, false);
    while(!Stack.empty()) {
        int vrtx = Stack.top();
        Stack.pop();
        if(!visited[vrtx]) {
            trans_DFS_visit(vrtx, visited, graph);
            cout << endl;
        }
    }
}

// void DFS_visit(int node_id, vector<bool> &visited, stack<int> &Stack, Graph* graph) {
//     visited[node_id] = true;

//     //visit all the neighbours of current node
//     vector<int> incidents = get_incident(graph, node_id);
//     for(int i = 0; i < incidents.size(); i++) {
//         if(!visited[incidents[i]]) {
//             DFS_visit(incidents[i], visited, Stack, graph);
//         }
//     }
//     Stack.push(node_id);
// }

// stack<int> DFS(Graph* graph) {
//     //initialize visited and the stack for DFS
//     vector<bool> visited(graph -> n_nodes, false);
//     stack<int> Stack;

//     for(int i = 0; i < graph -> n_nodes; i++) {
//         if(!visited[i]) {
//             DFS_visit(i, visited, Stack, graph);
//         }
//     }

//     return Stack;
// }

// bool comp_scc(Graph g1, Graph g2) {
//     return g1.n_nodes > g2.n_nodes;
// };

// void trans_DFS_visit(int vertex, vector<bool> &visited, Graph* graph, Graph* SCC_graph) {
//     visited[vertex] = true;
//     cout << vertex << " ";

//     // SCC_graph -> n_nodes += 1;
//     // struct node n = {
//     //     .id = vertex,
//     //     .lat = graph -> nodes[vertex].lat,
//     //     .longitude = graph -> nodes[vertex].longitude
//     // };
//     // SCC_graph -> nodes.push_back(n);

//     vector<int> incidents = trans_get_incident(graph, vertex);
//     for(int i = 0; i < incidents.size(); i++) {
//         if(!visited[incidents[i]]) {
//             //this only keeps the edges visited in the DFS
//             // struct edge e = {
//             //     .id = SCC_graph -> n_edges,
//             //     .srcid = incidents[i], 
//             //     .trgtid = vertex
//             // };
//             // SCC_graph -> edges.push_back(e);
//             // SCC_graph -> n_edges += 1;
//             trans_DFS_visit(incidents[i], visited, graph, SCC_graph);
//         }
//     }
// }

// void DFS_transpose(Graph* graph, Graph* SCC_graph, stack<int> Stack) {
//     vector<bool> visited(graph -> n_nodes, false);

//     vector<Graph> scc_graphs;
//     while(!Stack.empty()) {
//         int vrtx = Stack.top();
//         Stack.pop();

//         if(!visited[vrtx]) {
//             trans_DFS_visit(vrtx, visited, graph, SCC_graph);
//             cout << endl;
//             // if(SCC_graph -> n_nodes >= 50) {
//             //     scc_graphs.push_back(*SCC_graph);
//             // }

//             // SCC_graph -> n_nodes = 0;
//             // SCC_graph -> n_edges = 0;
//             // SCC_graph -> edges.clear();
//             // SCC_graph -> nodes.clear();
//         }
//     }
//     // sort(scc_graphs.begin(), scc_graphs.end(), comp_scc);

//     // SCC_graph -> n_edges = scc_graphs[0].n_edges;
//     // SCC_graph -> n_nodes = scc_graphs[0].n_nodes;
//     // SCC_graph -> edges = scc_graphs[0].edges;
//     // SCC_graph -> nodes = scc_graphs[0].nodes;
// }

//check tarjan's algorithm ==> change later?

void scc_graph(Graph* graph, Graph* SCC_graph) {
    //dfs and add nodes to stack 
    stack<int> Stack = DFS(graph);
    
    DFS_transpose(graph, SCC_graph, Stack);

    write_graph(SCC_graph, "SCC_graph.dat");
}

//store all the SCC in separate graphs (graph array) and have the option to choose one of them 

// or create a heap and keep track of the largest number of edges for each subgraph 