
#include <cmath>
#include "graph.h" // a copy of graph.h with change of nodes struct
#include "scale_projection.h"
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


double LookUp::dist_from_T0(node traj_nd, node g_nd) {
    double dist; 
    dist = sqrt(pow((traj_nd.lat - g_nd.lat), 2) + 
        pow((traj_nd.longitude - g_nd.longitude), 2));
    return dist; 
}  

bool LookUp::compare_dist(FSedge* sp1, FSedge* sp2){
    return sp1 -> botlneck_val < sp2 -> botlneck_val;
}

vector<FSedge*> LookUp::SearchNodes(Graph* graph, node traj_nd, int radius){
    vector<FSedge*> se_list;
    /* when building a FSgraph, only need to know the node id, and the initial bottle neck vel */
    for (int i; i < graph -> nodes.size(); i++){
        double dist = dist_from_T0(traj_nd, graph -> nodes[i]);
        if (dist <= radius){
            FSedge se;
            se.trg -> vid = graph -> nodes[i].id;
            se.trg -> tid = 0;
            se.trg -> dist = dist;
            se.trg -> visited = false;
            se.botlneck_val = dist;
            se_list.push_back(&se);
        }
    }
    sort(se_list.begin(), se_list.end(), compare_dist);
    return se_list;
};

int main(){

    // vector<node> nds;
    // struct node nd, traj_nd;
    // nd.id = 888;
    // nd.lat = 3;
    // nd.longitude = 8;
    // nds.push_back(nd);
    // traj_nd.id = 777;
    // traj_nd.lat = 3;
    // traj_nd.longitude = 9999;
// 
    // LookUp look;
    // bool answer = look.SearchNodes(traj_nd, nd, 20); // 
    // 
    // cout<<answer;

    // Graph graph = GRAPH_INIT;
    // read_file(argv[1], &graph);

    return 0;
}