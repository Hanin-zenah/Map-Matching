#ifndef GRAPH_GRID_STARTING_NODE_GRID_H
#define GRAPH_GRID_STARTING_NODE_GRID_H

#include <iostream> 
#include <string>
#include <vector> 
#include <cstdlib>
#include <cmath>
#include "graph.h" 
#include "disc_frechet_v2.h"
#include "scale_projection.h"
#include "graph_grid.h"


using namespace std;


/* comparison function to compare free space nodes' distance to T0 */
bool compare_node_dist(FSnode* nd1, FSnode* nd2);

/* return the distance between 2 nodes are within the required radius */
double dist_from_T0(Point* traj_nd, node g_nd);

bool available_nodes(Grid* grid, int col, int row, int range);

vector<int> add_range_to_Q(Grid* grid, int col, int row, int range)

/* list out the node IDs of the nodes that are within the specified distance using grid look up */
// vector<FSedge*> GridSearch(Graph* graph, Grid* grid, Point* traj_nd);

vector<FSnode*> GridSearch(Graph* graph, Grid* grid, Point* traj_nd);

int next_closest_node(vector<int> PQ);

vector<int> nodes_in_range(double radius);

vector<int> pos_list(Grid* grid);

vector<FSedge*> ExtendGrid(Graph* graph, Grid* grid, Point* traj_nd);

#endif


// class LookUp {
//     private:
        // Point* traj_nd;
// 
        // double lat_min, lat_max, lon_min, lon_max;
        // double g_dist1, g_dist2, g_dist3;
        // double e_dist1, e_dist2;
// 
        // int grid_h, grid_w;
// 
        // double box_height, box_width;
// 
        // double lon_min_to_x, lon_max_to_x, lat_min_to_y, lat_max_to_y; 
        // double x_scale, y_scale;
// 
//     public:
// 
        // /* assign the grid cell a node belongs to, grid_h and grid_w are user defined */
        // vector<int> assign_grid(node nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);
// 
        // /* find all the nodes in the same grid */
        // vector<int> nodes_in_same_grid(Graph* graph, node traj_nd, int grid_h, int grid_w);
// 
        // /* assign the grid for all the nodes in the graph */
        // void graph_grid(Graph* graph, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);
// };
