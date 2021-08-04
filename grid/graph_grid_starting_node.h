#ifndef GRAPH_GRID_STARTING_NODE_H
#define GRAPH_GRID_STARTING_NODE_H

#include <iostream> 
#include <algorithm>
#include <string>
#include <vector> 
#include <cstdlib>
#include <cmath>
#include <queue> 
#include "../preprocessing/graph.h" 
#include "../preprocessing/scale_projection.h"
#include "graph_grid.h"


typedef struct GridPair_key {
    //this pair will store node id and distance to T0 as a pair to be used as a key for the hashmap 
    int first; //node id 
    double second; //distance to T0

    bool operator==(const struct GridPair_key other) const { 
        return (first == other.first
            && second == other.second);
    }
} Gpair;

/* to sort the min priority queue for cloest nodes to T0 */
struct Comp_dist_to_t { 
    bool operator()(const Gpair node1, const Gpair node2) const {
        return node1.second > node2.second ;
    }
};


class Grid_search{
    public:
        /* return the distance between 2 nodes are within the required radius */
        double dist_from_T0(Point* traj_nd, node g_nd);
        
        /* check whether there are any nodes in the cells included in the given searching range */
        bool available_nodes(Grid* grid, int col, int row, int range);
        
        /* expand the included grid cells from n*n to (n+1)*(n+1) */
        void add_range_to_Q(Grid* grid, Graph* graph, int col, int row,
            int range, Point* traj_nd, std::priority_queue<Gpair, std::vector<Gpair>, Comp_dist_to_t>& PQ);
        
        /* check if the outer layer of cells is touched by the radius of distance to peak */
        bool range_check(Grid* grid, Point* traj_nd, Graph* graph, std::priority_queue<Gpair, std::vector<Gpair>, Comp_dist_to_t> PQ);
        
        /* all the node IDs in touches cells which are found using grid look up */
        std::priority_queue<Gpair, std::vector<Gpair>, Comp_dist_to_t> GridSearch(Graph* graph, Grid* grid, Point* traj_nd);
        
        /* compute the next available closest node from the query point */
        Gpair next_closest_node(Graph* graph, Grid* grid, Point* traj_nd, std::priority_queue<Gpair, std::vector<Gpair>, Comp_dist_to_t>& PQ);
        
        /* compute a list of next closest nodes sorted ascendingly by their distance to query point */
        std::vector<Gpair> next_n_nodes(Graph* graph, Grid* grid, Point* traj_nd, int n_cans, double radius);
        // std::vector<Gpair> next_n_nodes(Graph* graph, Grid* grid, Point* traj_nd, 
        // std::priority_queue<Gpair, std::vector<Gpair>, Comp_dist_to_t>& PQ, int n, double radius);

        /* compute a PQ of next k closest nodes sorted ascendingly by their distance to query point */
        std::priority_queue<Gpair, std::vector<Gpair>, Comp_dist_to_t> k_nodes(Graph* graph, Grid* grid, Point* traj_nd, int k);

};

#endif