#include <iostream> 
#include <fstream> 
#include <string>
#include <algorithm>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Not enough arguments provided." << std::endl;
        return 0;
    }

    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];

    std::ifstream input_file; 
    input_file.open(input_file_name);
    if(!input_file) {
        std::cerr << "Unable to open file." << std::endl;
        return 0;
    }

    std::ofstream output_file;
    output_file.open(output_file_name);
    if(!input_file) {
        std::cerr << "Unable to open file." << std::endl;
        return 0;
    }
    
    std::string buffer;

    int n_vertices;
    int n_edges;
    input_file >> n_vertices >> n_edges;
    std::getline(input_file, buffer);

    // Copy vertex and edge numbers
    output_file << "p sp " << n_vertices << " " << n_edges << '\n';

    // Skip latitude and longitude metadata
    for (int i=0; i<6; i++)
    {
        std::getline(input_file, buffer);
    }

    // Skip vertex definitions
    for (int i=0; i<n_vertices; i++)
    {
        std::getline(input_file, buffer);
    }

    

    int max_edge_weight = -1;

    // Copy edge definitions
    for (int i=0; i<n_edges; i++)
    {
        int edge_start;
        int edge_end;
        float edge_weight;

        input_file >> edge_start >> edge_end >> edge_weight;
        if (edge_weight <= 0 || (int)(edge_weight+1) <= 0)
        {
            // std::cerr << "Edge weights must be strictly positive." << edge_weight << std::endl;
            // return 0;
        }
        std::getline(input_file, buffer);

        
        if (edge_end == 578973)
        {
            std::cout << "start: " << edge_start << std::endl;
        }

        int new_edge_weight = (int)(edge_weight * 1000);
        max_edge_weight = std::max(new_edge_weight, max_edge_weight);
        // int new_edge_weight = std::max((int)(edge_weight * 100), 1);
        if (new_edge_weight == 0)
        {
            std::cout << "ZERO EDGE WEIGHT from " << edge_weight << std::endl;
        }
        output_file << "a " << edge_start+1 << " " << edge_end+1 << " " << new_edge_weight << '\n';
    }

    std::cout << "Max edge weight: " << max_edge_weight << std::endl;

    input_file.close();
    output_file.close();

    return 0;
}