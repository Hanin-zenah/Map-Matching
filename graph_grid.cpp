#include "graph_grid.h"
#include "graph.h"


// int cell_mapping(int row_id, int col_id, int num_rows, int num_cells){
    // int cell_index = num_rows * col_id + row_id;
    // return cell_index;
// }

// void initialize_cells(Graph* graph, Grid* grid, double size){
    // grid -> num_rows = ceil(graph -> max_lat/size);
    // grid -> num_columns = ceil(graph -> max_long/size);
    // for (int i = 0; i < grid -> num_rows * grid -> num_columns; i++){
        // Cell* c = (Cell*) malloc(sizeof(Cell));
        // c -> nodes_count = 0;
        // grid -> cells.push_back(c);
    // }
    // return;
// }

void make_grids(Graph* graph, Grid* grid, double size){
    grid -> size = size;
    // initialize_cells(graph, grid, size);
    grid -> num_rows = ceil(graph -> max_lat/size);
    grid -> num_columns = ceil(graph -> max_long/size);
    int num_cells = grid -> num_rows * grid -> num_columns;

    grid -> cell_offset.resize(num_cells + 1);
    // for (int i = 1; i < grid -> cell_offset.size(); i++){
        // cell_offset[i] = 0;
    // }
    std::fill(grid -> cell_offset.begin(), grid -> cell_offset.end(), 0);
    grid -> cell_nodes_list.resize(graph -> nodes.size());
    
    for (int i = 0; i < graph -> nodes.size(); i++){
        int col = floor(graph -> nodes[i].longitude/size); // floor this, cuz cell id starts with zero!
        int row = floor(graph -> nodes[i].lat/size);
        int index = grid -> num_columns * row + col;
        // cout<<"i: "<< i <<" graph -> nodes[i].id: "<< graph -> nodes[i].id <<" graph -> nodes[i].lat: "<< graph -> nodes[i].lat<<endl;
        // cout<<"row: "<<row<<" col: "<<col<<" index: "<<index<<endl;
        // grid.cells[index] -> nodes_count++;
        grid -> cell_offset[index]++;
        // cout<<" grid -> cells[index]->nodes_count: "<<grid -> cells[index]->nodes_count<<endl;
    }

    int sum = 0;
    for (int i = 1; i < grid -> cell_offset.size(); i++){
        grid -> cell_offset[i] += sum;
        sum += grid -> cell_offset[i];
    }

    // grid -> cell_offsets.push_back(0);
    // int sum = 0;
    // for (int i = 0; i < grid -> cells.size(); i++){
        // sum += grid -> cells[i] -> nodes_count;
        // grid -> cell_offsets.push_back(sum);
    // }
    // cout<<"grid -> cell_offsets: "<<grid -> cell_offsets.size()<<endl;

    // make_nodes_ids_vec(graph, grid, size);

     //initialize this somewhere else?

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

void output_grid(Grid* grid, string file_name){
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

