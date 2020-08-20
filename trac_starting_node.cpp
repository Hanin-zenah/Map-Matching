#include "starting_node_look_up.h"
#include "disc_frechet_v2.h"

using namespace std;

//  box_height = g_dist1;
//  box_width = (g_dist2 + g_dist3) * 0.5;


// vector<int> Lookup::assign_grid(node nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale){
    // vector<int> grid;
    // int num_grid_h, num_grid_w;
    // num_grid_h = box_height / grid_h;
    // num_grid_w = box_width / grid_w;
    // grid.push_back(floor((nd.lat- lat_min_to_y) * x_scale/ num_grid_w));
    // grid.push_back(floor((nd.longitude - lon_min_to_x) * y_scale/ num_grid_h)); //the height
//    
    // return grid;
// }
// 
// vector<int> LookUp::nodes_in_same_grid(Graph* graph, node traj_nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale){
    // vector<int> nodes_same_grid;
    // vector<int> traj_nd_grid = assign_grid(traj_nd, grid_h, grid_w, box_height, box_width, x_scale, y_scale);
    // for (int i; i < graph -> nodes.size(); i++){
        // vector<int> nd_grid = assign_grid(graph -> nodes[i], grid_h, grid_w, box_height, box_width, x_scale, y_scale);
        // if (nd_grid[0] == traj_nd_grid[0] && nd_grid[1]  == traj_nd_grid[1]){
            // nodes_same_grid.push_back(graph -> nodes[i]);
        // }
    // }
    // return nodes_same_grid;
// }


double dist_from_T0(Point* traj_nd, node g_nd, double x_scale, double y_scale) {
    double dist; 
    dist = sqrt(pow((traj_nd -> latitude - g_nd.lat)*x_scale, 2) + 
                    pow((traj_nd -> longitude - g_nd.longitude)*y_scale, 2));
    return dist; 
}  

bool compare_dist(FSedge* sp1, FSedge* sp2){
    return sp1 -> botlneck_val > sp2 -> botlneck_val;
}

vector<FSedge*> SearchNodes(Graph* graph, Point* traj_nd, double radius, double x_scale, double y_scale) {
    vector<FSedge*> se_list;
    /* when building a FSgraph, only need to know the node id, and the initial bottle neck vel */
    for(int i = 0; i < graph -> nodes.size(); i++) {
        double dist = dist_from_T0(traj_nd, graph -> nodes[i], x_scale, y_scale);
        if(dist <= radius) {
            FSedge* se = (FSedge*) malloc(sizeof(FSedge));
            FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
            se -> src = NULL;
            start_nd -> vid = graph -> nodes[i].id;
            start_nd -> tid = 0;
            start_nd -> dist = dist;
            start_nd -> visited = false;
            se -> trg = start_nd;
            se -> botlneck_val = dist;
            se_list.push_back(se);
        }
    }
    sort(se_list.begin(), se_list.end(), compare_dist);
    return se_list;
}

// int main(int argc, char** argv) {
// 
    // if(argc < 2) {
        // cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        // return 1;
    // }
// 
    // read graph from given file
    // Graph graph = GRAPH_INIT;
    // read_file(argv[1], &graph);
// 
    // Graph traj = GRAPH_INIT;
    // read_file(argv[2], &traj);
    // traj.nodes[0].lat = 0.05;
    // traj.nodes[2].lat = 0.27;
// 
    // node traj_nd = traj.nodes[0];
// 
    // LookUp look;
    // vector<FSedge*> superlist = look.SearchNodes(&graph, traj_nd, 3);
    // FSedge* sp_edge = (FSedge*) malloc(sizeof(FSedge));
    // sp_edge = superlist[0];
    // cout<<sp_edge -> botlneck_val<<endl; 
// 
// 
    // return 0;
// }