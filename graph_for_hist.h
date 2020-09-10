#ifndef GRAPH_FOR_HIST_H 
#define GRAPH_FOR_HIST_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 
#include "graph.h"

using namespace std;

/* generates only edges with edge IDs and costs */

void graph_edge_cost(Graph* graph, string file_name);

#endif