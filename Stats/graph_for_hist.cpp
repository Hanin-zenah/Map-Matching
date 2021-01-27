#include "graph_for_hist.h"


void EdgeStat::graph_edge_cost(Graph* graph, string file_name) { 
    ofstream file(file_name);
    for(int i = 0; i < graph -> n_edges; i++) {

        file << graph -> edges[i].cost << endl;
    }
    file.close();
}

