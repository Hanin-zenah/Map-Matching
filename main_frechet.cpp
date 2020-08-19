/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "disc_frechet_v2.h"
#include "starting_node_look_up.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read graph from given file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);
    // write_graph(&graph, "graph_frechet.dat");


    Graph traj = GRAPH_INIT;
    read_file(argv[2], &traj);
    traj.nodes[0].lat = 0.05;
    traj.nodes[2].lat = 0.27;
    // write_graph(&traj, "traj_frechet.dat");

    struct node traj_nd = traj.nodes[0];
    // vector<FSedge*> superlist = SearchNodes(&graph, traj_nd, 3);
    // FSedge* sp_edge = superlist[0];
    // cout<<sp_edge -> botlneck_val<<endl; 
// 
    FSgraph fsgraph = FSGRAPH_INIT;
    cout<<min_eps(&graph, &traj, &fsgraph, 0.01)<<endl;
    write_fsgraph(&fsgraph, "fsgraph.dat");
// 
    cleanup(&fsgraph);
// 

    return 0;
}