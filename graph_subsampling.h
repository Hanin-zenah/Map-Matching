#ifndef GRAPH_SUBSAMPLING_H 
#define GRAPH_SUBSAMPLING_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 
#include "graph.h"

using namespace std;

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
void output_graph(Graph* graph, string file_name);

#endif