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
    int cell_id;
    int nodes_count;
    bool included;
} Cell;

typedef struct cell_node {
    double row;
    double col;
    int index;
    int pos;
} Cell_node;

typedef struct grid {
    int num_rows;
    int num_columns;
    int size;

    int target_cell_idx;
    int upper_left_index;
    int lower_right_index;
    int curr_num_cells;

    vector<Cell*> cells;

    vector<int> cell_offsets;
    vector<int> nodes_ids;
    // vector<Cell_node*> nodes_ids;

} Grid;

// make it into own class, have constructors

void initialize_cells(Graph* graph, Grid* grid, double size);

void make_grids(Graph* graph, Grid* grid, double size);

void output_grid(Grid* grid, string file_name);


#endif