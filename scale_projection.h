#ifndef SCALE_PROJECTION_H
#define SCALE_PROJECTION_H

#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h"

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

        double lat_mercator_proj (double lat);

        double euc_dist (double lat1, double lon1, double lat2, double lon2, 
                            double x_scale, double y_scale, double lon_min);
};

#endif

