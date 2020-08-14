#ifndef DISC_FRECHET_V2_H
#define DISC_FRECHET_V2_H

#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h" 
#include "trajectory.h"
#include <utility>
#include <string>
#include <vector> 
#include <algorithm>
#include <queue> 
#include <stack>
#include <unordered_map> 

#define FSGRAPH_INIT {0}

using namespace std;

typedef struct fsnode{
    unsigned long long int fspair; // are fsnode IDs and edge ID neccesary?
    int vid;
    int tid;
    double dist;
    bool visited;
    //vector<FSedge*> edgelist; // change to pointer fsedge
} FSnode;


typedef struct fsedge{
    int edgeid;
    FSnode* src; //how do I point to a FSnode without using a sort of ID
    FSnode* trg;
    double botlneck_val;
} FSedge;


typedef struct static_fsgraph {
    double eps; //the min traversal distance, initial = distance(v1, t1) // global leashlength value for the freespace graph
    unordered_map<unsigned long long int, FSnode*> pair_dict; 
    vector<struct fsnode> fsnodes;
    vector<struct fsedge> fsedges;
} FSgraph;

struct Comp_eps {
    bool operator()(const FSedge* edge1, const FSedge* edge2) const{
        return edge1->botlneck_val > edge2->botlneck_val;
    }
};
/* to sort the min priority queue */
// bool compare_eps(FSedge* edge1, FSedge* edge2);

/* Cantor pairing function */
unsigned long long int pairing(int n, int m);

/* distance of Vi, Tj in a corner */
double nodes_dist(node g_nd, node t_nd);

/* calculate the minimal leash length for discrete frechet */
double min_eps(Graph* graph, Graph* traj, FSgraph* fsgraph);

/* given a FSnode, build the 3 outgoing edges and target nodes using this node */
double build_node(FSgraph* fsgraph, fsnode fsnd, int neighbor_id, int up, int right);

/* given a nodes pair on a FS graph, returns the node pair after the next traversal */
unsigned long long int traversal(FSgraph* fsgraph, unsigned long long int corner, Graph* graph, Graph* traj, priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, stack <FSedge*>& Stack);
#endif