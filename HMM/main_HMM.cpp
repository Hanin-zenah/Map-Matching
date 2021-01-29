/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "../preprocessing/graph.h"
#include "../Stats/graph_for_hist.h"
#include "../grid/graph_grid.h"
#include "../grid/graph_grid_starting_node.h"
#include "../preprocessing/graph_subsampling.h"
#include "../DF_sol/disc_frechet.h"
#include "../trajectories/trajectory.h"
#include "../trajectories/trajectory_split.h"
#include "HMM.h"
// #include "freespace_shortest_path.h"
#include <chrono>


int main(int argc, char** argv) {

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
    
    Build_grid build_grid;
    build_grid.make_grids(&after_graph, &grid, grid_size);

    auto elapsed_grid = std::chrono::high_resolution_clock::now() - start_grid;
    long long microseconds_grid = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_grid).count();

    cout<<"Building Grid Duration in microseconds: " << microseconds_grid << endl;

    // output_grid(&grid, "graph_grid_cell_offset_and_node_list.txt"); // for checking purposes

    vector<Trajectory> trajs = read_trajectories("saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 5, lon_min, lat_min, lat_scale, lon_scale);

    // vector<Trajectory> trajs = read_trajectories("saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 5, lon_min, lat_min, lat_scale, lon_scale);
    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];

    /* london-geq50m-clean-unmerged-2016-10-09-greater-london.binTracks */
    /* saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks */

    cout << "finished extracting the trajectory\n";
    calc_traj_edge_cost(&traj);
    double traj_length = calc_traj_length(&traj);
 
    subsample_traj(&traj, 50);

    write_traj(&traj, "saarland_traj.dat");

    // traj = trajs[1];
    // calc_traj_edge_cost(&traj);
    // subsample_traj(&traj, 40);
    // write_traj(&traj, "traj_frechet_with_sub_traj1.dat");
    // 

    // traj = trajs[2];
    // calc_traj_edge_cost(&traj);
    // subsample_traj(&traj, 40);
    // write_traj(&traj, "traj_frechet_with_sub_traj2.dat");

    // traj = trajs[3];
    // calc_traj_edge_cost(&traj);
    // subsample_traj(&traj, 40);
    // write_traj(&traj, "traj_frechet_with_sub_traj3.dat");
// 
    // traj = trajs[4];
    // calc_traj_edge_cost(&traj);
    // subsample_traj(&traj, 40);
    // write_traj(&traj, "traj_frechet_with_sub_traj4.dat");
// 
    // traj = trajs[5];
    // calc_traj_edge_cost(&traj);
    // subsample_traj(&traj, 40);
    // write_traj(&traj, "traj_frechet_with_sub_traj5.dat");
    

    HMM hmm;

    double sigma = hmm.sigma_est(&after_graph, &grid, &traj);

    double beta = hmm.beta_est(0.5, 100, 30);

    beta = 200; // stop overwriting;

    double radius = 500.00;

    int num_candidate = 50;

    auto start_HMM = std::chrono::high_resolution_clock::now();

    vector<int> best = hmm.best_path(&after_graph, &grid, &traj, num_candidate, sigma, beta, radius);

    vector<int> com_path = hmm.best_path_dijkstra(&after_graph, best);

    auto elapsed_HMM = std::chrono::high_resolution_clock::now() - start_HMM;
    long long microseconds_HMM = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_HMM).count();


    cout<<"finished nodes to nodes dijkstra\n";

    double HMM_length = hmm.HMM_path_cost(&after_graph, com_path);

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

    // /* try to fit the HMM results into a freespace and see what's the frechet distance between this HMM results and traj */

    Graph HMM_graph = GRAPH_INIT;

    hmm.make_a_HMM_graph(&after_graph, com_path, &HMM_graph);

    outedge_offset_array(&HMM_graph);
    inedge_offset_array(&HMM_graph);

    cout<<"convert the path to graph\n";

    hmm.write_HMM_graph(&after_graph, com_path, "HMM_matching_path_beta_200_radius_500_can_50_traj0_london.dat");

    // FSgraph fsgraph = FSGRAPH_INIT; 
    // FSpair last_pair = min_eps(&HMM_graph, &traj, &fsgraph, 750.00);
    // cout<<"final fsgraph.eps: "<<fsgraph.eps<<endl;
        
    // cleanup(&fsgraph);
    // cleanup_trajectory(&traj);


    return 0;
}

