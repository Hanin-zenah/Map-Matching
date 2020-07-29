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
    //uint64_t osmid;
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

    vector<int> out_offset;
    vector<int> out_edge;

    vector<int> in_offset;
    vector<int> in_edge;

} Graph;


template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element)
{
    std::pair<bool, int > result;
    // Find given element in vector
    auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);
    if (it != vecOfElements.end())
    {   result.second = distance(vecOfElements.begin(), it);
        result.first = true;
    }
    else
    {
        result.first = false;
        result.second = -1;
    }
    return result;
}


/* reads a file that contains the graph information and returns a graph with all the necessary attributes set */
//void read_file(string file_name, Graph* graph);

/* function used to find all bidirectional paths*/
int bi_dir(Graph* graph, int edgeID);
//vector<int> bi_dir(Graph* graph, vector<int> edgeID, vector<int> offset);

/* function used to split edges*/
void split_bi_dir_edge(Graph* graph, int edgeID1, int edgeID2);

/* process subsampling*/
void subsampling(Graph* graph, double threshold); //, vector<int>& in_edge, vector<int>& in_offset, vector<int>& out_edge, vector<int>& out_offset);

/* generates graph with subsampling*/
void subsampled_graph(Graph* graph, string file_name);

#endif