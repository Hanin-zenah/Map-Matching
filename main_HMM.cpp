#include "preprocessing/graph.h"
#include "Stats/graph_for_hist.h"
#include "grid/graph_grid.h"
#include "grid/graph_grid_starting_node.h"
#include "preprocessing/graph_subsampling.h"
#include "DF_sol/disc_frechet.h"
#include "trajectories/trajectory.h"
#include "trajectories/trajectory_split.h"
#include "HMM/HMM.h"
// #include "freespace_shortest_path.h"
#include <chrono>


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

    double grid_size = 500.00;
    
    Build_grid build_grid;
    build_grid.make_grids(&after_graph, &grid, grid_size);

    auto elapsed_grid = std::chrono::high_resolution_clock::now() - start_grid;
    long long microseconds_grid = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_grid).count();

    cout<<"Building Grid Duration in microseconds: " << microseconds_grid << endl;


    vector<Trajectory> trajs = read_trajectories(argv[2], 5, lon_min, lat_min, lat_scale, lon_scale);
    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];

    /* london-geq50m-clean-unmerged-2016-10-09-greater-london.binTracks */
    /* saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks */

    cout << "finished extracting the trajectory\n";
    calc_traj_edge_cost(&traj);
    double traj_length = calc_traj_length(&traj);
 
    subsample_traj(&traj, 50);

    write_traj(&traj, "trajactory.dat");    

    string method = argv[3];
    if (method == "HMM"){

        HMM hmm;

        std::string sigma_str = argv[4];
        double sigma = std::stod(sigma_str);
        // double sigma = hmm.sigma_est(&after_graph, &grid, &traj); // this can be the default value if the input is missing

        std::string beta_str = argv[5];
        double beta = std::stod(beta_str);
        // double beta = hmm.beta_est(0.5, 100, 30); // this can be the default value if the input is missing

        std::string radius_str = argv[6];
        double radius = std::stod(radius_str);
        // double radius = 500.00;

        std::string num_candidate_str = argv[7];
        int num_candidate = std::stoi(num_candidate_str);
        // int num_candidate = 50;

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

        /* try to fit the HMM results into a freespace and see what's the frechet distance between this HMM results and traj */

        Graph HMM_graph = GRAPH_INIT;

        hmm.make_a_HMM_graph(&after_graph, com_path, &HMM_graph);

        outedge_offset_array(&HMM_graph);
        inedge_offset_array(&HMM_graph);

        cout<<"convert the path to graph\n";

        hmm.write_HMM_graph(&after_graph, com_path, argv[8]);

        FSgraph fsgraph = FSGRAPH_INIT; 
        FSpair last_pair = min_eps(&HMM_graph, &traj, &fsgraph, 750.00);
        cout<<"final fsgraph.eps: "<<fsgraph.eps<<endl;
// 
        cleanup(&fsgraph);
        cleanup_trajectory(&traj);

    }
    return 0;
}

