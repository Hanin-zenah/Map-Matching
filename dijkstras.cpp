#include "disc_frechet_v2.h"
#include <limits>
#include <iostream>
#include <unordered_map>

#define INF_D (numeric_limits<double>::max())

using namespace std;
/*
Set dijkstra's implementation: 
- put nodes from S in priority queue with distance 0  
    ~ this will be all the vertical nodes in the freespace for the current tn
- run ordinary dijkstras until all the target nodes in T have been reached
- for each node u the distance will be 
    for each s in S:
        min(dist(s, u))
 = dist(S, u)
*/


/*
- for each point i on the trajectory -> find all the <vid, i> nodes on the freespace graph that belong to the set Mi
- find the shortest path from set Mi to everyother point in the set Mi+1 (running set dijkstra's)

*/



//Step 1: 
//fetch all the points from the free space graph given a trajectory id 

//two ways: either 1- loop through the fsnodes vector and fecth everything with an (x, tid) //to make this quicker i can sort them in tid order? but how do i know where it starts and ends? //worst case of O(n x n')
//or 2- use the hashmap to find the nodes that have (x, tid); //if i do this i need to know how high the graph goes //worst time O(n) where n is the number of nodes in the original matched path
//if i wanna know how high the graph goes it will be the last built node in the graph with (vid, tid) where tid is the last node on the trajectory 
//but then what if the first node was 100 and last node was 500k? the path between those two might not include all of the 
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
    bool operator()(const pair<FSnode*, double>& pair1, const pair<FSnode*, double>& pair2) const {
        return pair1.second > pair2.second;
    }
};

double get_edge_cost(FSnode* node, FSnode* neighbour) {
    if(node -> vid == neighbour -> vid) {
        return 0;
    }
    else {
        //return the actual graphs edge cost
        return 1;
    }
}


//length of a path is not the botlneck val of a freespace edge; 
// instead it is the actual graph's edge weight depending if the edge taken is diagonal (step forward on both the trajectory and graph) or horizontal (step forward o the graph alone)
void set_dijkstra(FSgraph* fsgraph, FSnode* target, vector<FSnode*>& starting_set, 
                    priority_queue<pair<FSnode*, double>, vector<pair<FSnode*, double>>, Comp_dijkstra_pq>& PQ, 
                        unordered_map<FSnode*, FSnode*>& parent, unordered_map<FSnode*, double>& distance) {
    
    pair<FSnode*, double> cur_pair = PQ.top();
    PQ.pop();
    //while we haven't reached target node
    while(cur_pair.first != target) {
        //check all of cur node's neighbours 
        vector<FSnode*> neighbours; //get_fs_neighbours() //constant time (at most three neighbours)
        for(FSnode* adj: neighbours) {
            // if(!adj -> visited) {}
            double edge_cost = get_edge_cost(cur_pair.first, adj);
            if(distance.at(cur_pair.first) + edge_cost < distance.at(adj)) {
                distance[adj] = distance.at(cur_pair.first) + edge_cost;
                parent[adj] = cur_pair.first;
            } 
        }
    }
}

void find_shortest_path(FSgraph* fsgraph, double x_scale, double y_scale) {
    unordered_map<FSnode*, FSnode*> parent;
    unordered_map<FSnode*, double> distance;
    // vector<FSnode*> parent(fsgraph -> fsnodes.size(), NULL); hashmap instead?
    // vector<double> distance(fsgraph -> fsnodes.size(), numeric_limits<double>::max()); hashmap instead of vector?
    vector<FSnode*> cur_candidates = get_corresponding_FSnodes(fsgraph, 0);
    priority_queue<pair<FSnode*, double>, vector<pair<FSnode*, double>>, Comp_dijkstra_pq> PQ;

    //initialize data for dijkstra
    for(FSnode* nd: cur_candidates) {
        distance[nd] = 0;
    }
    pair<FSnode*, double> p;
    for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        FSnode* cur_nd = fsgraph -> fsnodes[i];
        cur_nd -> visited = false;
        parent[cur_nd] = NULL;
        if(distance.at(cur_nd) != 0) {
            distance[cur_nd] = INF_D;
        }
        //make priority queue with pair(node, distance)
        p = make_pair(cur_nd, distance.at(cur_nd));
        PQ.push(p);   
    }
    //get the target list
    int next_index = 1;
    vector<FSnode*> next_cands = get_corresponding_FSnodes(fsgraph, next_index);
    // while(next_index <=  )
    
    for(FSnode* target: next_cands) {
        set_dijkstra(fsgraph, target, cur_candidates, PQ, parent, distance);
    }
    
    cur_candidates = next_cands; 
    next_cands = get_corresponding_FSnodes(fsgraph, 2); //and so on


}


//will i need to remake the priority queue everytime i finish finding one path? //becasue of remove min (pop) 
//every time were at a new path remake a pq where the initial distances are the distance of the current nodes except the nodes from the patch before
//
//
//
//pop update then push again into priority queue -> return when target is found




//have the priority queue store only the edges hence running time (mlogm) 
//run dijkstras on the whole freespace surface