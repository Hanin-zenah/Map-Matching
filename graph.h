#ifndef GRAPH_C_H 
#define GRAPH_C_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 
#include <stack>
#include <algorithm>

// #include "scale_projection.h"

using namespace std;

#define MAX_BUFF (100)
#define IGNORE_LINES (5)
#define GRAPH_INIT {0, 0, INT_MAX, INT_MIN, INT_MAX, INT_MIN}
#define DEF_NODE {0, 0, 0, 0}


struct node {
    uint64_t osmid;
    double longitude;
    double lat;
    int id;
};

struct edge {
    int srcid;
    int trgtid;
    int id;
    double cost;
};

typedef struct graph {
    int n_edges;
    int n_nodes;

    double min_lat;
    double max_lat;

    double min_long;
    double max_long;

    vector<struct node> nodes;
    vector<struct edge> edges;

    vector<int> out_offsets;
    vector<int> out_off_edges;

    vector<int> in_offsets;
    vector<int> in_off_edges;

} Graph;

/* writes the longitude, latitude of the end points of every edge of a given graph to a given file */
void write_graph(Graph* graph, string file_name);

/* checks and updates the graph's bounding box corners accordingly */
void check_boundaries(double latitude, double longitude, Graph* g);

/* reads a file that contains the graph information and returns a graph with all the necessary attributes set */
void read_file(string file_name, Graph* graph);

/* function used for sorting the edges of the graph in order of their source id */
bool compare_outedge(struct edge edge1, struct edge edge2);

/* function used for sorting the edges of the graph in order of their target id */
bool compare_inedge(struct edge edge1, struct edge edge2);

/* generates the out degree offset array and stores it in graph */
void outedge_offset_array(Graph* graph);

/* generates the in degree offset array and stores it in graph */
void inedge_offset_array(Graph* graph);

vector<int> get_incident(Graph* graph, int node_id);

void DFS_visit(int node_id, vector<bool> &visited, stack<int> &Stack, Graph* graph);

stack<int> DFS(Graph* graph);

/* get the incident nodes for the given node_id in the transpose graph */
vector<int> trans_get_incident(Graph* graph, int node_id);

// void trans_DFS_visit(int vertex, vector<bool> &visited, Graph* graph, Graph* scc_graph);
void trans_DFS_visit(int node_id, vector<bool> &visited, Graph* graph);


// void DFS_transpose(Graph* graph, Graph* scc_graph, stack<int> Stack); 
void DFS_transpose(Graph* graph, Graph* SCC_graph, stack<int> Stack);


/* extracts the strongly connected components of the graph */
void scc_graph(Graph* graph, Graph* scc_graph);

#endif