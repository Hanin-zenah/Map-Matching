#include <iostream> 
#include <fstream> 
using namespace std;

class Graph {
    public: 
    int edges;
    int nodes;

    double* x; //malloc later, this will store the converted value for longitude and latitude 
    double* y; 


}

// class Geeks 
// { 
//     public: 
//     string geekname; 
//     int id; 
      
//     // printname is not defined inside class definition 
//     void printname(); 
      
//     // printid is defined inside class definition 
//     void printid() 
//     { 
//         cout << "Geek id is: " << id; 
//     } 
// };

void read_file(cont char* file_name) {
    //read file 
    ifstream file = open(file_name, ios::in);
    //read line by line and store points (after converting them to meters) + their attributes in a class or struct 

}