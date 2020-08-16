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

typedef struct fsnode {
    // unsigned long long int fspair; //are fsnode IDs and edge ID neccesary?
    int vid;
    int tid;
    double dist;
    bool visited;
    //vector<FSedge*> edgelist; // change to pointer fsedge
} FSnode;


typedef struct fsedge {
    int edgeid;
    FSnode* src; //how do I point to a FSnode without using a sort of ID
    FSnode* trg;
    double botlneck_val;
} FSedge;


typedef struct FSPair_key {
    //this pair will store Vid and Tid as a pair to be used as a key for the hashmap 
    int first; //vid
    int second; //tid

    bool operator==(const struct FSPair_key& other) const { 
        return (first == other.first
            && second == other.second);
    }
} FSpair;

struct KeyHash {
    size_t operator()(const FSpair& k) const {
        using std::size_t;
        using std::hash;
        using std::string;

        return ((hash<int>()(k.first)
                ^ (hash<int>()(k.second) << 1)) >> 1);

        //or we can use 
        // 0.5*(n+m)*(n+m+1)+m;
        //or any hash function we come up with later
    }

};

typedef struct fsgraph {
    double eps; //the min traversal distance, initial = distance(v1, t1) // global leashlength value for the freespace graph
    unordered_map<FSpair, FSnode*, KeyHash> pair_dict; 
    vector<FSnode> fsnodes;
    vector<FSedge> fsedges;
} FSgraph;


/* to sort the min priority queue */
struct Comp_eps {
    bool operator()(const FSedge* edge1, const FSedge* edge2) const{
        return edge1->botlneck_val > edge2->botlneck_val;
    }
};

/* Cantor pairing function */
// unsigned long long int pairing(int n, int m);



/* distance of Vi, Tj in a corner */
double nodes_dist(node g_nd, node t_nd);

/* calculate the minimal leash length for discrete frechet */
double min_eps(Graph* graph, Graph* traj, FSgraph* fsgraph);

/* given a FSnode, build the 3 outgoing edges and target nodes using this node */
double build_node(FSgraph* fsgraph, fsnode fsnd, int neighbor_id, int up, int right);

/* given a nodes pair on a FS graph, returns the node pair after the next traversal */
FSpair traversal(FSgraph* fsgraph, FSpair corner, Graph* graph, Graph* traj, priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, stack <FSedge*>& Stack);
#endif