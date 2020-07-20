/* here goes the main function which will call all the necessary function to preprocess the graph */
#include "graph_scc_lola.h" 
//#include subsampling_v2.cpp

int main(int argc, char** argv) {

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    //read file
    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);

    // double lat_min = graph.min_lat;
    // double lat_max = graph.max_lat;
    // double lon_min = graph.min_long;
    // double lon_max = graph.max_long;


    //offset array
    
    // string file_name1;

    
    

    // cout << "\nPlease provide another file name to store the indegree offset array in: ";
    // cin >> file_name1;

    // outdeg_offset_array(&graph, file_name);
    //indeg_offset_array(&graph, file_name1);

    //strongly connectedd componetns

    string file_name;

    cout << "Please provide file name to store the outdegree offset array in: ";
    cin >> file_name;

    subsampled_array(&graph, file_name);

    return 0;
}