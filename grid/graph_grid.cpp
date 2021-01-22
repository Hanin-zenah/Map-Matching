#include "graph_grid.h"
#include "graph.h"


// int cell_mapping(int row_id, int col_id, int num_rows, int num_cells){
    // int cell_index = num_rows * col_id + row_id;
    // return cell_index;
// }


void Build_grid::make_grids(Graph* graph, Grid* grid, double size){
    grid -> size = size;
    // initialize_cells(graph, grid, size);
    grid -> num_rows = ceil(graph -> max_lat/size);
    grid -> num_columns = ceil(graph -> max_long/size);
    int num_cells = grid -> num_rows * grid -> num_columns;

    if (grid -> num_columns == graph -> max_long/size){
		grid -> num_columns++;}

    if (grid -> num_rows == graph -> max_long/size){
	grid -> num_rows++;}

    grid -> cell_offset.resize(num_cells + 1);
    // cout<<"cell offset initialized and resized to: "<< grid -> cell_offset.size()<<endl;
    // for (int i = 1; i < grid -> cell_offset.size(); i++){
        // cell_offset[i] = 0;
    // }

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

        // cout<<"i: "<< i <<" graph -> nodes[i].id: "<< graph -> nodes[i].id <<" graph -> nodes[i].lat: "<< graph -> nodes[i].lat<<
        // " graph -> nodes[i].longitude: "<< graph -> nodes[i].longitude<<endl;
        // cout<<"row: "<<row<<" col: "<<col<<" index: "<<index<<endl;
// 
        // cout<<"pos: "<<pos<<" grid -> cell_nodes_list.size(): "<<grid -> cell_nodes_list.size()<<endl;

        grid -> cell_nodes_list[pos] = graph -> nodes[i].id;
        // cout<<"cell_nodes_list[pos]: "<<grid -> cell_nodes_list[pos]<<index<<endl;
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

