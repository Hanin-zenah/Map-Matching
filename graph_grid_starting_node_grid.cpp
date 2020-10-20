#include "graph_grid_starting_node_grid.h"
#include "disc_frechet_v2.h"
#include "graph_grid.h"
#include <numeric> 

using namespace std;

double dist_from_T0(Point* traj_nd, node g_nd) {
    double dist; 
    dist = sqrt(pow((traj_nd -> latitude - g_nd.lat), 2) + 
                    pow((traj_nd -> longitude - g_nd.longitude), 2));
    return dist; 
}  

bool compare_dist(FSedge* sp1, FSedge* sp2) {
    return sp1 -> botlneck_val > sp2 -> botlneck_val;
}

// vector<int> dims(Graph* graph, Grid* grid, Point* traj_nd, double radius){
    // vector<int> dims;
    // int col = floor(traj_nd -> latitude/ grid -> size); 
    // int row = floor(traj_nd -> longitude/ grid -> size);
    // int index = grid -> columns * row + col;
    // int dim = grid -> curr_num_cells;
// }

// vector<int> index_list(Grid* grid){
    // int dim = sqrt(grid ->curr_num_cells);
    // int starting = grid -> target_cell_idx - (dim - 1)/2 * grid -> columns - (dim - 1)/2;
// 
    // vector<int> ilist;
// 
    // for (int i = 0; i < dim; i++){
        // int cell_ids[dim]; 
        // std::iota(cell_ids, cell_ids + dim, starting);
        
        // for (int j:cell_ids){
            // ilist.push_back(j);
        // }
    // }
// return ilist;
// }









bool available_nodes(Grid* grid, int col, int row, int range){
    vector<int> ids;
    int left = max(0, col - range);
    int right = min(grid -> num_columns - 1, col + range);
    int upper = max(0, row - range);
    int lower = min(grid -> num_rows - 1, row + range);
    int index = grid -> num_columns * row + col;
    int starting_idx, ending_idx;
    int num_columns = grid -> num_columns;
    int num_rows = grid -> num_rows;
    // cout<<"in available_nodes()  "<<left<<" "<<right<<" "<<upper<<" "<<lower<<endl;
  
    vector<int> cell_indices;
    /* horizontal top */
    starting_idx = num_columns * upper + left;
    ending_idx = num_columns * upper + right;
    // for (int i = starting_idx; i <= ending_idx +1; i++){
        // cell_indices.pushback(i);
    // }
    int pos1 = grid -> cell_offsets[starting_idx];
    int pos2 = grid -> cell_offsets[ending_idx + 1]; 
    if (pos2 > pos1){
        for (int k = pos1; k < pos2; k++){
                int nd_id = grid -> nodes_ids[k];

                ids.push_back(nd_id);
            }
            }

    /* horizontal bottom */
    starting_idx = num_columns * lower + left;
    ending_idx = num_columns * lower + right;
    int pos1 = grid -> cell_offsets[starting_idx];
    int pos2 = grid -> cell_offsets[ending_idx + 1]; 
    if (pos2 > pos1){
    for (int k = pos1; k < pos2; k++){
            int nd_id = grid -> nodes_ids[k];
            ids.push_back(nd_id);
        }
        }

    /* vertical left */
    for (int i = 1; i <= lower - upper - 1; i++){
    starting_idx = (num_columns * upper + i)  + left;
    cell_indices.pushback(starting_idx);
        for (int j = 0; j < cell_indices.size(); j++){
            int pos1 = grid -> cell_offsets[j];
            int pos2 = grid -> cell_offsets[j+ 1]; 
            if (pos2 > pos1){
                for (int k = pos1; k < pos2; k++){
                    int nd_id = grid -> nodes_ids[k];
                    ids.push_back(nd_id);
                }}}}
    
    /* vertical right */
    for (int i = 1; i <= lower - upper - 1; i++){
    ending_idx = (num_columns * upper + i)  + right;
    cell_indices.pushback(ending_idx);
    for (int j = 0; j < cell_indices.size(); j++){
        int pos1 = grid -> cell_offsets[j];
        int pos2 = grid -> cell_offsets[j+ 1]; 
        if (pos2 > pos1){
            for (int k = pos1; k < pos2; k++){
                int nd_id = grid -> nodes_ids[k];
                ids.push_back(nd_id);
            }}}}
    return ids;
}


// bool available_nodes(Grid* grid, int col, int row, int range){
    // int left = max(0, col - range);
    // int right = min(grid -> num_columns - 1, col + range);
    // int upper = max(0, row - range);
    // int lower = min(grid -> num_rows - 1, row + range);
    // int index = grid -> num_columns * row + col;
    // int starting_idx, ending_idx;
    // int num_columns = grid -> num_columns;
    // int num_rows = grid -> num_rows;
// 
//   
    // bool has_nodes = false;
    // for (int i = 0; i < min(2 * range + 1, num_rows - upper) ; i++){
        // starting_idx = num_columns * (upper + i) + left;
        // ending_idx = num_columns * (upper + i) + right;
        // 
        // 
        // int pos1 = grid -> cell_offsets[starting_idx];
        // int pos2 = grid -> cell_offsets[ending_idx + 1];
    //   
            // if (pos2 > pos1){
                // has_nodes = true;
                // break;
            // }
    // }
// return has_nodes;
// }

vector<int> add_range_to_Q(Grid* grid, int col, int row, int range){
    vector<int> ids;
    int left = max(0, col - range);
    int right = min(grid -> num_columns - 1, col + range);
    int upper = max(0, row - range);
    int lower = min(grid -> num_rows - 1, row + range);
    int num_columns = grid -> num_columns;
    int num_rows = grid -> num_rows;
    int starting_idx, ending_idx;
    cout<<"in get_nodes_ids()   "<<left<<" "<<right<<" "<<upper<<" "<<lower<<endl;
    
    // vector<int> cell_pos_list;
    for (int i = 0; i < min(2 * range + 1, num_rows - upper) ; i++){
        starting_idx = num_columns*(upper + i) + left;
        ending_idx = num_columns*(upper + i) + right;
        cout<<"iteration i: "<<i<<endl;
        cout<<"in_get_nodes_ids()   starting_idx, ending_idx: "<<starting_idx<<" "<<ending_idx<<endl;
        // for (int j = starting_idx; j <= ending_idx; j++){
        // grid -> cells[j] -> included = true;            
        int pos1 = grid -> cell_offsets[starting_idx];
        int pos2 = grid -> cell_offsets[ending_idx + 1]; 

        if (pos2 > pos1){
            for (int k = pos1; k < pos2; k++){
                // bool included = grid -> cells[grid -> cell_offsets[k]] -> included;
                // if (!included){
                    int nd_id = grid -> nodes_ids[k];
                    // cout<<"pushing back node id: "<<nd_id<<endl;
                    ids.push_back(nd_id);
                    // grid -> cells[grid -> cell_offsets[k]] -> included = true;
                // }
                }
                }
        }
        // }
    return ids;
}

vector<FSedge*> GridSearch(Graph* graph, Grid* grid, Point* traj_nd){
    
    cout<<"traj_nd -> latitude: "<<traj_nd -> latitude<<" traj_nd -> longitude: "<<traj_nd -> longitude<<" grid -> size "<<grid -> size<<endl;
    int col = floor(traj_nd -> latitude/ grid -> size); 
    int row = floor(traj_nd -> longitude/ grid -> size);
    int index = grid -> num_columns * row + col;

    cout<< "col: "<<col<<" row: "<<row<<" index: "<<index<<endl;
    cout<< " grid -> num_columns: "<< grid -> num_columns<<" grid -> num_rows: "<<grid -> num_rows<<endl;

    int range = 0;
    bool has_nodes = false;
    while (!has_nodes){
        cout<<"current range: "<<range<<endl;
    has_nodes = available_nodes(grid, col, row, range);
    range++;
    }

    grid -> curr_range = range;
    cout<<"settled range"<<endl;
    vector<int> nodes_idx_list = get_node_ids(grid, col, row, range);

    cout<<"finished indexing "<<endl;
    vector<FSedge*> se_list;
    for (int j = 0; j < nodes_idx_list.size(); j++){
        int node_id = nodes_idx_list[j];
        double dist = dist_from_T0(traj_nd, graph -> nodes[node_id]);
        FSedge* se = (FSedge*) malloc(sizeof(FSedge));
        FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
        se -> src = NULL;
        start_nd -> vid = node_id;
        start_nd -> tid = 0;
        start_nd -> dist = dist;
        start_nd -> visited = false;
        se -> trg = start_nd;
        se -> botlneck_val = dist;
        se_list.push_back(se);
    }
// 
    sort(se_list.begin(), se_list.end(), compare_dist);
// 
    grid -> dist_to_peak = se_list[0] ->trg -> dist;
// 
    return se_list;

    // return nodes_idx_list;
}


vector<FSedge*> ExtendGrid(Graph* graph, Grid* grid, Point* traj_nd, int col, int row){
    int range_old = grid -> curr_range;
    int range_new = range_old++;
    
    vector<int> nodes_idx_list = get_node_ids(grid, col, row, range_new);
    vector<FSedge*> se_list;
    for (int j = 0; j < nodes_idx_list.size(); j++){
        int node_id = nodes_idx_list[j];
        double dist = dist_from_T0(traj_nd, graph -> nodes[node_id]);
        FSedge* se = (FSedge*) malloc(sizeof(FSedge));
        FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
        se -> src = NULL;
        start_nd -> vid = node_id;
        start_nd -> tid = 0;
        start_nd -> dist = dist;
        start_nd -> visited = false;
        se -> trg = start_nd;
        se -> botlneck_val = dist;
        se_list.push_back(se);
    }
    sort(se_list.begin(), se_list.end(), compare_dist);
    grid -> dist_to_peak = se_list[0] ->trg -> dist;
    return se_list;
}
 
