
#include <iostream>
#include <cmath>
#include <math.h>

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
        //haversine formula
        double dist=earth_r*acos(cos(lat_r_1)*cos(lat_r_2)*cos(lon_r_1-lon_r_2)+sin(lat_r_1)*sin(lat_r_2));
        return dist;
    }

};

class euc_distance{
private: double lat_1, lat_2,lon_1, lon_2, x_scale, y_scale ;

public:
euc_distance() {
    x_scale=1;
    y_scale=1;
}

double degree_to_radian(double a) {return a*M_PI/180.0;};

double trans_range (double x){
    return (1.0 + (x / M_PI))*0.5;
}

double lon_mercator_proj (double lon) {
    double lon_r=degree_to_radian(lon);
    double y=log(tan(0.25*M_PI+0.5*lon_r));
    return y;
}

double euc_dist (double lat1,double lon1, double lat2,double lon2) {
    double x1= trans_range(degree_to_radian(lat1))*x_scale;
    double x2= trans_range(degree_to_radian(lat2))*x_scale;

    double y1= trans_range(lon_mercator_proj(lon1))*y_scale;
    double y2= trans_range(lon_mercator_proj(lon2))*y_scale;

    double dist= sqrt(pow((x2-x1),2.0)+pow((y2-y1),2.0));
    return dist;
}
};

int main(){
    double lat_min,lat_max,lon_min,lon_max;
    double g_dist1, g_dist2, g_dist3, e_dist1, e_dist2, e_dist3;
    cin>>lat_min>>lat_max>>lon_min>>lon_max;

    Bounds bd;
    euc_distance ed;

    g_dist1=bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    g_dist2=bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    g_dist3=bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
    e_dist1=ed.euc_dist(lat_min,lon_min,lat_max,lon_min,g_dist2,g_dist1);
    e_dist2=ed.euc_dist(lat_min,lon_max,lat_min,lon_min,g_dist2,g_dist1);
    e_dist3=ed.euc_dist(lat_max,lon_max,lat_max,lon_min,g_dist2,g_dist1);
    cout<<g_dist1<<endl<< g_dist2<<endl<<g_dist3<<endl<<e_dist1<<endl<< e_dist2<<endl<<e_dist3;

    return 0;
}