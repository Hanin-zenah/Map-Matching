
#include <iostream>
#include <cmath>
#include <math.h>

using namespace std;

class euc_distance{
private: double lat_1, lat_2,lon_1, lon_2, x_scale, y_scale ;

public:

double degree_to_radian(double a) {return a*M_PI/180.0;};

double trans_range (double x){
    return (1.0 + (x / M_PI))*0.5;
}

double lon_mercator_proj (double lon) {
    double lon_r=degree_to_radian(lon);
    double y=log(tan(0.25*M_PI+0.5*lon_r));
    return y;
}

double euc_dist (double lat1,double lon1, double lat2,double lon2, double x_scale,double y_scale) {
    double x1= trans_range(degree_to_radian(lat1))*x_scale;
    double x2= trans_range(degree_to_radian(lat2))*x_scale;

    double y1= trans_range(lon_mercator_proj(lon1))*y_scale;
    double y2= trans_range(lon_mercator_proj(lon2))*y_scale;

    double dist= sqrt(pow((x2-x1),2.0)+pow((y2-y1),2.0));
    return dist;
}
};

int main(){
    double lat_1, lat_2,lon_1, lon_2, x_scale, y_scale;
    double e_dist;
    cin>>lat_1>>lat_2>>lon_1>>lon_2>>x_scale>>y_scale;

    euc_distance ed;

    e_dist=ed.euc_dist(lat_1, lat_2,lon_1, lon_2, x_scale,y_scale);
    cout<<e_dist;
    
    return 0;
}