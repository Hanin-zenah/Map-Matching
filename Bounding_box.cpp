
#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h"

using namespace std;

class Bounds{
private:
    double lat_min, lat_max,lon_min, lon_max;
    double earth_r;
public:
    Bounds() {
        earth_r=6378000;
    }

    double degree_to_radian(double a) {return a*M_PI/180.0;};

    double geodesic_dist(double lat1,double lon1, double lat2,double lon2){
        double lat_r_1= degree_to_radian(lat1);
        double lon_r_1= degree_to_radian(lon1);
        double lat_r_2= degree_to_radian(lat2);
        double lon_r_2= degree_to_radian(lon2);
        //haversine formula:
        double dist=earth_r*acos(cos(lat_r_1)*cos(lat_r_2)*cos(lon_r_1-lon_r_2)+sin(lat_r_1)*sin(lat_r_2));
        return dist;
    }

}; 

// int main(){
    // double lat_min,lat_max,lon_min,lon_max;
    // double g_dist1, g_dist2, g_dist3;
    //cin>>lat_min>>lat_max>>lon_min>>lon_max;
// 
    // Graph graph = {
    //  0, 0, INT_MAX, INT_MIN, INT_MAX, INT_MIN
    // };
    // read_file("saarland-200601.car.txt", &graph);
    // lat_min = graph.min_lat;
    // lat_max = graph.max_lat;
    // lon_min = graph.min_long;
    // lon_max = graph.max_long;
// 
    // Bounds bd;
// 
    // g_dist1=bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    // g_dist2=bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    // g_dist3=bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
    // cout<<g_dist1<<endl<< g_dist2<<endl<<g_dist3;
// 
    // return 0;
// }