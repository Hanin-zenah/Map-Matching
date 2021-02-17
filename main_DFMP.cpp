/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "preprocessing/graph.h"
#include "Stats/graph_for_hist.h"
#include "grid/graph_grid.h"
#include "grid/graph_grid_starting_node.h"
#include "preprocessing/graph_subsampling.h"
#include "trajectories/trajectory.h"
#include "trajectories/trajectory_split.h"
#include "HMM/HMM.h"
#include "DF_sol/disc_frechet_grid.h"
#include "DF_sol/freespace_shortest_path.h"
#include <chrono>
#include <limits.h> 
#include <stdlib.h>  // for strtol


int main(int argc, char** argv) {
    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[1], &after_graph);

    double lon_min, lat_min, lat_scale, lon_scale;
    lon_min = after_graph.original_min_long;
    lat_min = after_graph.original_min_lat;
    lat_scale = after_graph.lat_scale;
    lon_scale = after_graph.lon_scale;

    auto start_grid = std::chrono::high_resolution_clock::now();

    Grid grid = GRID_INIT;
    std::string grid_size_str = argv[2];
    double grid_size = std::stod(grid_size_str); 
    Build_grid build_grid;
    build_grid.make_grids(&after_graph, &grid, grid_size);

    auto elapsed_grid = std::chrono::high_resolution_clock::now() - start_grid;
    long long microseconds_grid = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_grid).count();
    cout<<"Building Grid Duration in microseconds: " << microseconds_grid << endl;

    Traj tjtr;
    vector<Trajectory> trajs = tjtr.read_trajectories(argv[3], 4, lon_min, lat_min, lat_scale, lon_scale);
    /* london-geq50m-clean-unmerged-2016-10-09-greater-london.binTracks */
    /* saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks */

    Grid grid_copy =grid;
    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];

    tjtr.calc_traj_edge_cost(&traj);
    double traj_length = tjtr.calc_traj_length(&traj);

    std::string threshold_str = argv[4];
    double subsample_traj_thr = std::stod(threshold_str);

    Traj_subsample traj_sub;
    traj_sub.subsample_traj(&traj, subsample_traj_thr);

    tjtr.write_traj(&traj, argv[5]);

    FSgraph fsgraph = FSGRAPH_INIT; 
    Discrete_Frechet DF;

    auto start_frechet = std::chrono::high_resolution_clock::now();

    FSpair last_pair = DF.min_eps(&after_graph, &traj, &fsgraph, &grid);

    auto elapsed_frechet = std::chrono::high_resolution_clock::now() - start_frechet;
    long long microseconds_frechet = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_frechet).count();
    cout<<"Frechet matching duration in microseconds: " << microseconds_frechet << endl;

    cout<<"final fsgraph.eps: " << fsgraph.eps << endl;
     
    DF.write_sur_graph(&fsgraph, &after_graph, argv[6]);
    
    cout<<"path cost in m: " << DF.path_cost(&fsgraph, &after_graph, last_pair) << endl;

    DF.print_path(&fsgraph, &traj, &after_graph, argv[7], last_pair);
    cout<<"finished writing out path"<<endl;

    /* run dijkstra on the freespace */
    start_frechet = std::chrono::high_resolution_clock::now();

    stack<FSnode*> SP = find_shortest_path(&fsgraph, &after_graph, traj.length, argv[8]); // delete the i

    elapsed_frechet = std::chrono::high_resolution_clock::now() - start_frechet;
    microseconds_frechet = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_frechet).count();
    cout<<"dijk SP Frechet matching duration in microseconds: " << microseconds_frechet << endl;

    DF.cleanup(&fsgraph);
    tjtr.cleanup_trajectory(&traj);

return 0;
}

