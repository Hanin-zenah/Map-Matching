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
#include <chrono>


int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[1], &after_graph);
    write_graph(&after_graph, "saarland_all_with_sub.dat");
// 
    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double lat_scale = after_graph.lat_scale;
    double lon_scale = after_graph.lon_scale;

    Grid grid = GRID_INIT;
    double grid_size = 500.00;
    make_grids(&after_graph, &grid, grid_size);
 
    vector<Trajectory> trajs = read_trajectories("saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 3, lon_min, lat_min, lat_scale, lon_scale);
    Trajectory traj = trajs[1];
    Point* traj_nd = traj.points[0];
    
    cout << "finished extracting the trajectory\n";
    calc_traj_edge_cost(&traj);

    subsample_traj(&traj, 15);
    cout << "length of trajectory :"<< traj.length << endl;

    write_traj(&traj, "traj_frechet_with_sub.dat");
    cout << "finished subsampling the trajectory\n";
// 
// 
    FSgraph fsgraph = FSGRAPH_INIT; 
    /* auto t1 = chrono::high_resolution_clock::now(); */

    FSpair last_pair = min_eps(&after_graph, &traj, &fsgraph, &grid);

    /* auto t2 = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    cout << "Duration in milliseconds: " << duration << endl; */
// 
    cout<<"final fsgraph.eps: "<<fsgraph.eps<<endl;
    // write_fsgraph(&fsgraph, "fsgraph.dat");

    // cout<<"fsgraph -> fsedges.size: "<<fsgraph.fsedges.size()<<endl;
    // cout<<"fsgraph -> fsnodes.size: "<<fsgraph.fsnodes.size()<<endl;

    write_sur_graph(&fsgraph, &after_graph, "full_sur_graph_frechet.dat");
    // cout<<"finished printing survided graph"<<endl;
    // cout<<path_cost(&fsgraph, &after_graph, last_pair)<<endl;

    // cout<<"finished printing path"<<endl;
    print_path(&fsgraph, &traj, &after_graph, "full_frechet_path.dat", last_pair);
    // cout<<"finished writing out path"<<endl;



    /* run dijkstra on the freespace */
    // auto t3 = chrono::high_resolution_clock::now();
    // stack<FSnode*> SP = find_shortest_path(&fsgraph, &after_graph, traj.length);

    // auto t4 = chrono::high_resolution_clock::now();

    // auto duration2 = chrono::duration_cast<std::chrono::milliseconds>( t4 - t3 ).count();

    // cout << "Duration2 in milliseconds: " << duration2 << endl;


// 
    cleanup(&fsgraph);
    cleanup_trajectory(&traj);
    return 0;
}