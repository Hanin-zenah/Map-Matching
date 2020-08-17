#include "disc_frechet_v2.h"
#include "graph.h"


double nodes_dist(struct node g_nd, struct node t_nd) {
    double dist = sqrt(pow((t_nd.lat - g_nd.lat), 2) + pow((t_nd.longitude - g_nd.longitude), 2));
    return dist;
}

double build_node(FSgraph* fsgraph, Graph* graph, Graph* traj, fsnode fsnd, int neighbor_id, int up, int right) {
    FSnode fnd;
    FSedge fedge;
    //Point* traj
    //traj -> nodes[fsnd.tid] 
    if(up == 0) {
         fnd.vid = fsnd.vid;
    }
    else {
        fnd.vid = neighbor_id;
    }
    fnd.tid = fsnd.tid + right; // right = 1 means Point next.
    // /* test if the corner/node pair already exists, if not, build a new node, but need to build a new edge regardless */
    FSpair pair; // = {fnd.vid, fnd.tid};
    pair.first = fnd.vid;
    pair.second = fnd.tid;
    auto it = fsgraph -> pair_dict.find(pair);
    auto it2 = fsgraph -> pair_dict.end();
    FSnode* fndtest = it -> second;
    int test = fndtest -> tid;

    //cout<<it -> second -> vid <<it2 -> second -> vid;//<<endl;
    if(fsgraph -> pair_dict.find(pair) == fsgraph -> pair_dict.end()) {  //switched with ==
        //if pair not in map 
        cout<<"pair not in map!!!!!! fnd.vid:    "<<fnd.vid<<endl;
        fnd.visited = false; /* didn't really end up making this bool value */
        fnd.dist = nodes_dist(graph -> nodes[fnd.vid], traj -> nodes[fnd.tid]);
        fsgraph -> fsnodes.push_back(fnd);
        fsgraph -> pair_dict[pair] = &fnd; //fsgraph -> pair_dict.insert({pair, &fnd});
        fedge.trg = &fnd;
     }
    else { 
         // /* pair already exists on graph */
        auto it = fsgraph -> pair_dict.find(pair);
        // /* if (it -> visited){ } won't build this node nor edge */
        fedge.trg = it -> second;
        fnd.dist = fedge.trg -> dist; 
    }
    fedge.src = &fsnd;
    fedge.botlneck_val = max(fnd.dist, fsgraph -> eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);
    int size = fsgraph -> fsedges.size();
    cout<<"---------pushed back an edge--------------"<<endl;
    cout<<"fsgraph -> fsedges[size-1].trg -> vid"<<fsgraph -> fsedges[size-1].trg -> vid<<" fsgraph -> fsedges[size-1].trg ->tid: "<<fsgraph -> fsedges[size-1].trg ->tid<<endl;
    cout<<endl;
    return fedge.botlneck_val;
}

FSpair traversal(FSgraph* fsgraph, Graph* graph, Graph* traj, FSpair corner, 
                         priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, stack <FSedge*>& Stack) {
    auto it = fsgraph -> pair_dict.find(corner);
    FSnode* fnd = it -> second;
    vector<int> incidents = get_incident(graph, fnd -> vid);
    vector<double> btl_neck_vals;
    double eps = build_node(fsgraph, graph, traj, *fnd, fnd->vid, 0, 1);
    btl_neck_vals.push_back(eps);

    for(int i = 0 ; i < incidents.size(); i++) {
        cout<<"-------building upward edge and diagonal edge---------"<<endl;
        int neighbour_id  = incidents[i];
        //cout<<"neighbour_id: "<<neighbour_id<<endl;
        double eps1 = build_node(fsgraph, graph, traj, *fnd, neighbour_id, 1, 1);
        btl_neck_vals.push_back(eps1);
        double eps2 = build_node(fsgraph, graph, traj, *fnd, neighbour_id, 1, 0);
        btl_neck_vals.push_back(eps2);
    }

    int size = fsgraph -> fsedges.size();

    for(int i = 0; i < btl_neck_vals.size(); i++) {
        if(btl_neck_vals[i] > fsgraph -> eps) {
                /*store these local eps for potential global min eps increase
                store the edges with higher eps to be accessed later if needed */
                bigger_eps.push(&fsgraph -> fsedges[fsgraph -> fsedges.size() - i -1]);
        }
        else { /* only store the current 3 outgoing edges, if they meet the condition;
                 refresh at each iteration */
            cout<<"fsgraph -> fsedges.size() - i - 1: "<<fsgraph -> fsedges.size() - i - 1<<endl;
            cout<<"fsgraph -> fsedges[fsgraph -> fsedges.size() - i - 1].trg -> vid: "<<fsgraph -> fsedges[fsgraph -> fsedges.size() - i - 1].trg -> vid<<endl;
            Stack.push(&fsgraph -> fsedges[fsgraph -> fsedges.size() - i - 1]);
        }
    }
        //go forward with the traversal 
    FSpair next_fspair;

        /* if there are no more available edges in the current cell */
    if(Stack.empty()) {
        /* case 1: if there are no more readily traversable edges in the freespace graph,
        update the eps (leash length) */
        FSedge* min_eps_fedge = bigger_eps.top();
        bigger_eps.pop();
        fsgraph -> eps = min_eps_fedge -> botlneck_val;
        FSnode* next_nd = min_eps_fedge -> trg; //does the .trg need to be a pointer? does the bigger_eps need to be a queue of pointers?
        next_nd -> visited = true; //change the actual flag on memory and not the copy of it?
        next_fspair.first = next_nd -> vid;
        next_fspair.second = next_nd -> tid;
        cout<<"entered Stack.empty()"<<endl;
    }    
    else { 
        /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
            the current cell, might be from the previous cells if there are no reachable nodes in this cell */
        FSedge* back_edge = Stack.top();
        Stack.pop();
        FSnode* next_nd = back_edge -> trg;
        next_nd -> visited = true; 
        next_fspair.first = next_nd -> vid;
        cout<<"next_nd -> vid; "<< next_nd -> vid<<endl;
        next_fspair.second = next_nd -> tid; //where the problem is at???
        cout<<"next_nd -> tid; "<< next_nd -> tid<<endl;
        cout<<"did not enter Stack.empty()"<<endl;
        }
    return next_fspair;
}
       
double min_eps(Graph* graph, Graph* traj, FSgraph* fsgraph){
    int m = traj -> nodes.size();
    // int m = traj -> length;
    FSnode fnd;
    FSedge fedge;
    priority_queue<FSedge*, vector<FSedge*>, Comp_eps> bigger_eps;
    /* double next_super_eps = 999999;
   bigger_eps.push(next_super_eps);  */
    stack <FSedge*> Stack;
    /* building the starting node (V0,T0) */
                                /* this needs to be the closest found node on the graph for the map matching*/
    fsgraph -> eps = nodes_dist(graph -> nodes[0], traj -> nodes[0]); //change this to be the closest node 
    fnd.vid = graph -> nodes[0].id;
    fnd.tid = 0;
    // fnd.fspair = pairing(fnd.vid, fnd.tid);
    fnd.visited = true;
    //fnd.edgelist = {0,1,2};// can be overwritten later??
    fsgraph -> fsnodes.push_back(fnd);
    FSpair pair; // = {fnd.vid, fnd.tid};
    pair.first = fnd.vid;
    pair.second = fnd.tid;
    fsgraph -> pair_dict[pair] = &fnd;
    bool finished = false;
    
    while (!finished) {
        cout<<!finished<<endl;
    pair = traversal(fsgraph, graph, traj, pair, bigger_eps, Stack);
    finished = pair.second >= m;
    cout<<"pair.second"<<" "<<pair.second<<endl;
   // break;
}
// 
    return fsgraph -> eps;
}