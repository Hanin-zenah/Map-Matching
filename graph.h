#ifndef GRAPH_C_H 
#define GRAPH_C_H

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <vector> 

using namespace std;



#define PI (3.14159265358979323846)
#define MAX_BUFF (100)
#define IGNORE_LINES (5)

struct node {
    uint64_t osmid;
    double longitude;
    double lat;
    int id;
};

struct edge {
    // struct node* src;
    // struct node* target;

    int srcid;
    int trgtid;

    int id;

    double cost;
};

typedef struct graph {
    int n_edges;
    int n_nodes;

    double min_lat;
    double max_lat;

    double min_long;
    double max_long;

    vector<struct node> nodes;
    vector<struct edge> edges;

} Graph;

struct offset_array {
    vector<int> offsets;
    vector<int> edges;
};


void check_boundaries(double latitude, double longitude, Graph* g);
Graph* read_file(string file_name);


#endif