#ifndef GRAPH_C_H 
#define GRAPH_C_H

#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include <string.h> 
#include <stdbool.h> 
#include <limits.h>


#define PI (3.14159265358979323846)
#define MAX_BUFF (100)
#define IGNORE_LINES (5)

struct node {
    uint64_t osmid;
    double longitude;
    double lat;
};

struct edge {
    struct node src;
    struct node target;
    double cost;
};

typedef struct graph {
    int n_edges;
    int n_nodes;

    struct node* nodes;
    struct edge* edges;

    double min_lat;
    double max_lat;

    double min_long;
    double max_long;
} Graph;


void check_boundaries(double latitude, double longitude, Graph* g);
Graph* read_file(const char* file_name);


#endif