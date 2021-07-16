#include "interleaved.h"

int main(int argc, char** argv) {
    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph("../graphs/saarland_all_preprocessed.txt", &after_graph);
    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double lat_scale = after_graph.lat_scale;
    double lon_scale = after_graph.lon_scale;

    Build_grid builder;
    Grid grid = GRID_INIT;
    double grid_size = 250.00;
    builder.make_grids(&after_graph, &grid, grid_size);
    
    vector<int> indices; 
    // indices.push_back(0);
    indices.push_back(7);
    // indices.push_back(8);
    // indices.push_back(9);
    // indices.push_back(23);
    // indices.push_back(31);
    // indices.push_back(19);
    // indices.push_back(24);
    // indices.push_back(15);
    // indices.push_back(458);
    // indices.push_back(69);
    // indices.push_back(91);
    // indices.push_back(4);

    Traj t;
    Traj_subsample ts;
    for(int i = 0; i < indices.size(); i++) {
        Trajectory traj = t.read_trajectory_k("../trajectories/saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", indices[i], lon_min, lat_min, lat_scale, lon_scale);
        t.calc_traj_edge_cost(&traj);
        ts.subsample_traj(&traj, 15);
        // write_traj(&traj, "traj_frechet_with_sub.dat");
        cout << "traj size: " << traj.length <<endl;
        auto t1 = chrono::high_resolution_clock::now(); 

        pair<double, double> eps_cost = interleaved_matching(&after_graph, &traj, &grid);

        auto t2 = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

        cout<<"final eps: "<< eps_cost.first <<  " final path cost: "<< eps_cost.second << endl;
        cout << "Duration in milliseconds: " << duration << endl << endl;
        grid.curr_range = 0;

        t.cleanup_trajectory(&traj);
    }

    return 0;
}