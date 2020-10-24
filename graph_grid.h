#ifndef GRAPH_GRID_H 
#define GRAPH_GRID_H

#include <iostream> 
#include <string>
#include <vector> 
#include <cstdlib>
#include <cmath>
#include "graph.h" 
#include "scale_projection.h"

using namespace std;

#define GRID_INIT {0, 0} // ask Haneen on initialization, like how many components need to initialize and the order?


typedef struct cell {

    int nodes_count;

} Cell;

// typedef struct cell_node {
    // double row;
    // double col;
    // int index;
    // int pos;
// } Cell_node;

typedef struct grid {
    int num_rows;
    int num_columns;
    int size;

    double dist_to_peak;

    int curr_range;

    // vector<Cell> cells;

    vector<int> cell_offset;
    vector<int> cell_nodes_list;
    // vector<Cell_node*> nodes_ids;

} Grid;

// make it into own class, have constructors

void initialize_cells(Graph* graph, Grid* grid, double size);

void make_grids(Graph* graph, Grid* grid, double size);

void output_grid(Grid* grid, string file_name);


#endif