#ifndef GRAPH_H 
#define GRAPH_H

#include <iostream> 
#include <fstream> 

#define PI (3.14159)


struct node {
    uint64_t osmid;
    double longitude;
    double latitude;
};

struct edge {
    struct node src;
    struct node target;
    double cost;
};

#endif