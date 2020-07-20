#ifndef GRAPH_C_H 
#define GRAPH_C_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 

using namespace std;

#define MAX_BUFF (100)
#define IGNORE_LINES (5)
#define GRAPH_INIT {0, 0, INT_MAX, INT_MIN, INT_MAX, INT_MIN};


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

// struct offset_array {
//     vector<int> offsets;
//     vector<int> edges;
// };

typedef struct graph {
    int n_edges;
    int n_nodes;

    double min_lat;
    double max_lat;

    double min_long;
    double max_long;

    vector<struct node> nodes;
    vector<struct edge> edges;

    //vector<int> offsets;
    //vector<int> off_edges;

} Graph;



/* checks and updates the graph's bounding box corners accordingly */
void check_boundaries(double latitude, double longitude, Graph* g);

/* reads a file that contains the graph information and returns a graph with all the necessary attributes set */
void read_file(string file_name, Graph* graph);

/* function used for sorting the edges of the graph in order of their source id */
bool compare_outdegree(struct edge edge1, struct edge edge2);

/* function used for sorting the edges of the graph in order of their target id */
bool compare_indegree(struct edge edge1, struct edge edge2);

/* generates the out degree offset array and stores it in given file (file_name) */
void outdeg_offset_array(Graph* graph, string file_name);

/* generates the in degree offset array and stores it in given file (file_name) */
void indeg_offset_array(Graph* graph, string file_name);

/* extracts the strongly connected components of the graph and ... */
void str_cnctd_cmpnt(Graph* graph);

/* function used for testing if a path is bidirectional */
bool bi_dir(struct edge edge1, struct edge edge2);

/* generates soreted graph with subsampling*/
void sort_for_subsamp(Graph* graph);

/* generates graph with subsampling*/
void subsampled_array(Graph* graph, string file_name);

#endif