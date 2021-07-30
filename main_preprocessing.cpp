/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "preprocessing/graph.h"
#include "preprocessing/k_skip_cover.cpp"
#include "Stats/graph_for_hist.h"
#include "grid/graph_grid.h"
#include "grid/graph_grid.h"
#include "preprocessing/graph_subsampling.h"
#include "trajectories/trajectory.h"
#include "trajectories/trajectory_split.h"
#include <chrono>
#include <limits.h> 
#include <stdlib.h> 


int main(int argc, char** argv) {
    if(argc < 4) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./preprocess inGraph.txt subSampleThreshold outGraoh.txt kSkip\n";
        return 1;
    }
 
    /* read graph from given file */
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);

    /* bounding box */
    double lat_min = graph.min_lat;
    double lat_max = graph.max_lat;
    double lon_min = graph.min_long;
    double lon_max = graph.max_long;
 
    graph.original_min_lat = lat_min;
    graph.original_min_long = lon_min; 
    graph.original_max_lat = lat_max;
    graph.original_max_long = lon_max;
 
    cout<<"bounding box: "<<graph.original_min_lat<<" "<<graph.original_min_long<<" "<<graph.original_max_lat<<" "<<graph.original_max_long<<endl;
    Bounds bd;
    double g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    double g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    double g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);

    Euc_distance ed;
    double e_dist1, e_dist2, e_dist3;
    /* calculate the "pixel" euclidean distance between the bounding points */
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

    ed.calc_edge_cost(&graph, lat_scale, lon_scale);

    Graph SCC_graph = GRAPH_INIT;
    scc_graph(&graph, &SCC_graph);    

    cout<<"original graph # edges and nodes: "<<graph.edges.size()<<" #nodes "<<graph.nodes.size()<<endl;

    /* sub-sampling */
    cout<<"before subsampling scc # edges and nodes: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;

    std::string threshold_str = argv[2];
    double threshold = std::stod(threshold_str); 

    subsampling(&SCC_graph, threshold); 
    outedge_offset_array(&SCC_graph);
    inedge_offset_array(&SCC_graph); 
    
    // COMPUTE kSKIP cover in SCC_graph
    if( argc >= 5 ){
        int skipk = std::stoi(argv[4]);
        cout << "Computing Skip Cover for k="   << skipk << " ..." << endl;
        int nof_cover_nodes = k_skip_cover( skipk, &SCC_graph );
        cout << "   Picked nofCoverNodes="      << nof_cover_nodes << endl;
    }

    output_graph(&SCC_graph, argv[3], lat_scale, lon_scale, lat_min, lat_max, lon_min, lon_max); //"greater-london-_sub_50_projected.txt"
    
    /* checking sub-sampling results */
    cout<<"before second SCC # edges: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;
    Graph SCC_graph2 = GRAPH_INIT;
    scc_graph(&SCC_graph, &SCC_graph2);
    cout<<"after second SCC # edges: "<<SCC_graph2.edges.size()<<" #nodes "<<SCC_graph2.nodes.size()<<endl;  

return 0;
}

