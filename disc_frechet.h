#ifndef DISC_FRECHET_H
#define DISC_FRECHET_H

#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h" 


#include <string>
#include <vector> 

#include <algorithm>

using namespace std;

typedef struct fsnode{
    int vid;
    int tid;
    bool reachable;
    vector<int> edgelist;
} FSnode;


typedef struct fsedge{
    FSnode src;
    FSnode trg;
    double btlneck_val;
} FSedge;


typedef struct static_fsgraph{
    double eps; //the min traversal distance, initial = distance(v1, t1)
    int n_V, n_T;
    // n_FSnodes = (n+1)*(m+1);
    // n_FSedges = 3*n*m + m , if m > n, or else 3*n*m + n
    vector<struct FSnode> FSnodes;
    vector<struct FSedge> FSedges;

} FSgraph;

/* calculate the minimal leash length for discrete frechet */
double min_eps(Graph* graph, Graph* traj);

#endif