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
#include "freespace_shortest_path.h"
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


    vector<Trajectory> trajs = read_trajectories("london-geq50m-clean-unmerged-2016-10-09-greater-london.binTracks", 15, lon_min, lat_min, lat_scale, lon_scale);
    /* london-geq50m-clean-unmerged-2016-10-09-greater-london.binTracks */
    /* saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks */

    for (int i = 0; i<2; i++){

    Grid grid_copy =grid;
    Trajectory traj = trajs[i];
    Point* traj_nd = traj.points[0];
    
    cout << "For trajectory "<<i<<" :"<<endl;
    calc_traj_edge_cost(&traj);
    double traj_length = calc_traj_length(&traj);
// 
    subsample_traj(&traj, 40);

    char buff1[100];
    snprintf(buff1, sizeof(buff1), "london_traj_frechet_with_sub_traj%d.dat", i);

    write_traj(&traj, buff1);

    FSgraph fsgraph = FSGRAPH_INIT; 

    auto start_frechet = std::chrono::high_resolution_clock::now();

    FSpair last_pair = min_eps(&after_graph, &traj, &fsgraph, &grid_copy);

    auto elapsed_frechet = std::chrono::high_resolution_clock::now() - start_frechet;
    long long microseconds_frechet = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_frechet).count();
    cout<<"Frechet matching duration in microseconds: " << microseconds_frechet << endl;

    cout<<"final fsgraph.eps: "<<fsgraph.eps<<endl;

    cout<<"fsgraph -> fsedges.size: "<<fsgraph.fsedges.size()<<endl;
    cout<<"fsgraph -> fsnodes.size: "<<fsgraph.fsnodes.size()<<endl;

    char buff2[100];
    snprintf(buff2, sizeof(buff2), "london_full_sur_graph_frechet_traj%d.dat", i);
     
    write_sur_graph(&fsgraph, &after_graph, buff2);
    
    cout<<"path cost in m: "<<path_cost(&fsgraph, &after_graph, last_pair)<<endl;

    char buff3[100];
    snprintf(buff3, sizeof(buff3), "london_full_frechet_path_traj%d.dat", i);
    string buffAsStdStr = buff3;
    
    print_path(&fsgraph, &traj, &after_graph, buff3, last_pair);
    cout<<"finished writing out path"<<endl;

    // /* run dijkstra on the freespace */
    start_frechet = std::chrono::high_resolution_clock::now();
    stack<FSnode*> SP = find_shortest_path(&fsgraph, &after_graph, traj.length, i); // delete the i
    elapsed_frechet = std::chrono::high_resolution_clock::now() - start_frechet;
    microseconds_frechet = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_frechet).count();
    cout<<"dijk SP Frechet matching duration in microseconds: " << microseconds_frechet << endl;



    cleanup(&fsgraph);

    cleanup_trajectory(&traj);


    } // for the loop




return 0;
}

