#include "graph_grid.h"
#include "../preprocessing/graph.h"

void Build_grid::make_grids(Graph* graph, Grid* grid, double size){
    grid -> size = size;
    grid -> num_rows = ceil(graph -> max_lat/size);
    cout<<"graph -> max_lat: "<<graph -> max_lat<<endl;
    cout<<"graph -> max_long: "<<graph -> max_long<<endl;

    grid -> num_columns = ceil(graph -> max_long/size);
    int num_cells = grid -> num_rows * grid -> num_columns;
    grid -> max_range = max(grid -> num_columns , grid -> num_rows);

    if (grid -> num_columns == graph -> max_long/size){
		grid -> num_columns++;}

    if (grid -> num_rows == graph -> max_long/size){
	grid -> num_rows++;}

    grid -> cell_offset.resize(num_cells + 1);

    std::fill(grid -> cell_offset.begin(), grid -> cell_offset.end(), 0);
    grid -> cell_nodes_list.resize(graph -> nodes.size());
    
    for (int i = 0; i < graph -> nodes.size(); i++){
        int col = floor(graph -> nodes[i].longitude/size); // floor this -- cell id starts with zero
        int row = floor(graph -> nodes[i].lat/size);
        int index = grid -> num_columns * row + col;
        grid -> cell_offset[index + 1]++;
    }


    int sum = 0;
    for (int i = 1; i < grid -> cell_offset.size(); i++){
        grid -> cell_offset[i] += sum;
        sum = grid -> cell_offset[i];
    }


    for (int i = 0; i < graph -> nodes.size(); i++){
        int col = floor(graph -> nodes[i].longitude/size);
        int row = floor(graph -> nodes[i].lat/size);
        int index = grid -> num_columns * row + col;
        int pos = grid -> cell_offset[index];

        grid -> cell_nodes_list[pos] = graph -> nodes[i].id;
        grid -> cell_offset[index]++;  
    }

    /* reset cell offset */
    for (int i = grid -> cell_offset.size() - 1; i > 0; i--){
        grid -> cell_offset[i] = grid -> cell_offset[i - 1];
    }
    grid -> cell_offset[0] = 0;

    return;
}

void Build_grid::output_grid(Grid* grid, string file_name){
    ofstream txt_file(file_name);
    txt_file << grid -> cell_offset.size() << endl;
    txt_file << grid -> cell_nodes_list.size() << endl;

    for(int i = 0; i < grid -> cell_offset.size(); i++) {
        txt_file << i <<" "<<grid -> cell_offset[i] << endl;
    }

    for(int i = 0; i < grid -> cell_nodes_list.size(); i++) {
        txt_file << grid -> cell_nodes_list[i] << endl;
    }

    txt_file.close();
    return;
}

