#ifndef SCALE_PROJECTION_H
#define SCALE_PROJECTION_H

#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h"
#include "trajectory.h"

using namespace std;

class Bounds {
    private:
        double lat_min, lat_max,lon_min, lon_max;
        double earth_r;
    public:
        Bounds() {
            earth_r = 6378000;
        }

        double degree_to_radian(double a); 

        double geodesic_dist(double lat1,double lon1, double lat2,double lon2);
}; 

class Euc_distance {
    private: 
        double lat_1, lat_2, lon_1, lon_2, x_scale, y_scale, lon_min;

    public:
        double degree_to_radian(double a);

        double trans_range (double x);

        double lon_mercator_proj(double lon, double lon_min);

        double lat_mercator_proj (double lat, double lat_min);

        double lon_mercator_proj_scale(double lon, double lon_min, double y_scale);

        double lat_mercator_proj_scale(double lat, double lat_min, double x_scale);

        double euc_dist(double y1, double x1, double y2, double x2, double x_scale, double y_scale);

        void calc_edge_cost(Graph* graph, double x_scale, double y_scale);

        // void calc_traj_edge_cost(Trajectory* traj, double x_scale, double y_scale);

};

#endif