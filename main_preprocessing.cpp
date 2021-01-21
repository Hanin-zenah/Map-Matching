/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h"
#include "graph_for_hist.h"
#include "graph_grid.h"
#include "graph_grid_starting_node.h"
#include "graph_subsampling.h"
#include "disc_frechet_grid.h"
#include "trajectory.h"
#include "trajectory_split.h"
#include "HMM.h"
// #include "freespace_shortest_path.h"
#include <chrono>


int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }
// 
    // /* read graph from given file */
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);
// 
    /* bounding box */
    double lat_min = graph.min_lat;
    double lat_max = graph.max_lat;
    double lon_min = graph.min_long;
    double lon_max = graph.max_long;
// 
    graph.original_min_lat = lat_min;
    graph.original_min_long = lat_max;
    graph.original_max_lat = lon_min;
    graph.original_max_long = lon_max;
// 
    cout<<graph.original_min_lat<<" "<<graph.original_min_long<<" "<<graph.original_max_lat<<" "<<graph.original_max_long<<endl;
    Bounds bd;
    double g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    double g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    double g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
// 
    cout<<"geo disc: "<<g_dist1<<" "<<g_dist2<<" "<<g_dist3<<endl;
    Euc_distance ed;
    double e_dist1, e_dist2, e_dist3;
    // /* calculate the "pixel" euclidean distance between the bounding points */
    double lon_min_to_y = ed.lon_mercator_proj(lon_min, lon_min);
    double lon_max_to_y = ed.lon_mercator_proj(lon_max, lon_min);
    double lat_min_to_x = ed.lat_mercator_proj(lat_min, lat_min);
    double lat_max_to_x = ed.lat_mercator_proj(lat_max, lat_min);   
    e_dist1 = ed.euc_dist(lat_min_to_x,lon_min_to_y,lat_max_to_x,lon_min_to_y);
    e_dist2 = ed.euc_dist(lat_min_to_x,lon_max_to_y,lat_min_to_x,lon_min_to_y);
    e_dist3 = ed.euc_dist(lat_max_to_x,lon_max_to_y,lat_max_to_x,lon_min_to_y);
    /* calculates the cost of the edges */
    double lon_scale = (g_dist2/e_dist2 + g_dist3/e_dist3) * 0.5;
    double lat_scale = g_dist1/e_dist1;
    graph.lon_scale = lon_scale;
    graph.lat_scale = lat_scale;
// 
    cout<<"euc dists: "<<e_dist1<<" "<<e_dist2<<" "<<e_dist3<<endl;
    cout<<"scale: "<<lon_scale<<lat_scale<<endl;
// 
    ed.calc_edge_cost(&graph, lat_scale, lon_scale);

    graph_edge_cost(&graph, "Saarland_all_edges.dat"); 
// 
    Graph SCC_graph = GRAPH_INIT;
    scc_graph(&graph, &SCC_graph);            
    /* sub sampling */
    cout<<"done scc graph"<<endl;
    cout<<"before subsampling scc # edges and nodes: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;
// 
    subsampling(&SCC_graph, 40);

    graph_edge_cost(&SCC_graph, "SCC_Saarland_all_edges.dat"); 
// 
    outedge_offset_array(&SCC_graph);
    inedge_offset_array(&SCC_graph);    
    cout<<"original box: "<<graph.original_min_lat<<" "<<graph.original_min_long<<" "<<graph.original_max_lat<<" "<<graph.original_max_long<<endl;
    output_graph(&SCC_graph, "greater-london-_sub_50_projected.txt", lat_scale, lon_scale, lat_min, lat_max, lon_min, lon_max);
// 
    cout<<"before second SCC # edges: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;
    Graph SCC_graph2 = GRAPH_INIT;
    scc_graph(&SCC_graph, &SCC_graph2);
    cout<<"after second SCC # edges: "<<SCC_graph2.edges.size()<<" #nodes "<<SCC_graph2.nodes.size()<<endl;  

    // read_processed_graph(argv[1], &graph);
    // write_graph(&graph, "greater-london_sub_50_projected.dat");
    
return 0;
}

