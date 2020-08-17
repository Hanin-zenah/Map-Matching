/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph.h" 
#include "graph_subsampling.h"
#include "scale_projection.h"
#include "graph_subsampling.h"
#include "disc_frechet_v2.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read graph from given file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);


    Graph traj = GRAPH_INIT;
    read_file(argv[2], &traj);
    traj.nodes[0].longitude = 20;
    traj.nodes[2].lat = 16;


    // for (int i; i < graph.nodes.size();i++){
        // cout<<"graph.nodes[i].id: "<<graph.nodes[i].id<<endl;
    // }
  
    FSgraph fsgraph = FSGRAPH_INIT;
    cout<<min_eps(&graph, &traj, &fsgraph);

    cleanup(&fsgraph);


    return 0;
}