#ifndef STARTING_NODE_LOOK_UP_H
#define STARTING_NODE_LOOK_UP_H

#include <iostream> 
#include <string>
#include <vector> 
#include "graph.h" 
#include "disc_frechet_v2.h"


using namespace std;

class LookUp {
    private:
        struct node traj_nd;

        double lat_min, lat_max, lon_min, lon_max;
        double g_dist1, g_dist2, g_dist3;
        double e_dist1, e_dist2;

        //int grid_h, grid_w;

        double box_height, box_width;

        double lon_min_to_x, lon_max_to_x, lat_min_to_y, lat_max_to_y; 
        double x_scale, y_scale;

    public:
    //  LookUp(Graph* graph) {  //what's the "*"?
    //  lat_min = graph.min_lat;
    //  lat_max = graph.max_lat;
    //  lon_min = graph.min_long;
    //  lon_max = graph.max_long;
    //  Bounds bd;
    //  g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    //  g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    //  g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
    //  box_height = g_dist1;
    //  box_width = (g_dist2 + g_dist3)/2;
    //  Euc_distance ed;
    //  lon_min_to_x = ed.lon_mercator_proj(lon_min, lon_min);
    //  lon_max_to_x = ed.lon_mercator_proj(lon_max, lon_min);
    //  lat_min_to_y = ed.lat_mercator_proj(lat_min, lat_min);
    //  lat_max_to_y = ed.lat_mercator_proj(lat_max, lat_min);
    //  e_dist1 = ed.euc_dist(lat_min_to_y,lon_min_to_x,lat_max_to_y,lon_min_to_x, 1, 1);
    //  e_dist2 = ed.euc_dist(lat_min_to_y,lon_max_to_x,lat_min_to_y,lon_min_to_x,1 ,1);
    //  x_scale = box_width / e_dist2;
    //  y_scale = box_height / e_dist1;
    // }

       
       

        
        

        // /* assign the grid cell a node belongs to, grid_h and grid_w are user defined */
        // vector<int> assign_grid(node nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);
// 
        // /* find all the nodes in the same grid */
        // vector<int> nodes_in_same_grid(Graph* graph, node traj_nd, int grid_h, int grid_w);
// 
        // /* assign the grid for all the nodes in the graph */
        // void graph_grid(Graph* graph, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);
};

 /* comparison function to sort the superedges */
 bool compare_dist(FSedge* sp1, FSedge* sp2);

/* return the distance between 2 nodes are within the required radius */
 double dist_from_T0(node traj_nd, node g_nd);

/* list out the node IDs of the nodes that are within the requied distance */
vector<FSedge*> SearchNodes(Graph* graph, struct node traj_nd, double radius);

// typedef struct super_edge_eps {
    // this pair will store Vid and Tid as a pair to be used as a key for the hashmap 
    // int node_id; //vid
    // double dist; //tid
// 
    // bool operator==(const struct SE_eps& other) const { 
        // return (first == other.node_id
            // && second == other.dist);
    // }
// } SE_eps;
// 
// struct KeyHash {
    // size_t operator()(const SE_eps& se) const {
        // using std::size_t;
        // using std::hash;
        // using std::string;
// 
        // return ((hash<int>()(se.node_id)
                // ^ (hash<double>()(se.dist) << 1)) >> 1);
// 
    // }
// };






#endif