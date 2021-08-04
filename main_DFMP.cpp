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
#include <stdlib.h> 
#include <nlohmann/json.hpp>


int main(int argc, char** argv) {
    if(argc < 8) {
        cerr << "Not enough arguments; please provide all required inputs";
        return 1;
    }

    Build_grid build_grid;
    Traj tjtr;
    Traj_subsample traj_sub;
    Discrete_Frechet DF;
    Freespace_Dijkstra FD;

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
    build_grid.make_grids(&after_graph, &grid, grid_size);

    cout<<"grid.num_columns and grid.num_rows: "<<grid.num_columns<<" "<<grid.num_rows<<endl;

    auto elapsed_grid = std::chrono::high_resolution_clock::now() - start_grid;
    long long microseconds_grid = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_grid).count();
    cout<<"Building Grid Duration in microseconds: " << microseconds_grid << endl;

    
    vector<Trajectory> trajs = tjtr.read_processed_trajectories(argv[3],lon_min, lat_min, lat_scale, lon_scale);
    /* london-geq50m-clean-unmerged-2016-10-09-greater-london.binTracks */
    /* saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks */

    int number_traj = tjtr.num_trajectories(argv[3]);  

    cout<< "number_traj: "<<number_traj<<endl;
    std::string threshold_str = argv[4];
    double subsample_traj_thr = std::stod(threshold_str);
    
    Trajectory traj;
    Point* traj_nd;

    for(int i = 0; i < number_traj; i++) {
        cout<<"grid -> curr_range: "<<grid.curr_range<<endl;
        grid.curr_range = 0;

        traj = trajs[i];
        traj_nd = traj.points[0];
        tjtr.calc_traj_edge_cost(&traj);
        double traj_length = tjtr.calc_traj_length(&traj);

        traj_sub.subsample_traj(&traj, subsample_traj_thr);
        tjtr.write_traj(&traj, argv[5]);

        FSgraph fsgraph = FSGRAPH_INIT; // re-initialize?

        auto start_frechet = std::chrono::high_resolution_clock::now();

        FSpair last_pair = DF.min_eps(&after_graph, &traj, &fsgraph, &grid); 

        // make an error message and terminate the code if the initial traj point is out of the bounding box

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

        stack<FSnode*> SP = FD.find_shortest_path(&fsgraph, &after_graph, traj.length, argv[8]); // delete the i

        elapsed_frechet = std::chrono::high_resolution_clock::now() - start_frechet;
        microseconds_frechet = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_frechet).count();
        cout<<"dijk SP Frechet matching duration in microseconds: " << microseconds_frechet << endl;

        vector<double> stats;
        stats.push_back(fsgraph.eps);
        stats.push_back(microseconds_frechet);
        DF.write_path_json(&fsgraph, &traj, &after_graph, argv[9], last_pair, stats);

        DF.cleanup(&fsgraph);
        tjtr.cleanup_trajectory(&traj);
    } // end the loop after matching all trajectories

    return 0;
}

