#include "freespace_shortest_path.h"

double Freespace_Dijkstra::edge_cost(FSedge* fedge, Graph* graph) {
    int src_id = fedge -> src -> vid;
    int trg_id = fedge -> trg -> vid;
    struct node src_node = graph -> nodes[src_id];
    struct node trg_node = graph -> nodes[trg_id];
    Euc_distance ed;
    double cost = ed.euc_dist(src_node.lat, src_node.longitude, trg_node.lat, trg_node.longitude);
    return cost;
}
    
FSnode* Freespace_Dijkstra::dijkstra(FSgraph* fsgraph, Graph* graph, int m,
                priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq>& PQ) {

    pair<FSedge*, double> p;
    while(!PQ.empty()) {
        pair<FSedge*, double> cur_pair = PQ.top();
        PQ.pop();
        FSnode* src = cur_pair.first -> src;
        FSnode* org_trg = cur_pair.first -> trg;
        if(org_trg -> settled) { 
            continue;
        }
        org_trg -> settled = true;
        org_trg -> sp_parent = src;

        //stop the loop whenever the last corner (target) is reached (ie any corner with tid = last node of trajectory)
        if(org_trg -> tid == m - 1) {
            return org_trg;
        }
        
        for(FSedge* adj: fsgraph -> adj_list.at(org_trg)) {
            if(!adj -> trg -> settled) {
                if(adj -> botlneck_val <= fsgraph -> eps) {
                    double cost = edge_cost(adj, graph);       
                    FSpair trg_pair; 
                    trg_pair.first = adj -> trg -> vid;
                    trg_pair.second = adj -> trg -> tid;
                    if((org_trg -> sp_dist + cost) < adj -> trg -> sp_dist) {
                        adj -> trg -> sp_dist = org_trg -> sp_dist + cost;
                        // adj -> trg -> sp_parent = org_trg;
                        p = make_pair(adj, adj -> trg -> sp_dist);
                        PQ.push(p);
                    }
                }
            }
        }
    }
    //shouldn't reach here 
    return NULL;
}

stack<FSnode*> Freespace_Dijkstra::find_shortest_path(FSgraph* fsgraph, Graph* graph, int m, string file_name) {
    priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq> PQ; //stores nodes for now, later can change to store only edges 
    for(FSnode* source: fsgraph -> source_set) {
        source -> sp_dist = 0;
    }
    pair<FSedge*, double> p;
    // ---------------------------
    // improve:::: add super edges in the PQ with cost 0 ==> run dijkstra regularly instead of doing the same first step here 
    // ---------------------------
    for(FSnode* nd: fsgraph -> source_set) {
        //for all the outgoing edges of the starting nodes; add all of them to the priority queue as "active" edges 
        for(FSedge* adj: fsgraph -> adj_list.at(nd)) {
            //only add edge for traversal of its bottle neck value is less than (or equal to) the graph's bottleneck
            if(adj -> botlneck_val <= fsgraph -> eps) {
                double cost = edge_cost(adj, graph); //cost of the actual graph edge
                if(adj -> trg -> sp_dist > cost) {
                    adj -> trg -> sp_dist = cost;
                    adj -> trg -> sp_parent = adj -> src;
                    p = make_pair(adj, cost);
                    PQ.push(p);
                }
            }
            nd -> settled = true;
        }
    }
    // cout << "initialized data for dijkstra\n";
    //ready to run dijkstra
    FSnode* cur = dijkstra(fsgraph, graph, m, PQ);
    stack<FSnode*> path;

    if(cur == NULL) {
        cerr << "Dijkstra Failed; returned NULL\n";
        return path;
    }
    cout<<"shorter frechet distance path length "<< cur -> sp_dist <<endl;
    
    //extract path 
    ofstream file(file_name);
    while(cur -> sp_parent) {
        path.push(cur);
        file << graph -> nodes[cur -> vid].longitude << " " << graph -> nodes[cur -> vid].lat
        << " " << graph -> nodes[cur -> sp_parent -> vid].longitude <<" "<< graph -> nodes[cur -> sp_parent -> vid].lat<<endl;
        cur = cur -> sp_parent;  
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
        // <<" "<<graph -> nodes[cur -> sp_parent -> vid].lat <<" "<<graph -> nodes[cur -> sp_parent -> vid].longitude<<endl;
    // }
    // file.close();
    // return;
// }



