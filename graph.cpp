
using namespace std;


/*
    Graph class 
    this will store all the attributes of our graph (nodes, edges, )
*/

class Graph {
    public: 
    int edges;
    int nodes;

    // double* x; //malloc later, this will store the converted value for longitude and latitude 
    // double* y; 

    //functions go here 


};

//function to read the data from the txt files (can use parallelism to breakdown tasks -> faster performance)

// function to project and scale the data 

void read_file(const char* file_name) {
    //read file 
    ifstream file = open(file_name, ios::in);
    //read line by line and store points (after converting them to meters) + their attributes in a class or struct 
    if(!file) {
        return;
    }
    file.close();
}