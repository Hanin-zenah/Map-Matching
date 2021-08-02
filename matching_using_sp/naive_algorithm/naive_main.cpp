#include "naive.h"

int main(int argc, char** argv) {
    if(argc < 4) {
        cerr << "Not enough arguments. Program accepts 3 command line arguments as follows: \
        \n\nExample: ./preprocess <preprocessed graph file path> <trajectory file path> <trajectory index>\n";
        return 1;
    }
    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[1], &after_graph); 
    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double lat_scale = after_graph.lat_scale;
    double lon_scale = after_graph.lon_scale;

    Build_grid builder;
    Grid grid = GRID_INIT;
    builder.make_grids(&after_graph, &grid, GRID_SIZE);

    Traj t;
    Traj_subsample ts;
    Trajectory traj = t.read_trajectory_k(argv[2], stoi(argv[3]), lon_min, lat_min, lat_scale, lon_scale);
    t.calc_traj_edge_cost(&traj);
    ts.subsample_traj(&traj, 15);

    cout << "traj size: " << traj.length <<endl;
    auto t1 = chrono::high_resolution_clock::now(); 

    pair<double, double> eps_cost = naive_matching(&after_graph, &traj, &grid);

    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    cout<<"final eps: "<< eps_cost.first <<  " final path cost: "<< eps_cost.second << endl;
    cout << "Duration in milliseconds: " << duration << endl << endl;
    grid.curr_range = 0;

    t.cleanup_trajectory(&traj);

    return 0;
}