/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "scale_projection.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
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

    //calculate the "pixel" euclidean distance between the bounding points 
    e_dist1 = ed.euc_dist(lat_min,lon_min,lat_max,lon_min, 1, 1,lon_min,lat_min);
    e_dist2 = ed.euc_dist(lat_min,lon_max,lat_min,lon_min, 1, 1,lon_min,lat_min);
    e_dist3 = ed.euc_dist(lat_max,lon_max,lat_max,lon_min, 1, 1,lon_min,lat_min);

    //calculates the cost of the edges 
    double x_scale = (g_dist2+g_dist3)*0.5/e_dist2; // e_dist2=_e_dist3 doesn't matter which one we use
    double y_scale = g_dist1/e_dist1;
    ed.calc_edge_cost(&graph, x_scale, y_scale);

    //offset array
    string file_name;
    string file_name1;

    cout << "Please provide file name to store the outdegree offset array in: ";
    cin >> file_name;

    cout << "\nPlease provide another file name to store the indegree offset array in: ";
    cin >> file_name1;

    outdeg_offset_array(&graph, file_name);
    indeg_offset_array(&graph, file_name1);

    //strongly connectedd componetns

    return 0;
}