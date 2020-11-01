#include "graph_grid_starting_node.h"
#include "graph_grid.h"
#include <numeric> 

using namespace std;

double dist_from_T0(Point* traj_nd, node g_nd) {
    double dist; 
    dist = sqrt(pow((traj_nd -> latitude - g_nd.lat), 2) + 
                    pow((traj_nd -> longitude - g_nd.longitude), 2));
    return dist; 
}  

void add_range_to_Q(Grid* grid, Graph* graph, int col, int row, int range, Point* traj_nd, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& PQ){

    // TODO: this is wrong when a cell on the boundary was already added to the PQ
    // Needs to be done separately per loop: hor-top, hor-bot,...
    int left  = max(0, col - range);
    int right = min(grid -> num_columns - 1, col + range);
    int upper = min(grid -> num_rows - 1, row + range);
    int lower = max(0, row - range);

    int starting_idx, ending_idx;
    int num_columns = grid -> num_columns;
    int num_rows    = grid -> num_rows;
//   cout<<"starting to add nodes\n";

   if (grid -> num_rows - 1 >= row + range){
    //    cout<<"entered horizontal top: \n";
        /* horizontal top */
        starting_idx = num_columns * upper + left;
        ending_idx   = num_columns * upper + right;

        int pos1 = grid -> cell_offset[starting_idx];
        int pos2 = grid -> cell_offset[ending_idx + 1]; 
        for (int k = pos1; k < pos2; k++){
                int nd_id = grid -> cell_nodes_list[k];
                Gpair grid_nd;
                grid_nd.first  = nd_id;
                grid_nd.second = dist_from_T0(traj_nd, graph -> nodes[nd_id]);
                PQ.push(grid_nd);
            }
   }
//    cout<<"horizontal top fine\n";
    
    if(upper > lower){
    /* horizontal bottom */
        if (row >= range){
            // cout<<"entered horizontal bottom: \n";
            starting_idx = num_columns * lower + left;
            ending_idx   = num_columns * lower + right;

            int pos1 = grid -> cell_offset[starting_idx];
            int pos2 = grid -> cell_offset[ending_idx + 1]; 
            for (int k = pos1; k < pos2; k++){
                int nd_id = grid -> cell_nodes_list[k];
                Gpair grid_nd;
                grid_nd.first  = nd_id;
                grid_nd.second = dist_from_T0(traj_nd, graph -> nodes[nd_id]);
                PQ.push(grid_nd);
                }
        }
        //  cout<<"horizontal bottom fine\n";

    int index_i;
    if (col >= range){
    /* vertical left */   
        for (int i =  row - range + 1; i <=  min(row + range - 1, num_rows - 1); i++){
            // cout<<"current range: "<<range<<"  i:  "<<i<<endl;
            int a = row - range + 1;
            int b = row + range - 1;
            // cout<<" for vertical left ---- row - range + 1: "<< a << " row + range - 1: " <<b<<endl;
            index_i = (num_columns * i)  + left;
            int pos1 = grid -> cell_offset[index_i];
            int pos2 = grid -> cell_offset[index_i + 1]; 
            // cout<<"index_i: "<<index_i<<" pos1: "<<pos1<<" pos2: "<<pos2<<endl;
                for (int k = pos1; k < pos2; k++){
                    int nd_id = grid -> cell_nodes_list[k];
                    Gpair grid_nd;
                    grid_nd.first = nd_id;
                    grid_nd.second = dist_from_T0(traj_nd, graph -> nodes[nd_id]);
                    PQ.push(grid_nd);
            }}
        }
        //  cout<<"vertical left fine\n";

        if (grid -> num_columns - 1 >= col + range){
            // cout<<"grid -> num_columns - 1 >= col + range is okay\n";
            // cout<<"entered vertical right: \n";
            /* vertical right */
            int a = row - range + 1;
            int b = row + range - 1;
            // cout<<" for vertical right ---- row - range + 1: "<< a << " row + range - 1: " <<b<<endl;
            for (int i =  max(row - range + 1, 0); i <=  row + range - 1; i++){
                index_i = (num_columns * i)  + right;
                int pos1 = grid -> cell_offset[i];
                int pos2 = grid -> cell_offset[i+ 1]; 
                    for (int k = pos1; k < pos2; k++){
                        int nd_id = grid -> cell_nodes_list[k];
                        Gpair grid_nd;
                        grid_nd.first = nd_id;
                        grid_nd.second = dist_from_T0(traj_nd, graph -> nodes[nd_id]);
                        PQ.push(grid_nd);

                }}
        }
        // cout<<"vertical right fine\n";
            }
            
    return;
}

bool range_check(Grid* grid, Point* traj_nd, double dist_peak, int range, double graph_max_x, double graph_max_y){
    int col = floor(traj_nd -> longitude/ grid -> size);
    int row = floor(traj_nd -> latitude/ grid -> size);
    double T0_x = traj_nd -> longitude;
    double T0_y = traj_nd -> latitude;

    bool within_range = true;

    double left_limit = max(0.00, T0_x - dist_peak);
    int left = max(0, col - range);
    double left_bd = left * grid -> size;

    double right_limit = min(graph_max_x, T0_x + dist_peak);
    int right = min(grid -> num_columns - 1, col + range);
    double right_bd = right * grid -> size;

    double upper_limit = min(graph_max_y, T0_y + dist_peak);
    int upper = min(grid -> num_rows - 1, row + range);
    double upper_bd = upper * grid -> size;
    
    double lower_limit = max(0.0, T0_y - dist_peak);
    int lower = max(0, row - range);
    double lower_bd = lower * grid -> size;

    if (left_limit <= left_bd || right_limit >= right_bd || upper_limit >= upper_bd || lower_limit <= lower_bd){ 
    within_range = false;
    }
    return within_range;
}

priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> GridSearch(Graph* graph, Grid* grid, Point* traj_nd){
    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>  closer_dist;
    int col = floor(traj_nd -> longitude/ grid -> size);
    int row = floor(traj_nd -> latitude/ grid -> size);
    int index = grid -> num_columns * row + col;

    cout<<"grid number of columns: "<<grid->num_columns<<" number rows: "<<grid->num_rows<<endl;
    cout<<"search query point   column: "<<col<<" row: "<<row<<" index: "<<index<<endl;

    double graph_max_x = graph -> max_long;
    double graph_max_y = graph -> max_lat;
    
    int range = 0;

    while(closer_dist.empty()){
    add_range_to_Q(grid, graph, col, row, range, traj_nd, closer_dist);
    range++; // is this the correct place to calculate this?
    }
    add_range_to_Q(grid, graph, col, row, range, traj_nd, closer_dist);

    cout<<" current nodes_idx_list size before range check: "<<closer_dist.size()<<endl;

    grid -> dist_to_peak = closer_dist.top().second; 
    grid -> curr_range   = range;

    bool enough_range = range_check(grid, traj_nd, grid -> dist_to_peak, grid -> curr_range, graph_max_x,graph_max_y);
    if (!enough_range) {
        cout<<"adding another layer of cells!\n";
        grid -> curr_range++;
        add_range_to_Q(grid, graph, col, row, grid -> curr_range, traj_nd, closer_dist);
        return closer_dist;
    }
    else{
        return closer_dist;
    }
}


void ExtendGrid(Graph* graph, Grid* grid, Point* traj_nd, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& PQ){ //while loop this in discret frechet
    int col = floor(traj_nd -> longitude/ grid -> size);
    int row = floor(traj_nd -> latitude/ grid -> size);

    while(PQ.empty()){
        grid -> curr_range++;
        add_range_to_Q(grid, graph, col, row, grid -> curr_range, traj_nd, PQ);
    }
    bool enough_range = range_check(grid, traj_nd, grid -> dist_to_peak, grid -> curr_range, graph -> max_long, graph -> max_lat);
    if (!enough_range) {
        cout<<"adding another layer of cells!\n";
        grid -> curr_range++;
        add_range_to_Q(grid, graph, col, row, grid -> curr_range, traj_nd, PQ);
}
    return;
}



//  
// int next_closest_node(vector<FSnode*> SN_PQ){
    // return SN_PQ[0] -> vid;
// }
// 
// vector<int> nodes_in_range(vector<FSnode*> SN_PQ, double radius){
    // vector<int> nd_list;
    // for (int i = 0; i < SN_PQ.size(); i++){
        // if (SN_PQ[i] -> dist <= radius){
            // nd_list.push_back(SN_PQ[i] -> vid);
        // }
        // else{
            // break;
        // }
    // }
    // return nd_list;
// }

// vector<FSedge*> convert_to_se(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> PQ, double radius) {
    // vector<FSedge*> se_list;
    // /* when building a FSgraph, only need to know the node id, and the initial bottle neck val */
    // for(int i = 0; i < PQ.size(); i++) {
        // Gpair gp = PQ.top();
        // double dist = gp.second;
        // PQ.pop();
        // if(dist <= radius) {
            // FSedge* se = (FSedge*) malloc(sizeof(FSedge));
            // FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
            // se -> src = NULL;
            // start_nd -> vid = gp.first;
            // start_nd -> tid = 0;
            // start_nd -> dist = dist;
            // start_nd -> visited = false;
            // se -> trg = start_nd;
            // se -> botlneck_val = dist;
            // se_list.push_back(se);
        // }
    // }
    // return se_list;
// }