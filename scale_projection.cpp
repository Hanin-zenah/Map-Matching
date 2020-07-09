
#include <iostream>
#include <cmath>
#include <math.h>
#include "graph.h"
// #include "Bounding_box.cpp"

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

class euc_distance{
private: double lat_1, lat_2,lon_1, lon_2, x_scale, y_scale, lon_min ;

public:

double degree_to_radian(double a) {return a*M_PI/180.0;};

double trans_range (double x){
    return (1.0 + (x / M_PI))*0.5;
}

double lat_mercator_proj (double lat) {
    double lat_r=degree_to_radian(lat);
    double y=log(tan(0.25*M_PI+0.5*lat_r));
    return y;
}

double euc_dist (double lat1,double lon1, double lat2,double lon2, double x_scale,double y_scale, double lon_min) {
    double x1= degree_to_radian(lon1-lon_min);
    double x2= degree_to_radian(lon2-lon_min);

    double y1= lat_mercator_proj(lat1);
    double y2= lat_mercator_proj(lat2);

    double dist= sqrt(pow((x2-x1)*x_scale,2.0)+pow((y2-y1)*y_scale,2.0));
    return dist;
}
};

int main(){
    double lat_min,lat_max,lon_min,lon_max;
    double g_dist1, g_dist2, g_dist3;

    Graph graph = {
    0, 0, INT_MAX, INT_MIN, INT_MAX, INT_MIN
    };
    read_file("saarland-200601.car.txt", &graph);
    lat_min = graph.min_lat;
    lat_max = graph.max_lat;
    lon_min = graph.min_long;
    lon_max = graph.max_long;

    Bounds bd;
    g_dist1=bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    g_dist2=bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    g_dist3=bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);

    euc_distance ed;
    double e_dist,e_dist1,e_dist2,e_dist3;

    e_dist1=ed.euc_dist(lat_min,lon_min,lat_max,lon_min,lon_min, 1, 1);
    e_dist2=ed.euc_dist(lat_min,lon_max,lat_min,lon_min,lon_min, 1, 1);
    e_dist3=ed.euc_dist(lat_max,lon_max,lat_max,lon_min,lon_min, 1, 1);

    double lat_1, lat_2,lon_1, lon_2, x_scale,y_scale;
    
    cin>>lat_1>>lon_1>>lat_2>>lon_2;

    x_scale=(g_dist2+g_dist3)*0.5/e_dist1;
    y_scale=g_dist1/e_dist1;

    e_dist=ed.euc_dist(lat_1, lon_1, lat_2, lon_2, lon_min,x_scale,y_scale);
    cout<<g_dist1<<endl<<e_dist1<<endl<<e_dist;

    return 0;
}