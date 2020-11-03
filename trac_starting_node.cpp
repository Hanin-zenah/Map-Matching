#include "starting_node_look_up.h"
#include "disc_frechet_v2.h"

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

vector<FSedge*> SearchNodes(Graph* graph, Point* traj_nd, double radius) {
    vector<FSedge*> se_list;
    /* when building a FSgraph, only need to know the node id, and the initial bottle neck val */
    for(int i = 0; i < graph -> nodes.size(); i++) {
        double dist = dist_from_T0(traj_nd, graph -> nodes[i]);
        if(dist <= radius) {
            FSedge* se = (FSedge*) malloc(sizeof(FSedge));
            FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
            se -> src = NULL;
            start_nd -> vid = graph -> nodes[i].id;
            // cout<<"i: "<<i<<" graph -> nodes[i].id: "<<graph -> nodes[i].id<<endl;
            // cout<<"dist: "<<dist<<" start_nd -> vid: "<<start_nd -> vid<<" graph -> nodes[i].lat & long: "<< graph -> nodes[i].lat<<" "<< graph -> nodes[i].longitude <<endl;
            start_nd -> tid = 0;
            start_nd -> dist = dist;
            start_nd -> visited = false;
            se -> trg = start_nd;
            se -> botlneck_val = dist;
            se_list.push_back(se);
            // if(se_list.size() == 2) {
            //     break;
            // }
        }
    }
    sort(se_list.begin(), se_list.end(), compare_dist);
    return se_list;
}
