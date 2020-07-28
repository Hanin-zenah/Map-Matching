/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph_scc_lola_v3.h"
#include <vector>  

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read file
    Graph graph = GRAPH_INIT; // graph initialization

    read_file(argv[1], &graph);


    vector<int> in_offset{0, 2, 3, 4, 6, 7};
    vector<int> out_offset{0, 1, 3, 4, 5, 7};
    vector<int> in_edge{1, 5, 0, 6, 2, 4, 3};
    vector<int> out_edge{0, 1, 2, 5, 3, 6, 4};




    // int backward_edge = bi_dir(&graph, 5, in_edge, in_offset);
    // cout<<"backward_edge: "<<backward_edge<<endl;

    
    

    subsampling(&graph, 100, in_edge, in_offset, out_edge, out_offset);

    // cout<<"out_edge: "<<endl;
    // for(int i = 0; i < out_edge.size(); i++) {
    // cout<< out_edge[i] << endl;}

    string file_name;

    cout << "Please provide file name to store the subsamplied graph in: ";
    cin >> file_name;

    subsampled_graph(&graph, file_name);
    return 0;
}