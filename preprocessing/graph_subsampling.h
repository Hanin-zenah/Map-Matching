#ifndef GRAPH_SUBSAMPLING_H 
#define GRAPH_SUBSAMPLING_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 
#include "graph.h"

using namespace std;

/* function used to find all bidirectional paths*/
int bi_dir(Graph* graph, int edgeID);

/* function used to split edges*/
void split_bi_dir_edge(Graph* graph, int edgeID1, int edgeID2);

/* process subsampling */
void subsampling(Graph* graph, double threshold); 


#endif