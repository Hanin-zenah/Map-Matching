#ifndef DISC_FRECHET_V2_H
#define DISC_FRECHET_V2_H

#include <iostream>
#include <cmath>
#include <math.h>
#include <utility>
#include <string>
#include <vector> 
#include <algorithm>
#include <queue> 
#include <stack>
#include <unordered_map> 
#include <cstdlib>
#include "graph.h"
#include "trajectory.h"
#include "scale_projection.h"

#define FSGRAPH_INIT {0}

using namespace std;

typedef struct fsnode {
    int vid;
    int tid;
    double dist;
    double sp_dist;
    bool visited;
    bool settled;
    struct fsnode* parent;
    struct fsnode* sp_parent;
    
} FSnode;


typedef struct fsedge {
    FSnode* src; //super edge will have no source 
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

struct KeyPairHash {
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

struct keyNodeHash {
    size_t operator()(const FSnode* n) const {
        using std::size_t;
        using std::hash;
        using std::string;

        return ((hash<int>()(n -> vid)
                ^ (hash<int>()(n -> tid) << 1)) >> 1);
    }
};

typedef struct fsgraph {
    double eps; //the min traversal distance, initial = distance(v1, t1) // global leashlength value for the freespace graph
    unordered_map<FSpair, FSnode*, KeyPairHash> pair_dict; 
    unordered_map<FSnode*, vector<FSedge*>, keyNodeHash> adj_list;
    vector<FSnode*> fsnodes;// can be changed to pointers?? only used them to count the number of nodes/edges in the FSgraph so far
    vector<FSedge*> fsedges;
    vector<FSnode*> source_set;
} FSgraph;


/* to sort the min priority queue */
struct Comp_eps { 
    bool operator()(const FSedge* edge1, const FSedge* edge2) const {
        return edge1->botlneck_val > edge2->botlneck_val;
    }
};


/* distance of Vi, Tj in a corner */
double nodes_dist(node g_nd, Point* t_nd);

/* calculate the minimal leash length for discrete frechet */
FSpair min_eps(Graph* graph, Trajectory* traj, FSgraph* fsgraph, double radius);

/* given a FSnode, build the 3 outgoing edges and target nodes using this node */
double build_node(FSgraph* fsgraph, fsnode fsnd, int neighbor_id, int up, int right);

/* given a nodes pair on a FS graph, returns the node pair after the next traversal */
FSpair traversal(FSgraph* fsgraph, FSpair corner, Graph* graph, Trajectory* traj, 
priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, stack <FSedge*>& Stack, vector<FSedge*> superEdges);

/* always have a back up super edge in the priority queue while traversaling */
void back_up_se(FSgraph* fsgraph, priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, vector<FSedge*>& super_edges);

/* increase the current leash length value when there is no readily traversable edges */
FSnode* increase_eps(priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, FSgraph* fsgraph, vector<FSedge*>& super_edges);

/* travel to the next reachable edge */
FSnode* travel_reachable (FSgraph* fsgraph, stack <FSedge*>& Stack);

double path_cost(FSgraph* fsgraph, Graph* graph, FSpair pair);

/* produce a graph that shows the path */
void print_path(FSgraph* fsgraph, Trajectory* traj, Graph* graph, string file_name, FSpair pair);

void cleanup(FSgraph* fsgraph);

void write_fsgraph(FSgraph* fsgraph, string file_name);

void write_sur_graph(FSgraph* fsgraph, Graph* graph, string file_name);

#endif