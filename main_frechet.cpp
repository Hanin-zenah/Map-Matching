/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "graph_subsampling.h"
#include "disc_frechet_v2.h"
#include "trajectory.h"
#include "starting_node_look_up.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read graph from given file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);
    /* bounding box */
    double lat_min = graph.min_lat;
    double lat_max = graph.max_lat;
    double lon_min = graph.min_long;
    double lon_max = graph.max_long;
    cout << "here\n";

    // read processed graph from a given file
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[2], &after_graph);
    cout << "hello\n";

    vector<Trajectory> trajs = read_trajectories("saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 1, lon_min, lat_min);
    cout << "finished extracting the trajectory\n";
    cout << trajs.size() << endl;
    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];

    write_traj(&traj, "traj_frechet.dat");

    FSgraph fsgraph = FSGRAPH_INIT;
    cout<<min_eps(&after_graph, &traj, &fsgraph, 80, after_graph.x_scale, after_graph.y_scale)<<endl;
    write_fsgraph(&fsgraph, "fsgraph.dat");
    write_sur_graph(&fsgraph, &after_graph, "sur_graph_frechet.dat");
    cleanup(&fsgraph);
    cleanup_trajectory(&traj);
    return 0;
}