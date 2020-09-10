#include "freespace_shortest_path.h"

/* change this to return the starting nodes with tid 0 AND distance <= radius within the first trajectory point? */
vector<FSnode*> get_corresponding_FSnodes(FSgraph* fsgraph, int tid) {
    //looping through all the nodes in the free space and fetching those 
    vector<FSnode*> candidates;
    for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        if(fsgraph -> fsnodes[i]->tid == tid) {
            candidates.push_back(fsgraph->fsnodes[i]);
        }
    }
    return candidates;
}

double edge_cost(FSedge* fedge, Graph* graph) {
    int src_id = fedge -> src -> vid;
    int trg_id = fedge -> trg -> vid;
    struct node src_node = graph -> nodes[src_id];
    struct node trg_node = graph -> nodes[trg_id];
    Euc_distance ed;
    double x_scale = graph -> x_scale;
    double y_scale = graph -> y_scale;
    double cost = ed.euc_dist(src_node.lat, src_node.longitude, trg_node.lat, trg_node.longitude, graph -> x_scale, graph -> y_scale);
    return cost;
}

bool found_fsnode(FSgraph* fsgraph, FSnode* node) {
    for(FSnode* cur: fsgraph -> fsnodes) {
        if(node == cur) {
            return true;
        }
    }
    return false;
}
    
FSnode* dijkstra(FSgraph* fsgraph, Graph* graph, int m, unordered_map<FSnode*, FSnode*, KeyHash>& parent, unordered_map<FSnode*, double, KeyHash>& distance,
                priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq>& PQ) {

    pair<FSedge*, double> p;
    while(!PQ.empty()) {
        pair<FSedge*, double> cur_pair = PQ.top();
        PQ.pop();

        
        FSnode* src = cur_pair.first -> src;
        FSnode* org_trg = cur_pair.first -> trg;
        if(org_trg -> visited) { 
            continue;
        }
        org_trg -> visited = true;

        //stop the loop whenever the last corner (target) is reached (ie any corner with tid = last node of trajectory)
        if(org_trg -> tid == m - 1) {
            return org_trg;
        }
        
        for(FSedge* adj: fsgraph -> adj_list.at(org_trg)) {
            if(!adj -> trg -> visited) {
                if(adj -> botlneck_val <= fsgraph -> eps) {
                    double cost = edge_cost(adj, graph);       
                    FSpair trg_pair; 
                    trg_pair.first = adj -> trg -> vid;
                    trg_pair.second = adj -> trg -> tid;
                    if((distance.at(org_trg) + cost) < distance.at(adj -> trg)) {

                        distance[adj -> trg] = distance.at(org_trg) + cost;
                        parent[adj -> trg] = org_trg;

                        // //if trg is target node ; break
                        // if(adj -> trg -> tid == m - 1) {
                        //     return adj -> trg;
                        // }

                        //only pushing if distance is smaller //no need to push edge if distance is larger 
                        p = make_pair(adj, distance.at(adj -> trg));
                        PQ.push(p);
                    }
                }
            }
        }
    }
    //shouldn't reach here 
    return NULL;
}

stack<FSnode*> find_shortest_path(FSgraph* fsgraph, Graph* graph, int m) {
    unordered_map<FSnode*, FSnode*, KeyHash> parent;
    unordered_map<FSnode*, double, KeyHash> distance;

    priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq> PQ; //stores nodes for now, later can change to store only edges 
    vector<FSnode*> source_set = get_corresponding_FSnodes(fsgraph, 0);

    //initialize data for dijkstra
    for(FSnode* cur_nd: fsgraph -> fsnodes) {
        cur_nd -> visited = false;
        parent[cur_nd] = NULL;
        distance[cur_nd] = INF_D;
    }

    for(FSnode* source: source_set) {
        distance[source] = 0;
    }
    pair<FSedge*, double> p;
    cout<<source_set.size()<<endl;
    cout<<"inital PQ size: "<<PQ.size()<<endl;
    for(FSnode* nd: source_set) {
        //for all the outgoing edges of the starting nodes; add all of them to the priority queue as "active" edges 
        for(FSedge* adj: fsgraph -> adj_list.at(nd)) {
            //only add edge for traversal of its bottle neck value is less than (or equal to) the graph's bottleneck
            if(adj -> botlneck_val <= fsgraph -> eps) {
                double cost = edge_cost(adj, graph); //cost of the actual graph edge
                if(distance.at(adj -> trg) > cost) {
                    distance[adj -> trg] = cost;
                    parent[adj -> trg] = adj -> src;
                    p = make_pair(adj, cost);
                    PQ.push(p);


                }
            }
            nd -> visited = true;
        }
    }

    //ready to run dijkstra
    FSnode* cur = dijkstra(fsgraph, graph, m, parent, distance, PQ);
    stack<FSnode*> path;

    if(cur == NULL) {
        cerr << "Dijkstra Failed; returned NULL\n";
        return path;
    }
    cur ->dist = distance.at(cur);

    // cout<<"shorter path length "<<cur ->dist<<endl;
    
    //extract path 
    ofstream file("dijkstra_path.dat");
    while(cur) {
        path.push(cur);
        file<< graph -> nodes[cur -> vid].lat <<" "<<graph -> nodes[cur -> vid].longitude
        <<" "<<graph -> nodes[cur -> parent -> vid].lat <<" "<<graph -> nodes[cur -> parent -> vid].longitude<<endl;
        cur = parent.at(cur);   
        
    }
    path.push(cur);
    file.close();

    return path;
}

// void print_dijk_path(stack<FSnode*> SP, Graph* graph, string file_name) {
    // ofstream file(file_name);
    // while(SP.size()>0) {
        // path.push(cur);
        // cout<<SP.size();
        // FSnode* cur = SP.top();
        // SP.pop();
        // file<< graph -> nodes[cur -> vid].lat <<" "<<graph -> nodes[cur -> vid].longitude
        // <<" "<<graph -> nodes[cur -> parent -> vid].lat <<" "<<graph -> nodes[cur -> parent -> vid].longitude<<endl;
    // }
    // file.close();
    // return;
// }

/* note to self: 
    we are storing the edges in the priority queue so we dont have to deal with decrease_key operation of the nodes evey single time we update the distance */

/* 
speedup: 
use bidirectional dijkstra
*/
