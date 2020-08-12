
#include <cmath>
#include "graph.h" // a copy of graph.h with change of nodes struct
#include "scale_projection.h"
#include "starting_node_look_up.h"

using namespace std;


//  box_height = g_dist1;
//  box_width = (g_dist2 + g_dist3) * 0.5;


vector<int> Lookup::assign_grid(node nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale){
    vector<int> grid;
    int num_grid_h, num_grid_w;
    num_grid_h = box_height / grid_h;
    num_grid_w = box_width / grid_w;
    grid.push_back(floor((nd.lat- lat_min_to_y) * x_scale/ num_grid_w));
    grid.push_back(floor((nd.longitude - lon_min_to_x) * y_scale/ num_grid_h)); //the height
   
    return grid;
}

vector<int> LookUp::nodes_in_same_grid(Graph* graph, node traj_nd, int grid_h, int grid_w, double box_height, double box_width, double x_scale, double y_scale){
    vector<int> nodes_same_grid;
    vector<int> traj_nd_grid = assign_grid(traj_nd, grid_h, grid_w, box_height, box_width, x_scale, y_scale);
    for (int i; i < graph -> nodes.size(); i++){
        vector<int> nd_grid = assign_grid(graph -> nodes[i], grid_h, grid_w, box_height, box_width, x_scale, y_scale);
        if (nd_grid[0] == traj_nd_grid[0] && nd_grid[1]  == traj_nd_grid[1]){
            nodes_same_grid.push_back(graph -> nodes[i]);
        }
    }
    return nodes_same_grid;
}


bool LookUp::withinDist(node traj_nd, node g_nd, int radius) {
    int dist; 
    dist = sqrt(pow((traj_nd.lat - g_nd.lat), 2) + 
        pow((traj_nd.longitude - g_nd.longitude), 2));
    return (radius >= dist); 
}  

vector<struct node> LookUp::SearchNodes(Graph* graph, node traj_nd, int radius){
    vector<struct node> nodeslist;
    for (int i; i < graph -> nodes.size(); i++){
        if withinDist(traj_nd, graph -> nodes[i] , r){
            nodeslist.push_back(graph -> nodes[i].id);
        }
    }
    return nodeslist;
};

int main(){

    vector<node> nds;
    struct node nd, traj_nd;
    nd.id = 888;
    nd.lat = 3;
    nd.longitude = 8;
    nds.push_back(nd);
    traj_nd.id = 777;
    traj_nd.lat = 3;
    traj_nd.longitude = 9999;

    LookUp look;
    bool answer = look.withinDist(traj_nd, nd, 20); // 
    
    cout<<answer;

    // Graph graph = GRAPH_INIT;
    // read_file(argv[1], &graph);

    return 0;
}