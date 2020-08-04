/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "graph_subsampling.h"
#include "scale_projection.h"
#include "graph_self_loop.h" 
#include "graph_for_hist.h"


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

    //generate a graph with only edge costs to plot histogram
    graph_edge_cost(&graph, "graph_for_hist.dat");

    //strongly connected componetns
    Graph SCC_graph = GRAPH_INIT;
    scc_graph(&graph, &SCC_graph);

    //sub-sampling 
    subsampling(&SCC_graph, 100);

    cout<<"number of nodes after subsampling before second SCC: "<<SCC_graph.nodes.size()<<endl;
    cout<<"number of edges after subsampling before second SCC: "<<SCC_graph.edges.size()<<endl;
    //SCC_graph.n_nodes = SCC_graph.nodes.size();

    //generate strongly connected components again
    Graph SCC_graph2 = GRAPH_INIT;
    scc_graph(&SCC_graph, &SCC_graph2);
    cout<<"number of nodes after second SCC: "<<SCC_graph2.nodes.size()<<endl;
    cout<<"number of nodes after second SCC: "<<SCC_graph2.edges.size()<<endl;
    //write_graph(&SCC_graph, "SCC_graph_subsampled.dat");

    

    //checking if there is any self-looping nodes and print out
    vector<int> self_looped_nodes = self_loop_nodes(&SCC_graph);
    output_self_looping_nodes(self_looped_nodes, "self_looped_nodes.txt");

    return 0;
}