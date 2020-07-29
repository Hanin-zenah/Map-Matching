/* here goes the main function which will call all the necessary function to preprocess the graph */
//#include "graph_scc_lola_v3.h"
#include <vector> 
#include "graph.h" 
#include "graph_subsampling.h"
#include "scale_projection.h" 

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read file
    Graph graph = GRAPH_INIT; // graph initialization

    read_file(argv[1], &graph);

    //bounding box
    double lat_min = graph.min_lat;
    double lat_max = graph.max_lat;
    double lon_min = graph.min_long;
    double lon_max = graph.max_long;

    Bounds bd;
    double g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    double g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    double g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);

    Euc_distance ed;
    double e_dist, e_dist1, e_dist2, e_dist3;

    //calculate the "pixel" euclidean distance between the bounding points 
    double lon_min_to_x = ed.lon_mercator_proj(lon_min, lon_min);
    double lon_max_to_x = ed.lon_mercator_proj(lon_max, lon_min);
    double lat_min_to_y = ed.lat_mercator_proj(lat_min, lat_min);
    double lat_max_to_y = ed.lat_mercator_proj(lat_max, lat_min);

    e_dist1 = ed.euc_dist(lat_min_to_y,lon_min_to_x,lat_max_to_y,lon_min_to_x, 1, 1);
    e_dist2 = ed.euc_dist(lat_min_to_y,lon_max_to_x,lat_min_to_y,lon_min_to_x,1 ,1);
    //calculates the cost of the edges 
    double x_scale = (g_dist2+g_dist3)*0.5/e_dist2;
    double y_scale = g_dist1/e_dist1;
    ed.calc_edge_cost(&graph, x_scale, y_scale);


    // vector<int> in_offset{0, 2, 3, 4, 6, 7};
    // vector<int> out_offset{0, 1, 3, 4, 5, 7};
    // vector<int> in_edge{1, 5, 0, 6, 2, 4, 3};
    // vector<int> out_edge{0, 1, 2, 5, 3, 6, 4};


    subsampling(&graph, 100);//, in_edge, in_offset, out_edge, out_offset);

    // cout<<"out_edge: "<<endl;
    // for(int i = 0; i < out_edge.size(); i++) {
    // cout<< out_edge[i] << endl;}

    string file_name;
 
    cout << "Please provide file name to store the subsamplied graph in: ";
    cin >> file_name;

    output_graph(&graph, file_name);
    return 0;
}