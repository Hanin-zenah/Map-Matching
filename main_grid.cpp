/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h"
#include "graph_for_hist.h"
#include "graph_grid.h"
#include "graph_subsampling.h"
#include "disc_frechet_grid.h"
#include "trajectory.h"
#include "trajectory_split.h"
#include "graph_grid_starting_node.h"
#include "freespace_shortest_path.h"
#include <chrono>


int main(int argc, char** argv) {
    // if(argc < 2) {
        // cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        // return 1;
    // }

    /* read graph from given file */
    // Graph graph = GRAPH_INIT;
    // read_file(argv[1], &graph);

    // /* bounding box */
    // double lat_min = graph.min_lat;
    // double lat_max = graph.max_lat;
    // double lon_min = graph.min_long;
    // double lon_max = graph.max_long;

    // graph.original_min_lat = lat_min;
    // graph.original_min_long = lat_max;
    // graph.original_max_lat = lon_min;
    // graph.original_max_long = lon_max;

    // cout<<graph.original_min_lat<<" "<<graph.original_min_long<<" "<<graph.original_max_lat<<" "<<graph.original_max_long<<endl;
    // Bounds bd;
    // double g_dist1 = bd.geodesic_dist(lat_min,lon_min,lat_max,lon_min);
    // double g_dist2 = bd.geodesic_dist(lat_min,lon_max,lat_min,lon_min);
    // double g_dist3 = bd.geodesic_dist(lat_max,lon_max,lat_max,lon_min);
// 
    // cout<<"geo disc: "<<g_dist1<<" "<<g_dist2<<" "<<g_dist3<<endl;
    // Euc_distance ed;
    // double e_dist1, e_dist2, e_dist3;
    /* calculate the "pixel" euclidean distance between the bounding points */
    // double lon_min_to_y = ed.lon_mercator_proj(lon_min, lon_min);
    // double lon_max_to_y = ed.lon_mercator_proj(lon_max, lon_min);
    // double lat_min_to_x = ed.lat_mercator_proj(lat_min, lat_min);
    // double lat_max_to_x = ed.lat_mercator_proj(lat_max, lat_min);   
    // e_dist1 = ed.euc_dist(lat_min_to_x,lon_min_to_y,lat_max_to_x,lon_min_to_y);
    // e_dist2 = ed.euc_dist(lat_min_to_x,lon_max_to_y,lat_min_to_x,lon_min_to_y);
    // e_dist3 = ed.euc_dist(lat_max_to_x,lon_max_to_y,lat_max_to_x,lon_min_to_y);
    // /* calculates the cost of the edges */
    // double lon_scale = (g_dist2/e_dist2 + g_dist3/e_dist3) * 0.5;
    // double lat_scale = g_dist1/e_dist1;
    // graph.lon_scale = lon_scale;
    // graph.lat_scale = lat_scale;

    // cout<<"euc dists: "<<e_dist1<<" "<<e_dist2<<" "<<e_dist3<<endl;
    // cout<<"scale: "<<lon_scale<<lat_scale<<endl;

    // ed.calc_edge_cost(&graph, lat_scale, lon_scale);

    // Graph SCC_graph = GRAPH_INIT;
    // scc_graph(&graph, &SCC_graph);            
    // /* sub sampling */
    // cout<<"done scc graph"<<endl;
    // cout<<"before subsampling scc # edges and nodes: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;

    // subsampling(&SCC_graph, 40);

    // outedge_offset_array(&SCC_graph);
    // inedge_offset_array(&SCC_graph);    
    // cout<<"original box: "<<graph.original_min_lat<<" "<<graph.original_min_long<<" "<<graph.original_max_lat<<" "<<graph.original_max_long<<endl;
    // output_graph(&SCC_graph, "saarland_all_sub_40_projected.txt", lat_scale, lon_scale, lat_min, lat_max, lon_min, lon_max);

    // cout<<"before second SCC # edges: "<<SCC_graph.edges.size()<<" #nodes "<<SCC_graph.nodes.size()<<endl;
    // Graph SCC_graph2 = GRAPH_INIT;
    // scc_graph(&SCC_graph, &SCC_graph2);
    // cout<<"after second SCC # edges: "<<SCC_graph2.edges.size()<<" #nodes "<<SCC_graph2.nodes.size()<<endl;  


    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[1], &after_graph);
    // write_graph(&after_graph, "saarland_all_with_sub.dat");

    double lon_min, lat_min, lat_scale, lon_scale;
    lon_min = after_graph.original_min_long;
    lat_min = after_graph.original_min_lat;
    lat_scale = after_graph.lat_scale;
    lon_scale = after_graph.lon_scale;

    cout<< "after graph bounding box: "<<after_graph.min_lat<<" "<<after_graph.min_long<<" "<<after_graph.max_lat<<" "<<after_graph.max_long<<endl;
    cout<<"original mins for projection: "<<lon_min<<" "<<lat_min<<endl;
    cout<< "lat_scale: "<<lat_scale<< " lon_scale: "<<lon_scale<<endl;

    auto t3 = chrono::high_resolution_clock::now();
    // change the measure to microsecond or nanosecond;
    //emurate over a radius, or top 50 or 100 cloest -- should still remain very quick
    //essentially stay lenant
    //half the graph vertices -- that case may be slower

    Grid grid = GRID_INIT;
    make_grids(&after_graph, &grid, 1000.00);
    output_grid(&grid, "graph_grid_cell_offset_and_node_list.txt");
    auto t4 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<std::chrono::milliseconds>( t4 - t3 ).count();
    cout << "Duration in milliseconds: " << duration1 << endl;


    vector<Trajectory> trajs = read_trajectories("saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 3, lon_min, lat_min, lat_scale, lon_scale);
    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];
    
    cout << "finished extracting the trajectory\n";
    calc_traj_edge_cost(&traj);

    subsample_traj(&traj, 15);
    cout << "length of trajectory :"<< traj.length << endl;

    write_traj(&traj, "traj_frechet_with_sub.dat");

    auto t1 = chrono::high_resolution_clock::now();
// 
    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> PQ;

    PQ = GridSearch(&after_graph, &grid, traj_nd);
// 
    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    cout << "Duration in milliseconds: " << duration << endl;
    cout<<"number of closest nodes: "<<PQ.size()<<endl;
    cout<<"range size: "<<grid.curr_range<<endl;
    // while (!PQ.empty()){
        // cout<<" next closest dist ID and distance: "<<PQ.top().first<<" "<<PQ.top().second<<endl;
        // PQ.pop();
    // }
    cout<<"distance to peak: "<<grid.dist_to_peak<<endl;
    cout<<"PQ length before extending: "<<PQ.size()<<endl;

    FSgraph fsgraph = FSGRAPH_INIT; 
    FSpair last_pair = min_eps(&after_graph, &traj, &fsgraph, &grid);

    cleanup(&fsgraph);
    cleanup_trajectory(&traj);
    return 0;
}