#include "graph_grid.h"
#include "graph.h"


// int cell_mapping(int row_id, int col_id, int num_rows, int num_cells){
    // int cell_index = num_rows * col_id + row_id;
    // return cell_index;
// }

void initialize_cells(Graph* graph, Grid* grid, double size){
    
    grid -> num_rows = ceil(graph -> max_long/size);
    grid -> num_columns = ceil(graph -> max_lat/size);
    for (int i = 0; i < grid -> num_rows * grid -> num_columns; i++){
        Cell* c = (Cell*) malloc(sizeof(Cell));
        c -> cell_id = i;
        c -> nodes_count = 0;
        c -> included = false;
        grid -> cells.push_back(c);
    }
    return;
}

void make_grids(Graph* graph, Grid* grid, double size){
    grid -> size = size;
    initialize_cells(graph, grid, size);
    
    for (int i = 0; i < graph -> nodes.size(); i++){
        int col = floor(graph -> nodes[i].lat/size); // floor this, cuz cell id starts with zero!
        int row = floor(graph -> nodes[i].longitude/size);
        int index = grid -> num_columns * row + col;
        // cout<<"i: "<< i <<" graph -> nodes[i].id: "<< graph -> nodes[i].id <<" graph -> nodes[i].lat: "<< graph -> nodes[i].lat<<endl;
        // cout<<"row: "<<row<<" col: "<<col<<" index: "<<index<<endl;
        grid -> cells[index] -> nodes_count++;
        // cout<<" grid -> cells[index]->nodes_count: "<<grid -> cells[index]->nodes_count<<endl;
    }
    grid -> cell_offsets.push_back(0);
    int sum = 0;
    for (int i = 0; i < grid -> cells.size(); i++){
        sum += grid -> cells[i] -> nodes_count;
        grid -> cell_offsets.push_back(sum);
    }
    cout<<"grid -> cell_offsets: "<<grid -> cell_offsets.size()<<endl;

    // make_nodes_ids_vec(graph, grid, size);

    grid -> nodes_ids.resize(graph -> nodes.size()); //initialize this somewhere else?


    for (int i = 0; i < graph -> nodes.size(); i++){
    int col = floor(graph -> nodes[i].lat/size);
    int row = floor(graph -> nodes[i].longitude/size);
    int index = grid -> num_columns * row + col;
    int pos = grid -> cell_offsets[index];

    // int pos2 = grid -> cell_offsets[index + 1];
// 
    // auto it = grid -> nodes_ids.begin();
    // grid -> nodes_ids.insert(it + pos1, graph -> nodes[i].id);
    // it = grid -> nodes_ids.begin();
    // grid -> nodes_ids.erase(it + pos2);

    grid -> nodes_ids[pos] = graph -> nodes[i].id;
    grid -> cell_offsets[index]++;  

    // cout<<"i: "<<i<<" col: "<<col<<" row: "<<row<<" index: "<<index << " pos: "
    // <<pos<<" graph -> nodes[i].id "<<graph -> nodes[i].id<<" grid -> nodes_ids[pos] "<<grid -> nodes_ids[pos]<<endl;
    }

    /* reset cell offset */
    for (int i = grid -> cell_offsets.size() - 1; i > 0; i--){
        grid -> cell_offsets[i] = grid -> cell_offsets[i - 1];
    }
    grid -> cell_offsets[0] = 0;

    return;
}

void output_grid(Grid* grid, string file_name){
    ofstream txt_file(file_name);

    for(int i = 0; i < grid -> nodes_ids.size(); i++) {
        txt_file << grid -> nodes_ids[i] << endl;
    }

    txt_file.close();
    return;
}

