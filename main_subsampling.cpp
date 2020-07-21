/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph_scc_lola.h" 
//#include subsampling_v2.cpp

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read file
    Graph graph = GRAPH_INIT; // graph initialization
    read_file(argv[1], &graph);

    string file_name;

    cout << "Please provide file name to store the subsamplied graph in: ";
    cin >> file_name;

    subsampled_array(&graph, file_name);

    return 0;
}