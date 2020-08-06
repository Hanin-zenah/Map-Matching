#ifndef DISC_FRECHET_H
#define DISC_FRECHET_H

#include <iostream> 

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
    doublebool btlneck_val;
} FSedge;

#endif