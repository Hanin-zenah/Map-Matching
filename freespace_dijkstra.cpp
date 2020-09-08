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

bool found_fsnode(FSgraph* fsgraph, FSnode* node) {
    for(FSnode* cur: fsgraph -> fsnodes) {
        if(node == cur) {
            return true;
        }
    }
    return false;
}
    
<<<<<<< HEAD
FSnode* dijkstra(FSgraph* fsgraph, Graph* graph, int m, unordered_map<FSnode*, FSnode*, KeyHash>& parent, unordered_map<FSnode*, double, KeyHash>& distance,
                priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq>& PQ) {
=======
FSnode* dijkstra(FSgraph* fsgraph, Graph* graph, unordered_map<FSnode*, FSnode*, KeyHash>& parent, unordered_map<FSnode*, double, KeyHash>& distance,
                priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq>& PQ, FSpair final_pair) {
>>>>>>> 16013c927788a9d34b26b6eb5a4c8bb6e7a89c59

    pair<FSedge*, double> p;
    while(!PQ.empty()) {
        // cout << "pq not empty\n";
        //stop the loop whenever the last corner (target) is reached (ie any corner with tid = last node of trajectory)
        pair<FSedge*, double> cur_pair = PQ.top();
        cout<<"target pair:"<<cur_pair.first -> trg->vid<<" "<<cur_pair.first -> trg->tid<<"cur_pair.second: "<<cur_pair.second<<endl;
        PQ.pop();
        // cout << "popped edge\n";
        FSnode* src = cur_pair.first -> src;
<<<<<<< HEAD
        FSnode* org_trg = cur_pair.first -> trg;
        if(org_trg -> tid == m - 1) {  //if it equals the id of the last trajectory node 
            cout << "REACHED TARGET!!\n";
            return org_trg;
            // break;
=======
        FSnode* trg = cur_pair.first -> trg;
        if(trg -> tid == final_pair.second && trg -> vid == final_pair.first) {  //assuming last built node's tid is the final trajectory tid
            return trg;
            break;
>>>>>>> 16013c927788a9d34b26b6eb5a4c8bb6e7a89c59
        }
        // cout << fsgraph -> adj_list.at(org_trg).size() << endl;
        for(FSedge* adj: fsgraph -> adj_list.at(org_trg)) {
            if(adj -> botlneck_val < fsgraph -> eps) {
                double cost = edge_cost(adj, graph);      
                cout << org_trg -> tid << ", " << org_trg -> vid << " ||| " << adj -> trg -> tid << ", " << adj -> trg -> vid << endl;   
                FSpair trg_pair; 
                trg_pair.first = adj -> trg -> vid;
                trg_pair.second = adj -> trg -> tid;

                // if(fsgraph -> pair_dict.find(trg_pair) == fsgraph -> pair_dict.end()) {
                //     cout << "FSNODE DOESN'T EXIST IN DICT\n";
                // }         
                if(!found_fsnode(fsgraph, adj -> trg)) {
                    cout << "FSNODE NOT IN VECTOR ERRORRR" << endl;
                }                                     
                cout << "distance at org_trg = " << distance.at(org_trg) << ", distance at next trg = " << distance.at(adj -> trg)<< endl;
                if((distance.at(org_trg) +  cost) < distance.at(adj -> trg)) {
                    distance[adj -> trg] = distance.at(org_trg) + cost;
                    parent[adj -> trg] = org_trg;

                    // //if trg is target node ; break
                    // if(adj -> trg -> tid == fsgraph -> fsnodes.back() -> tid) {
                    //     return;
                    // }

                    p = make_pair(adj, distance.at(adj -> trg));
                    PQ.push(p);
                }
                cout << "done\n";
            }
        }
    }
    return NULL;
}

stack<FSnode*> find_shortest_path(FSgraph* fsgraph, Graph* graph, int m) {
    // cout<<"adj graph: "<<fsgraph -> adj_list.size()<<endl;
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
        // cout << distance.at(cur_nd) << endl;
    }

    pair<FSedge*, double> p;
    for(FSnode* nd: source_set) {
        distance[nd] = 0;
        // cout<<"current fsnode: "<<nd->vid<<" "<<nd->tid<<endl;
        //for all the outgoing edges of the starting nodes; add all of them to the priority queue as "active" edges 
        for(FSedge* adj: fsgraph -> adj_list.at(nd)) {
            /***********/
            //only add edge for traversal of its bottle neck value is less than the graph's bottleneck --> ask lola about this 
            if(adj -> botlneck_val < fsgraph -> eps) {
                double cost = edge_cost(adj, graph); //cost of the actual graph edge
                distance[adj -> trg] = cost;
                parent[adj -> trg] = adj -> src;
                p = make_pair(adj, cost);
                PQ.push(p);
                // cout << "pushed edge\n";
            }
        }
    }
    cout << "finished initializing maps\n";
    //ready to run dijkstra
    FSnode* cur = dijkstra(fsgraph, graph, m, parent, distance, PQ);
    stack<FSnode*> path;

    if(cur == NULL) {
        cerr << "Dijkstra Failed; returned NULL";
        return path;
    }
    cur ->dist = distance.at(cur);
    cout<<"shorter path length "<<cur ->dist<<endl;
    
    
    // //extract path 
    while(cur) {
        path.push(cur);
        cur = parent.at(cur);
    }
    path.push(cur);

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
            

//are all the nodes being build <even the super sources> being added to fsgraph -> fsnodes? 
//are all the nodes in the adj list in the fsnodes vector of the fsgraph?



//add a settled boolean flag (settle a node whenever u assign a parent to it ?)