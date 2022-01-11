#include "skip_algorithm.h"

HubLabelOffsetList hl;

std::ofstream log_output;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " graph_file trajectory_file [hub_label_file]\n";
        return 1;
    }

    char* graph_file = argv[1];
    char* trajectory_file = argv[2];
    char* hub_label_file = nullptr;

    if (argc >= 4) {
        hub_label_file = argv[3];
        use_hub_labels = true;
    }
    else {
        use_hub_labels = false;
    }

    if (hub_label_file != nullptr) {
        // hl.importHubLabelsBinary("saarland_x100_binary");
        hl.importHubLabelsBinary(hub_label_file);
    }

    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    // read_processed_graph("../example_files/saarland_all_preprocessed_cover_10.txt", &after_graph);

    cout << "Reading graph" << endl;
    // read_processed_graph("saarland_kskip.txt", &after_graph);
    read_processed_graph(graph_file, &after_graph);
    cout << "Finished reading graph" << endl;

    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double lat_scale = after_graph.lat_scale;
    double lon_scale = after_graph.lon_scale;

    Build_grid builder;
    Grid grid = GRID_INIT;
    builder.make_grids(&after_graph, &grid, GRID_SIZE);
    
    // vector<int> indices; 
    // indices.push_back(0);
    // indices.push_back(7);
    // indices.push_back(8);
    // indices.push_back(9);
    // indices.push_back(23);
    // indices.push_back(31);
    // indices.push_back(19);
    // indices.push_back(24);
    // indices.push_back(91);
    // indices.push_back(69);
    // indices.push_back(458);
    // indices.push_back(15);
    // indices.push_back(4);

    Traj t; 
    Traj_subsample ts;

    vector<Trajectory> trajectories = t.read_processed_trajectories(trajectory_file, 0);
    // vector<Trajectory> trajectories = t.read_processed_trajectories("saarland_trajectories_noise100.txt", 0);

    #ifdef LOG_OUTPUT
    log_output.open(argv[2]);
    #endif

    // for(int i = 0; i < indices.size(); i++) {
    for (int i=0; i<trajectories.size(); i++) {
        // Trajectory traj = t.read_trajectory_k("../example_files/saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", indices[i], lon_min, lat_min, lat_scale, lon_scale);
        Trajectory traj = trajectories[i];
        t.calc_traj_edge_cost(&traj);
        ts.subsample_traj(&traj, 15);
        // write_traj(&traj, "traj_frechet_with_sub.dat");

        cout << "traj " << i << " \tlength: " << traj.length << endl;
        auto t1 = chrono::high_resolution_clock::now(); 

        pair<double, double> eps_cost = match(&after_graph, &traj, &grid);

        auto t2 = chrono::high_resolution_clock::now();

        auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

        cout<<"final eps: "<< eps_cost.first <<  " final path cost: "<< eps_cost.second << endl;
        cout << "Duration in milliseconds: " << duration << endl << endl;
        grid.curr_range = 0;

        t.cleanup_trajectory(&traj);

        // break;
        
        #ifdef LOG_OUTPUT
        log_output << '\n';
        #endif
    }

    #ifdef LOG_OUTPUT
        log_output.close();
    #endif

    return 0;
}