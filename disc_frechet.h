#ifndef DISC_FRECHET_H
#define DISC_FRECHET_H

#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h" 

#include <string>
#include <vector> 

#include <algorithm>
#include <queue> 
#include <stack>
#include <unordered_map> 


#define FSGRAPH_INIT {0}

using namespace std;


typedef struct fsnode{
    pair<int,int> fsID; // are fsnode IDs and edge ID neccesary?
    int vid;
    int tid;
    double dist;
    bool reachable;
    bool visited;
    vector<int> edgelist;
} FSnode;


typedef struct fsedge{
    int edgeid;
    fsnode src; //how do I point to a FSnode without using a sort of ID
    fsnode trg;
    double botlneck_val;
} FSedge;


typedef struct static_fsgraph{
    double eps; //the min traversal distance, initial = distance(v1, t1)
    //vector<struct fsnode> fsnodes;
    //unordered_map<fsnode.fsID, fsnode> fsnodes;
    vector<struct fsnode> fsnodes;
    vector<struct fsedge> fsedges;
} FSgraph;

/* calculate the minimal leash length for discrete frechet */
double min_eps(Graph* graph, Graph* traj, FSgraph* fsgraph);

#endif