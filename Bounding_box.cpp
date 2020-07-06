
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

    double degree_to_radian(double a) {return a*M_PI/180;};

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

int main(){
    double lat_min,lat_max,lon_min,lon_max;
    double dist1, dist2, dist3;
    cin>>lat_min>>lat_max>>lon_min>>lon_max;

    Bounds bd;

    dist1=bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    dist2=bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    dist3=bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
    cout<<dist1<<endl<< dist2<<endl<<dist3;
    return 0;
}