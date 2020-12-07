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

    // cout<< "after graph bounding box: "<<after_graph.min_lat<<" "<<after_graph.min_long<<" "<<after_graph.max_lat<<" "<<after_graph.max_long<<endl;
    // cout<<"original mins for projection: "<<lon_min<<" "<<lat_min<<endl;
    // cout<< "lat_scale: "<<lat_scale<< " lon_scale: "<<lon_scale<<endl;

    auto start_grid = std::chrono::high_resolution_clock::now();

    Grid grid = GRID_INIT;

    double grid_size = 500.00;
    make_grids(&after_graph, &grid, grid_size);

    auto elapsed_grid = std::chrono::high_resolution_clock::now() - start_grid;
    long long microseconds_grid = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_grid).count();

    cout<<"Building Grid Duration in microseconds: " << microseconds_grid << endl;

    // output_grid(&grid, "graph_grid_cell_offset_and_node_list.txt"); // for checking purposes


    vector<Trajectory> trajs = read_trajectories("saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 6, lon_min, lat_min, lat_scale, lon_scale);
    Trajectory traj = trajs[2];
    Point* traj_nd = traj.points[0];
    // 
    cout << "finished extracting the trajectory\n";
    calc_traj_edge_cost(&traj);
    double traj_length = calc_traj_length(&traj);
// 
    subsample_traj(&traj, 40);
    
// 
    write_traj(&traj, "traj_frechet_with_sub_HMM_traj3.dat");


double sigma = sigma_est(&after_graph, &grid, &traj);
// 
double beta = beta_est(0.5, 100, 30);

//   need to make sure the order of emission vector, transition vector, state vector all match!!!!

beta = 200;

double radius = 500.00;

int num_candidate = 25;

auto start_HMM = std::chrono::high_resolution_clock::now();

vector<int> best = best_path(&after_graph, &grid, &traj, num_candidate, sigma, beta, radius);

// cout<<"best_path found\n";

vector<int> com_path = best_path_dijkstra(&after_graph, best);

auto elapsed_HMM = std::chrono::high_resolution_clock::now() - start_HMM;
long long microseconds_HMM = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_HMM).count();



cout<<"finished nodes to nodes dijkstra\n";
 
write_HMM_graph(&after_graph, com_path, "HMM_matching_path_beta_200_radius_500_can_25_traj3.dat");

double HMM_length = HMM_path_cost(&after_graph, com_path);

cout<<"Building Grid Duration in microseconds: " << microseconds_grid << endl;

cout<<"grid size: "<<grid_size<<endl;

cout<<"HMM sigma: "<<sigma<<endl;

cout<<"HMM beta: "<<beta<<endl;

cout<<"HMM radius: "<<radius<<endl;

cout<<"HMM num candidate: "<<num_candidate<<endl;

cout<<"HMM in microseconds: " << microseconds_HMM << endl;

cout <<"length of trajectory :"<< traj.length << endl;

cout<<"length of the traj: "<<traj_length<<endl;

cout<<"length of the HMM matching path: "<<HMM_length<<endl;

// try to fit the HMM results into a freespace and see what's the frechet distance between this HMM results and traj

cleanup_trajectory(&traj);


return 0;
}

