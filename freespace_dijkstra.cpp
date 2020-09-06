#include "disc_frechet_v2.h"
#include "scale_projection.h"
#include <limits>
#include <iostream>
#include <unordered_map>

#define INF_D (numeric_limits<double>::max())


using namespace std;


vector<FSnode*> get_corresponding_FSnodes(FSgraph* fsgraph, int tid) {
    //taking step one: looping through all the nodes in the free space and fetching those 
    vector<FSnode*> candidates;
    for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        if(fsgraph -> fsnodes[i]->tid == tid) {
            candidates.push_back(fsgraph->fsnodes[i]);
        }
    }
    return candidates;
}

struct KeyHash {
    size_t operator()(const FSnode* node) const {
        using std::size_t;
        using std::hash;
        using std::string;

        return ((hash<int>()(node -> tid)
                ^ (hash<int>()(node -> vid) << 1)) >> 1);
    }
};

struct Comp_dijkstra_pq {
    bool operator()(const pair<FSedge*, double>& pair1, const pair<FSedge*, double>& pair2) const {
        return pair1.second > pair2.second; //descending order
    }
};

double edge_cost(FSedge* fedge, Graph* graph){
    int src_id = fedge -> src -> vid;
    int trg_id = fedge -> trg -> vid;
    struct node src_node = graph -> nodes[src_id];
    struct node trg_node = graph -> nodes[trg_id];
    Euc_distance ed;
    double x_scale = graph -> x_scale;
    double y_scale = graph -> y_scale;
    double x1 = src_node.lat;
    double cost = ed.euc_dist(src_node.lat, src_node.longitude, trg_node.lat, trg_node.longitude, x_scale, y_scale);
    return cost;
}

void dijkstta(FSgraph* fsgraph) {
    unordered_map<FSnode*, FSnode*, KeyHash> parent;
    unordered_map<FSnode*, double, KeyHash> distance;

    priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq> PQ; //stores nodes for now, later can change to store only edges 
    vector<FSnode*> source_set = get_corresponding_FSnodes(fsgraph, 0);
    //make super edges with distance 0 
    // vector<FSedge*> super_edges;
    
    //initialize data for dijkstra
    pair<FSedge*, double> p;
    for(FSnode* nd: source_set) {
        distance[nd] = 0;
        //for all the outgoing edges of the starting nodes; add all of them to the priority queue as "active" edges 
        for(FSedge* adj: fsgraph -> adj_list.at(nd)) {
            double cost; //cost of the actual graph edge
            p = make_pair(adj, cost);
            PQ.push(p);
        }

        //make priority queue with pair(node, distance) with all the initial starting sources 
        // p = make_pair(nd, 0);
        // PQ.push(p);   
    }

    for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        FSnode* cur_nd = fsgraph -> fsnodes[i];
        cur_nd -> visited = false;
        parent[cur_nd] = NULL;
        if(distance.at(cur_nd) != 0) {
            distance[cur_nd] = INF_D;
        }
    }

    while(!PQ.empty()) {
        //keep traversing untill target node (vid, m) has been reached (m is the id of the last node on the trajectory)
        pair<FSedge*, double> cur_pair = PQ.top();
        PQ.pop();
        vector<FSnode*> neighbours; //return only the node with reachable edge?
        for(FSnode* adj: neighbours) {
            //if distance is lower 
        }
        cur_pair.first -> visited = true;

    }
    


}




/* note to self: 
    we are storing the edges in the priority queue so we dont have to deal with decrease_key operation of the nodes evey single time we update the distance */