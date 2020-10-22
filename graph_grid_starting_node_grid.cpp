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

bool compare_node_dist(FSnode* nd1, FSnode* nd2) {
    return nd1 -> dist > nd2 -> dist;   // bigger or smaller?
}

// vector<int> initial_range(Grid* grid, int col, int row){
    // vector<int> ids;
    // int range = 1;
    // int left = max(0, col - range);
    // int right = min(grid -> num_columns - 1, col + range);
    // int upper = max(0, row - range);
    // int lower = min(grid -> num_rows - 1, row + range);
    // int num_columns = grid -> num_columns;
    // int num_rows = grid -> num_rows;
    // int starting_idx, ending_idx;
    // cout<<"in get_nodes_ids()   "<<left<<" "<<right<<" "<<upper<<" "<<lower<<endl;
    // 
    // for (int i = 0; i < min(3, num_rows - upper) ; i++){
        // starting_idx = num_columns*(upper + i) + left;
        // ending_idx = num_columns*(upper + i) + right;
        // int pos1 = grid -> cell_offsets[starting_idx];
        // int pos2 = grid -> cell_offsets[ending_idx + 1]; 
        // if (pos2 > pos1){
            // for (int k = pos1; k < pos2; k++){
                    // int nd_id = grid -> nodes_ids[k];
                    // ids.push_back(nd_id);
                // }}
    // }
    // return ids;
// }


vector<int> add_range_to_Q(Grid* grid, int col, int row, int range){
    vector<int> nodes_list;
    int left = max(0, col - range);
    int right = min(grid -> num_columns - 1, col + range);
    int upper = max(0, row - range);
    int lower = min(grid -> num_rows - 1, row + range);

    int starting_idx, ending_idx;
    int num_columns = grid -> num_columns;
    int num_rows = grid -> num_rows;
    // cout<<"add_range_to_Q  "<<left<<" "<<right<<" "<<upper<<" "<<lower<<endl;
  
    /* horizontal top */
    starting_idx = num_columns * upper + left;
    ending_idx = num_columns * upper + right;
    // vector<int> cell_indices;

    // for (int i = starting_idx; i <= ending_idx +1; i++){
        // cell_indices.pushback(i);
    // }
    int pos1 = grid -> cell_offsets[starting_idx];
    int pos2 = grid -> cell_offsets[ending_idx + 1]; 
    if (pos2 > pos1){
        for (int k = pos1; k < pos2; k++){
                int nd_id = grid -> nodes_ids[k];

                nodes_list.push_back(nd_id);
            }}

    /* horizontal bottom */
    if(range > 0){
    starting_idx = num_columns * lower + left;
    ending_idx = num_columns * lower + right;
    int pos1 = grid -> cell_offsets[starting_idx];
    int pos2 = grid -> cell_offsets[ending_idx + 1]; 
    if (pos2 > pos1){
    for (int k = pos1; k < pos2; k++){
            int nd_id = grid -> nodes_ids[k];
            nodes_list.push_back(nd_id);
        }}

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
                    nodes_list.push_back(nd_id);
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
                nodes_list.push_back(nd_id);
            }}}}
    }
    return nodes_list;
}

bool range_check(Grid* grid, Point* traj_nd, double dist_peak, int range){
    int col = floor(traj_nd -> longitude/ grid -> size);
    int row = floor(traj_nd -> latitude/ grid -> size);
    double T0_x = traj_nd -> longitude;
    double T0_y = traj_nd -> latitude;

    bool within_range = true;

    double left_bd = max(0, T0_x - dist_peak);
    int left = max(0, col - range);

    double right_bd = min(grid -> num_columns - 1, T0_x + dist_peak);
    int right = min(grid -> num_columns - 1, col + range);

    double upper_bd = max(0, T0_y - dist_peak);
    int upper = max(0, row - range);
    
    double lower_bd = min(grid -> num_rows - 1, T0_y + dist_peak);
    int lower = min(grid -> num_rows - 1, row + range);

    if (floor(left_bd/grid -> size ) <= left || floor(right_bd/grid -> size ) >= right 
    || floor(upper_bd/grid -> size ) <= upper || floor(lower_bd/grid -> size ) >= lower){ 
    within_range = false;
    }
    return within_range;
}

vector<FSnode*> GridSearch(Graph* graph, Grid* grid, Point* traj_nd){
    int col = floor(traj_nd -> longitude/ grid -> size);
    int row = floor(traj_nd -> latitude/ grid -> size);
    int index = grid -> num_columns * row + col;
    
    int range = 0;

    vector<int> nodes_in_init_range = initial_range(grid, col, row);

    if (!nodes_in_init_range.empty()){
        //create fs nodes;
    }
    
    else{
        int range = 2;
        vector<int> nodes_idx_list;
        while(nodes_idx_list.empty()){
        nodes_idx_list = add_range_to_Q(grid, col, row, range);
        range++;
        }}
        
    vector<FSnode*> nodes_list;
    for (int j = 0; j < nodes_list.size(); j++){
        int node_id = nodes_list[j];
        double dist = dist_from_T0(traj_nd, graph -> nodes[node_id]);
        FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
        start_nd -> vid = node_id;
        start_nd -> tid = 0;
        start_nd -> dist = dist;
        start_nd -> visited = false;
        nodes_list.push_back(start_nd);
    }
 
    sort(nodes_list.begin(), nodes_list.end(), compare_node_dist);
 
    grid -> dist_to_peak = nodes_list[0] -> dist; 

    bool enough_range = false;
    while (!enough_range){
    enough_range = range_check(grid, Point* traj_nd, grid -> dist_to_peak, range);
    range++;
    }

    return nodes_list;
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
 
