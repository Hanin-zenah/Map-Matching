#include "freespace_shortest_path.h"

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
    
FSnode* dijkstra(FSgraph* fsgraph, Graph* graph, unordered_map<FSnode*, FSnode*, KeyHash>& parent, unordered_map<FSnode*, double, KeyHash>& distance,
                priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq>& PQ, FSpair final_pair) {

    pair<FSedge*, double> p;
    while(!PQ.empty()) {
        //stop the loop whenever the last corner (target) is reached (ie any corner with tid = last node of trajectory)
        pair<FSedge*, double> cur_pair = PQ.top();
        cout<<"cur_pair.second: "<<cur_pair.second<<endl;
        PQ.pop();

        FSnode* src = cur_pair.first -> src;
        FSnode* trg = cur_pair.first -> trg;
        if(trg -> tid == final_pair.second && trg -> vid == final_pair.first) {  //assuming last built node's tid is the final trajectory tid
            return trg;
            break;
        }
 
        for(FSedge* adj: fsgraph -> adj_list.at(trg)) {
            if(adj -> botlneck_val < fsgraph -> eps) {
                double cost = edge_cost(adj, graph);
                if((distance.at(trg) +  cost) < distance.at(adj -> trg)) {
                    distance[adj -> trg] = distance.at(trg) + cost;
                    parent[adj -> trg] = trg;

                    // //if trg is target node ; break
                    // if(adj -> trg -> tid == fsgraph -> fsnodes.back() -> tid) {
                    //     return;
                    // }

                    p = make_pair(adj, distance.at(trg) + cost);
                    PQ.push(p);
                }
            }
        }
    }
    return NULL;
}

stack<FSnode*> find_shortest_path(FSgraph* fsgraph, Graph* graph) {
    cout<<"adj graph: "<<fsgraph -> adj_list.size()<<endl;
    unordered_map<FSnode*, FSnode*, KeyHash> parent;
    unordered_map<FSnode*, double, KeyHash> distance;

    priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq> PQ; //stores nodes for now, later can change to store only edges 
    vector<FSnode*> source_set = get_corresponding_FSnodes(fsgraph, 0);
    //make super edges with distance 0 ? -> same as just adding all the outgoing edges from the super sources with their initial lengths
    // vector<FSedge*> super_edges;
    
    //initialize data for dijkstra
    for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        FSnode* cur_nd = fsgraph -> fsnodes[i];
        // cur_nd -> visited = false;
        parent[cur_nd] = NULL;
        distance[cur_nd] = INF_D;
    }

    pair<FSedge*, double> p;
    for(FSnode* nd: source_set) {
        distance[nd] = 0;
        cout<<source_set.size()<<endl;
        cout<<"current fsnode: "<<nd->vid<<" "<<nd->tid<<endl;
        //for all the outgoing edges of the starting nodes; add all of them to the priority queue as "active" edges 
        for(FSedge* adj: fsgraph -> adj_list.at(nd)) {
            cout<<"adj -> botlneck_val: "<<adj -> botlneck_val<<endl;
            /***********/
            //only add edge for traversal of its bottle neck value is less than the graph's bottleneck --> ask lola about this 
            if(adj -> botlneck_val < fsgraph -> eps) {
                double cost = edge_cost(adj, graph); //cost of the actual graph edge
                distance[adj -> trg] = cost;
                p = make_pair(adj, cost);
                PQ.push(p);
            }
        }
    }
    cout<<"entering dijkstra \n";
    cout<<PQ.size()<<endl;
    FSnode* cur = dijkstra(fsgraph, graph, parent, distance, PQ);
     cur ->dist = distance.at(cur);
     cout<<"shorter path fist"<<cur ->dist<<endl;
    if(!cur) {
        cerr << "Dijkstra Failed; returned NULL";
    }
    
    //extract path 
    stack<FSnode*> path;
    while(cur) {
        path.push(cur);
        cur = parent.at(cur);
    }

    return path;
}

/* note to self: 
    we are storing the edges in the priority queue so we dont have to deal with decrease_key operation of the nodes evey single time we update the distance */

/*
two options: 
    1- pop edge and then update distance for target 
    2- pop edge and update distance for the outgoing edges of the target 

*/

/*
when do i relacx an edge? or mark a node as visited? 
what is th epossibility that an edge will be added twice to the priority queue 

*/

/* 
speedup: 
use bidirectional dijkstra
*/


       // if(cur_pair.second < distance.at(trg)) {
        //     distance[trg] = cur_pair.second;
        //     parent[trg] = src;
        //     if(trg -> tid == fsgraph -> fsnodes.back() -> tid) { //assuming last built node's tid is the final trajectory tid
        //         //end loop we found the final node -> return path 
        //         break;
        //         return;

        //     }
        //     cur_pair.first -> relaxed = true; 

        // }
            