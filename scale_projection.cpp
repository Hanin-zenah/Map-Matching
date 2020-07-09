
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

class euc_distance{
private: double lat_1, lat_2,lon_1, lon_2, x_scale, y_scale, lon_min , lat_min;

public:

double degree_to_radian(double a) {return a*M_PI/180.0;};

double lat_mercator_proj (double lat) {
    double lat_r=degree_to_radian(lat);
    double y=log(tan(0.25*M_PI+0.5*lat_r));
    return y;
}

double euc_dist (double lat1,double lon1, double lat2,double lon2, double x_scale,double y_scale, double lon_min, double lat_min) {
    double x1= degree_to_radian(lon1-lon_min);
    double x2= degree_to_radian(lon2-lon_min);

    double y1= lat_mercator_proj(lat1 - lat_min);
    double y2= lat_mercator_proj(lat2 - lat_min);

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

    Bounds bd; // pulling the data from txt file and generates the bounding pts geodesic distances, for the 3 bounding egdes
    g_dist1=bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    g_dist2=bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    g_dist3=bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);

    euc_distance ed;
    double e_dist,e_dist1,e_dist2,e_dist3;

    e_dist1=ed.euc_dist(lat_min,lon_min,lat_max,lon_min, 1, 1,lon_min,lat_min); //the original Euclidean distance of the 2 pts after the projection, without scaling
    e_dist2=ed.euc_dist(lat_min,lon_max,lat_min,lon_min, 1, 1,lon_min,lat_min);
    e_dist3=ed.euc_dist(lat_max,lon_max,lat_max,lon_min, 1, 1,lon_min,lat_min);

    double lat_1, lat_2,lon_1, lon_2, x_scale,y_scale;
    
    cin>>lat_1>>lon_1>>lat_2>>lon_2; 

    x_scale=(g_dist2+g_dist3)*0.5/e_dist2; //calculating the scale
    y_scale=g_dist1/e_dist1;

    e_dist=ed.euc_dist(lat_1, lon_1, lat_2, lon_2, x_scale,y_scale,lon_min,lat_min);
    //cout<<g_dist2<<endl<<e_dist2<<endl<<e_dist;

    return e_dist;
}