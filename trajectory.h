#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <iostream> 
#include <fstream> 
#include <cmath>
#include <cstdlib>
#include <vector>

#define TRAJ_VAL_SIZE (4)
#define LON_LAT_COMMA_SHIFT (7)
#define DEF_TRAJ {NULL, NULL, 0, 0, 0};

using namespace std;

typedef struct point {
    double longitude;
    double latitude;
    double timestamp; //??
    struct point* next;
    struct point* prev;
} Point;

typedef struct trajectory {
    Point* head;
    Point* tail;
    int length; //nPoints
    uint32_t traceId;
    uint32_t subId;
} Trajectory;

void add_point(Trajectory* traj, double longitude, double latitude, int timestamp);

void read_next_k_bytes(ifstream& file, char* buffer, int k);

void extract_next_trajectory(ifstream& file, int offset, Trajectory* traj);

vector<Trajectory> read_trajectories(string file_path, int k);





#endif