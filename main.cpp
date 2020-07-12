/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "scale_projection.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name to read\n";
        return 1;
    }

    //read file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);

    double lat_min = graph.min_lat;
    double lat_max = graph.max_lat;
    double lon_min = graph.min_long;
    double lon_max = graph.max_long;

    //bounding box
    Bounds bd;
    double g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    double g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    double g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);

    //geodesic distance and eucledian distance 
    Euc_distance ed;
    double e_dist, e_dist1, e_dist2, e_dist3;

    e_dist1 = ed.euc_dist(lat_min,lon_min,lat_max,lon_min,lon_min, 1, 1);
    e_dist2 = ed.euc_dist(lat_min,lon_max,lat_min,lon_min,lon_min, 1, 1);
    e_dist3 = ed.euc_dist(lat_max,lon_max,lat_max,lon_min,lon_min, 1, 1);

    double lat_1, lat_2, lon_1, lon_2, x_scale, y_scale;
    cin >> lat_1 >> lon_1 >> lat_2 >> lon_2;

    x_scale = (g_dist2+g_dist3)*0.5/e_dist1;
    y_scale = g_dist1/e_dist1;

    e_dist = ed.euc_dist(lat_1, lon_1, lat_2, lon_2, lon_min,x_scale,y_scale);
    cout << g_dist1 << endl << e_dist1 << endl << e_dist;

    //get edges costs


    //offset array


    //strongly connectedd componetns

    return 0;
}