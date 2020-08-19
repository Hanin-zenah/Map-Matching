#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <iostream> 
#include <fstream> 
#include <cmath>
#include <cstdlib>
#include <vector>
#include "scale_projection.h"

#define TRAJ_VAL_SIZE (4)
#define LON_LAT_COMMA_SHIFT (7)
#define DEF_TRAJ {0, 0, 0};

using namespace std;

typedef struct point {
    double longitude;
    double latitude;
    double timestamp; //??
    //int id;


    // struct point* next;
    // struct point* prev;
} Point;

typedef struct edge {
    Point* src; 
    Point* trg;
    double cost;
} Tedge;

typedef struct trajectory {
    int length; //nPoints
    uint32_t traceId;
    uint32_t subId; 
    vector<Point*> points;
    vector<Tedge*> edges;
    // Point* head;
    // Point* tail;
} Trajectory;

void add_point(Trajectory* traj, double longitude, double latitude, int timestamp);

void read_next_k_bytes(ifstream& file, char* buffer, int k);

void extract_next_trajectory(ifstream& file, int offset, Trajectory* traj, double min_long, double min_lat);

vector<Trajectory> read_trajectories(string file_path, int k, double min_long, double min_lat);

#endif