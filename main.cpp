/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "graph_subsampling.h"
#include "scale_projection.h"
#include "graph_subsampling.h"
#include <chrono>

using namespace std::chrono;

double get_average_cost(Graph* graph) {
    double total = 0;
    for(int i = 0 ; i < graph -> edges.size(); i++) {
        total += graph -> edges[i].cost;
    }
    return total / graph -> edges.size();
}
int get_above_k(Graph* graph) {
    int total = 0; 
    for(int i = 0; i < graph -> edges.size(); i++) {
        if(graph -> edges[i].cost >= 100) {
            total++;
        }
    }
    return total;
}
int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read graph from given file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);

    //write file with the projected x_y coordinates (+ .dat file to graph)


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
    double lon_min_to_y = ed.lon_mercator_proj(lon_min, lon_min);
    double lon_max_to_y = ed.lon_mercator_proj(lon_max, lon_min);
    double lat_min_to_x = ed.lat_mercator_proj(lat_min, lat_min);
    double lat_max_to_x = ed.lat_mercator_proj(lat_max, lat_min);

    e_dist1 = ed.euc_dist(lat_min_to_x,lon_min_to_y,lat_max_to_x,lon_min_to_y, 1, 1);
    e_dist2 = ed.euc_dist(lat_min_to_x,lon_max_to_y,lat_min_to_x,lon_min_to_y,1 ,1);

    //calculates the cost of the edges 
    double x_scale = (g_dist2+g_dist3)*0.5/e_dist2;
    double y_scale = g_dist1/e_dist1;
    ed.calc_edge_cost(&graph, x_scale, y_scale);


    //strongly connected componetns
    Graph SCC_graph = GRAPH_INIT;
    scc_graph(&graph, &SCC_graph);

    cout << "number of nodes: " << SCC_graph.n_nodes << endl;
    cout << "number of edges: " << SCC_graph.n_edges << endl;


    // cout << get_average_cost(&SCC_graph) << endl;
    // cout << get_above_k(&graph) << endl;

    //sub sampling 
    // auto start = high_resolution_clock::now(); 
    // subsampling(&graph, 100);
    // auto stop = high_resolution_clock::now();

    // auto duration = duration_cast<microseconds>(stop - start);
    // cout << "Time taken by function: "
    //      << duration.count() << " microseconds" << endl; 

    return 0;
}