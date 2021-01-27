#ifndef GRAPH_FOR_HIST_H 
#define GRAPH_FOR_HIST_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 
#include "../preprocessing/graph.h"

using namespace std;

class EdgeStat{
/* generates only edges with edge IDs and costs */
    public:
    void graph_edge_cost(Graph* graph, string file_name);
};

#endif
