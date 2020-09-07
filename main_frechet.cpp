/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "graph_subsampling.h"
#include "disc_frechet_v2.h"
#include "trajectory.h"
#include "trajectory_split.h"
#include "starting_node_look_up.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    // read graph from given file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);
    
    // /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;
    read_processed_graph(argv[1], &after_graph);
    write_graph(&after_graph, "saarland_all_with_sub.dat");

    cout<<"in saarland_all_with_sub.dat"<<endl;

    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double x_scale = after_graph.x_scale;
    double y_scale = after_graph.y_scale;
    cout<<"in the after graph: "<<lat_min<<" "<<lon_min<<" "<<x_scale<<" "<<y_scale<<endl;
 
    vector<Trajectory> trajs = read_trajectories("trajectories/saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 1, lon_min, lat_min);
    Trajectory traj = trajs[0];
    Point* traj_nd = traj.points[0];
    
    cout << "finished extracting the trajectory\n";
    calc_traj_edge_cost(&traj, x_scale, y_scale);
    cout<<traj_nd->latitude<<traj_nd->longitude<<endl;

    subsample_traj(&traj, 15);
    write_traj(&traj, "traj_frechet_test3.dat");
    cout << "finished subsampling the trajectory\n";

    //QH: make a prompt for the radius???
    FSgraph fsgraph = FSGRAPH_INIT; 
    // vector<FSedge*> nodes_within_dist = SearchNodes(&after_graph, traj_nd, 80, x_scale, y_scale);//&SCC_graph
    // cout<<"number nearest nodes: "<<nodes_within_dist.size()<<endl;

    // cout<<min_eps(&after_graph, &traj, &fsgraph, 40, x_scale, y_scale)<<endl;
    min_eps(&after_graph, &traj, &fsgraph, 40, x_scale, y_scale);
    write_fsgraph(&fsgraph, "fsgraph.dat");
    write_sur_graph(&fsgraph, &after_graph, "sur_graph_frechet.dat");
    stack<FSnode*> path = get_path(&fsgraph);
    cout<<"finished printing path"<<endl;
    print_path(&fsgraph, &traj, &after_graph, "frechet_path.dat");
    cout<<"finished writing out path"<<endl;
    
    //run dijkstra on the freespace 
    
    cleanup(&fsgraph);
    cleanup_trajectory(&traj);
    return 0;
}