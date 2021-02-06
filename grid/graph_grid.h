#ifndef GRAPH_GRID_H 
#define GRAPH_GRID_H

#include <iostream> 
#include <string>
#include <vector> 
#include <cstdlib>
#include <cmath>
#include "../preprocessing/graph.h" 
#include "../trajectories/trajectory.h"
#include "../preprocessing/scale_projection.h"

using namespace std;

#define GRID_INIT {0, 0, 0, 0} 


typedef struct cell {
    int nodes_count;

} Cell;


typedef struct grid {
    int num_rows;
    int num_columns;
    int size;
    int curr_range;

    double dist_to_peak;

    vector<int> cell_offset;
    vector<int> cell_nodes_list;

} Grid;


class Build_grid{
    public:
        /* build grid cells for a graph */
        void make_grids(Graph* graph, Grid* grid, double size);

        /* store the grid, mainly for checking purposes */
        void output_grid(Grid* grid, string file_name);
};

#endif
