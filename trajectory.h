#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 
#include "graph.h" 

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
        LookUp(Graph* graph) {  //what's the "*"?
            lat_min = graph.min_lat;
            lat_max = graph.max_lat;
            lon_min = graph.min_long;
            lon_max = graph.max_long;

            Bounds bd;
            g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
            g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
            g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);

            box_height = g_dist1;
            box_width = (g_dist2 + g_dist3)/2;

            Euc_distance ed;
            lon_min_to_x = ed.lon_mercator_proj(lon_min, lon_min);
            lon_max_to_x = ed.lon_mercator_proj(lon_max, lon_min);
            lat_min_to_y = ed.lat_mercator_proj(lat_min, lat_min);
            lat_max_to_y = ed.lat_mercator_proj(lat_max, lat_min);

            e_dist1 = ed.euc_dist(lat_min_to_y,lon_min_to_x,lat_max_to_y,lon_min_to_x, 1, 1);
            e_dist2 = ed.euc_dist(lat_min_to_y,lon_max_to_x,lat_min_to_y,lon_min_to_x,1 ,1);

            x_scale = box_width / e_dist2;
            y_scale = box_height / e_dist1;
        }

        
        

        bool withinDist(node traj_nd, node g_nd, int radius){
            int dist; 
            //calculate the Euclidean distance of node and the starting node of the trajectory;
            dist = sqrt(pow((traj_nd.lat - g_nd.lat), 2) + pow((traj_nd.longitude - g_nd.longitude), 2));
            //return true if the distance is within the required radius/distance.
            return (radius >= dist); 
        };

        vector<struct node> SearchNodes(Graph* graph, node traj_nd, int radius){
            vector<struct node> nodeslist;
            for (int i; i < graph -> nodes.size(); i++){
                if withinDist(traj_nd, graph -> nodes[i] , r){
                    nodeslist.push_back(graph -> nodes[i].id);
                }
            }
            return nodeslist;
        };

        vector<struct node> SearchGrid (Graph* graph, node traj_nd, int radius);

        vector<int> assign_grid(node nd, int grid_h, int grid_w){
            vector<int> grid;
            int num_grid_h, num_grid_w;
            num_grid_h = box_height / grid_h;
            num_grid_w = box_width / grid_w;
            grid.push_back(floor((nd.lat- lat_min_to_y) * x_scale/ num_grid_w));
            grid.push_back(floor((nd.longitude - lon_min_to_x) * y_scale/ num_grid_h)); //the height
   
        return grid;
        } 

        vector<int> nodes_in_same_grid(Graph* graph, node traj_nd, int grid_h, int grid_w){
            vector<int> nodes_same_grid;
            vector<int> traj_nd_grid = assign_grid(traj_nd, grid_h, grid_w);
            for (int i; i < graph -> nodes.size(); i++){
                vector<int> nd_grid = assign_grid(graph -> nodes[i], grid_h, grid_w);
                if (nd_grid[0] == traj_nd_grid[0] && nd_grid[1]  == traj_nd_grid[1]){
                    nodes_same_grid.push_back(graph -> nodes[i]);
                }
            }
        return nodes_same_grid;
        }

/* return if the distance between 2 nodes are within the required radius */
// bool withinDist(node traj_nd, node g_nd, int r);

/* list out the node IDs of the nodes that are within the requied distance */
// vector<int> ClosestNodes(Graph* graph, node traj_nd, int radius);

/* assign the grid cell a node belongs to, grid_h and grid_w are user defined */
void assign_grid(node nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);

/* assign the grid for all the nodes in the graph */
void graph_grid(Graph* graph, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);

/* find all the nodes in the same grid */
vector<int> nodes_in_same_grid(node traj_nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale);

#endif