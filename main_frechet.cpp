/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h"
#include "graph_for_hist.h"
#include "graph_for_hist.h"
#include "graph_subsampling.h"
#include "disc_frechet_v2.h"
#include "trajectory.h"
#include "trajectory_split.h"
#include "starting_node_look_up.h"
#include "freespace_shortest_path.h"
#include <chrono>


//print out the nodes that have the same tid and vid with different memory address
// void print_dupilcates(FSgraph* fsgraph) {
    // for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        // for(int j = 0; j < fsgraph -> fsnodes.size(); j++) {
            // if(i != j) {
                // if(fsgraph -> fsnodes[i] -> vid == fsgraph -> fsnodes[j] -> vid && 
                                // fsgraph -> fsnodes[i] -> tid == fsgraph -> fsnodes[j] -> tid) {
                    // cout << "DUP: " << fsgraph -> fsnodes[i] -> vid << " " << fsgraph -> fsnodes[i] -> tid << endl; 
                    // cout <<"indices: " << i << " " << j << endl;
                // }
            // }   
        // }
    // }
// }

int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

//     /* read graph from given file */
//     Graph graph = GRAPH_INIT;
//     read_file(argv[1], &graph);
//     // 

//     // /* bounding box */
//     double lat_min = graph.min_lat;
//     double lat_max = graph.max_lat;
//     double lon_min = graph.min_long;
//     double lon_max = graph.max_long;
//     cout<<lat_min<<" "<<lat_max<<" "<<lon_min<<" "<<lon_max<<endl;
//     Bounds bd;
//     double g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
//     double g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
//     double g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
//     cout<<g_dist1<<" "<<g_dist2<<" "<<g_dist3<<endl;
//     Euc_distance ed;
//     double e_dist, e_dist1, e_dist2, e_dist3;
//     // /* calculate the "pixel" euclidean distance between the bounding points */
//     double lon_min_to_y = ed.lon_mercator_proj(lon_min, lon_min);
//     double lon_max_to_y = ed.lon_mercator_proj(lon_max, lon_min);
//     double lat_min_to_x = ed.lat_mercator_proj(lat_min, lat_min);
//     double lat_max_to_x = ed.lat_mercator_proj(lat_max, lat_min);   
//     e_dist1 = ed.euc_dist(lat_min_to_x,lon_min_to_y,lat_max_to_x,lon_min_to_y, 1, 1);
//     e_dist2 = ed.euc_dist(lat_min_to_x,lon_max_to_y,lat_min_to_x,lon_min_to_y,1 ,1);
//     /* calculates the cost of the edges */
//     double x_scale = (g_dist2+g_dist3)*0.5/e_dist2;
//     cout<< "x_scale: "<<x_scale<<endl;
//     double y_scale = g_dist1/e_dist1;
//     cout<< "y_scale: "<<y_scale<<endl;
//     ed.calc_edge_cost(&graph, x_scale, y_scale);

//     graph_edge_cost(&graph, "Graph_for_hist_y_x.dat");
// // 
//     // /* strongly connected componetns */
//     Graph SCC_graph = GRAPH_INIT;
//     scc_graph(&graph, &SCC_graph);
//     // /* sub sampling */
//      cout<<"done scc graph"<<endl;

//      cout<<"before subsampling scc # edges and nodes: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;

    
// // 
//     // graph_edge_cost(&SCC_graph, "scc_graph_for_hist_y_x.dat");

//     auto t1 = chrono::high_resolution_clock::now();
//     subsampling(&SCC_graph, 80);

//     auto t2 = chrono::high_resolution_clock::now();
     
//     outedge_offset_array(&SCC_graph);
//     inedge_offset_array(&SCC_graph);    
    
//     auto t3 = chrono::high_resolution_clock::now();
//     auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
//     auto duration2 = chrono::duration_cast<std::chrono::milliseconds>( t3 - t1 ).count();

//     cout << "Duration in milliseconds: " << duration << endl;
//     cout << "Duration2 in milliseconds: " << duration2 << endl;

//     output_graph(&SCC_graph, "subsampled_scc_40.txt", x_scale, y_scale);
//     graph_edge_cost(&SCC_graph, "subsampled_scc_40_for_hist_y_x.dat");
//     cout<<"done subsampling graph"<<endl;
   
    // cout<<"before second SCC # edges: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;
    // Graph SCC_graph2 = GRAPH_INIT;
    // scc_graph(&SCC_graph, &SCC_graph2);
    // cout<<"after second SCC # edges: "<<SCC_graph2.edges.size()<<" #nodes "<<SCC_graph2.nodes.size()<<endl;

    // /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[1], &after_graph);
    write_graph(&after_graph, "saarland_all_with_sub.dat");

    // cout<<"number of nodes and edges in graph after scc: "<<after_graph.n_nodes<<" "<<after_graph.n_edges<<endl;

    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double x_scale = after_graph.x_scale;
    double y_scale = after_graph.y_scale;
    // cout<<"in the after graph: "<<lat_min<<" "<<lon_min<<" "<<x_scale<<" "<<y_scale<<endl;
 
    vector<Trajectory> trajs = read_trajectories("trajectories/saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 10, lon_min, lat_min); //trajectory no.3 has 5713 points
    // for(int i = 0; i < 10; i++) {
    //     Trajectory traj = trajs[i];
    //     Point* traj_nd = traj.points[0];
    //     calc_traj_edge_cost(&traj, x_scale, y_scale);
    //     subsample_traj(&traj, 15);
    //     cout << "length of trajectory :"<< traj.length << endl;
    //     // write_traj(&traj, "traj_frechet_with_sub.dat");
    //     FSgraph fsgraph = FSGRAPH_INIT; 
    //     FSpair last_pair = min_eps(&after_graph, &traj, &fsgraph, 1000, x_scale, y_scale);
    //     cout<<"first path cost: " << path_cost(&fsgraph, &after_graph, last_pair)<<endl;
    //     // print_path(&fsgraph, &traj, &after_graph, "frechet_path.dat", last_pair);
    //     auto b = chrono::high_resolution_clock::now();
    //     stack<FSnode*> SP = find_shortest_path(&fsgraph, &after_graph, traj.length);
    //     auto a = chrono::high_resolution_clock::now();
    //     auto duration2 = chrono::duration_cast<std::chrono::milliseconds>( a - b ).count();
    //     cout << "Duration2 in milliseconds: " << duration2 << endl;

    //     cout << "leashlength: " << fsgraph.eps << endl;
    //     cout << "free space edges: " << fsgraph.fsedges.size() << endl;
    //     cout << "----------------------------------\n";
    //     cleanup_trajectory(&traj);
    //     cleanup(&fsgraph);
    // }

    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];
    calc_traj_edge_cost(&traj, x_scale, y_scale);

    subsample_traj(&traj, 15);

    write_traj(&traj, "traj_frechet_with_sub.dat");

    FSgraph fsgraph = FSGRAPH_INIT; 
    FSpair last_pair = min_eps(&after_graph, &traj, &fsgraph, 500, x_scale, y_scale);
    write_fsgraph(&fsgraph, "fsgraph.dat");
    write_sur_graph(&fsgraph, &after_graph, "sur_graph_frechet.dat");
    cout<<"first path cost: " << path_cost(&fsgraph, &after_graph, last_pair)<<endl;
    print_path(&fsgraph, &traj, &after_graph, "frechet_path.dat", last_pair);

    /* run dijkstra on the freespace */
    stack<FSnode*> SP = find_shortest_path(&fsgraph, &after_graph, traj.length);


    cleanup(&fsgraph);
    cleanup_trajectory(&traj);
    return 0;
}