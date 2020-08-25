#ifndef STARTING_NODE_LOOK_UP_H
#define STARTING_NODE_LOOK_UP_H

#include <iostream> 
#include <string>
#include <vector> 
#include <cstdlib>
#include <cmath>
#include "graph.h" 
#include "disc_frechet_v2.h"
#include "scale_projection.h"

using namespace std;


/* comparison function to sort the superedges */
bool compare_dist(FSedge* sp1, FSedge* sp2);

/* return the distance between 2 nodes are within the required radius */
 double dist_from_T0(Point* traj_nd, node g_nd, double x_scale, double y_scale);

/* list out the node IDs of the nodes that are within the specified distance */
vector<FSedge*> SearchNodes(Graph* graph, Point* traj_nd, double radius, double x_scale, double y_scale);

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
