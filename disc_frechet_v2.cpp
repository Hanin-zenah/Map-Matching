#include "disc_frechet_v2.h"
 #include "starting_node_look_up.h"

double nodes_dist(struct node g_nd, Point* t_nd, double x_scale, double y_scale) {
    double dist = sqrt(pow((t_nd -> latitude - g_nd.lat)*x_scale, 2.0) + pow((t_nd -> longitude - g_nd.longitude)*y_scale, 2.0));
    return dist;
}


void back_up_se(FSgraph* fsgraph, stack <FSedge*>& Stack, vector<FSedge*>& superEdges){
    if (!superEdges.empty()){
    FSedge* fedge_b = superEdges.back();
    Stack.push(fedge_b);
    superEdges.pop_back();
    FSnode* fnd_b = fedge_b -> trg;
    cout<<"back up superedge"<<endl;
    FSpair back_up_pair; // = {fnd.vid, fnd.tid};
    back_up_pair.first = fnd_b-> vid;
    back_up_pair.second = fnd_b-> tid;
    cout<<"back_up_ pair: "<<back_up_pair.first<<"  " <<back_up_pair.second<<endl;
    fsgraph -> pair_dict[back_up_pair] = fnd_b;
    }
    return;
}

FSpair increase_eps(priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, FSgraph* fsgraph, stack <FSedge*>& Stack){
    FSedge* min_eps_fedge = bigger_eps.top();
    bigger_eps.pop(); 
    /* check if it is a super edge and put the next super edge in the queue  */
    cout<<" Stack is empty, a new eps fron priority Queue: "<< min_eps_fedge -> botlneck_val<<endl;
    fsgraph -> eps = min_eps_fedge -> botlneck_val;
    FSnode* next_nd = min_eps_fedge -> trg; //does the .trg need to be a pointer? does the bigger_eps need to be a queue of pointers?
    next_nd -> visited = true; //QH: is this chaning the bool on the fsgraph for this node as well??
    FSpair next_fspair;
    next_fspair.first = next_nd -> vid;
    next_fspair.second = next_nd -> tid;
    return next_fspair;
}

double build_node(FSgraph* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right, double x_scale, double y_scale) {
    FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
    FSedge* fedge = (FSedge*) malloc(sizeof(FSedge));

    if(up == 0) {
         fnd -> vid = fsnd -> vid;}
    else {
        fnd -> vid = neighbor_id;}

    fnd -> tid = fsnd -> tid + right; // right = 1 means Point next.
    // /* test if the corner/node pair already exists, if not, build a new node, but need to build a new edge regardless */
    FSpair pair; // = {fnd.vid, fnd.tid};
    pair.first = fnd -> vid;
    pair.second = fnd -> tid;

    if(fsgraph -> pair_dict.find(pair) == fsgraph -> pair_dict.end()) {
        //if pair not in map 
        fnd -> visited = false; /* didn't really end up making this bool value */
                                                            /* traj_node -> next */
        fnd -> dist = nodes_dist(graph -> nodes[fnd -> vid], traj -> points[fnd -> tid], x_scale, y_scale); //error is here
        double distance = fnd -> dist; 
        cout<<"creating pair dist: " << distance <<" from: "<<pair.first<<" "<<pair.second<<endl;
        fsgraph -> fsnodes.push_back(fnd);
        fsgraph -> pair_dict[pair] = fnd; //fsgraph -> pair_dict.insert({pair, &fnd});
        fedge -> trg = fnd; 
    }
    else { 
         // /* pair already exists on graph */
         cout<<"pair already exists on graph "<<endl;
        auto it = fsgraph -> pair_dict.find(pair);
        // /* if (it -> visited){ } won't build this node nor edge */
        fedge -> trg = it -> second;
        fnd -> dist = fedge -> trg -> dist; 
        double distance = fnd -> dist; 
        cout<<"existing pair dist: " << distance <<" from: "<<pair.first<<" "<<pair.second<<endl;
    }
    fedge -> src = fsnd; ///and fix this
    fedge -> botlneck_val = max(fnd -> dist, fsgraph -> eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);
    int size = fsgraph -> fsedges.size();

    return fedge -> botlneck_val;
}

FSpair traversal(FSgraph* fsgraph, Graph* graph, Trajectory* traj, FSpair corner, 
                         priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, 
                         stack <FSedge*>& Stack, vector<FSedge*>& superEdges, double x_scale, double y_scale) {
    auto it = fsgraph -> pair_dict.find(corner);
    FSnode* fnd = it -> second;
    vector<int> incidents = get_incident(graph, fnd -> vid);
    vector<double> btl_neck_vals; 
    double eps = build_node(fsgraph, graph, traj, fnd, fnd -> vid, 0, 1, x_scale, y_scale);
    btl_neck_vals.push_back(eps);

    // cout<<"-----building up and diagonal nodes----"<<endl;

    for(int i = 0 ; i < incidents.size(); i++) {
        int neighbour_id  = incidents[i];
        double eps1 = build_node(fsgraph, graph, traj, fnd, neighbour_id, 1, 1, x_scale, y_scale); // build diagonal node //change to return edges 
        btl_neck_vals.push_back(eps1);
        double eps2 = build_node(fsgraph, graph, traj, fnd, neighbour_id, 1, 0, x_scale, y_scale); //build upwards node 
        btl_neck_vals.push_back(eps2);
    }

    int size = fsgraph -> fsedges.size();
    for(int i = 0; i < btl_neck_vals.size(); i++) {
        if(btl_neck_vals[i] > fsgraph -> eps) {
            cout<<"btl_neck_vals "<<i<<" : "<<btl_neck_vals[i]<<" src vid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i] ->src -> vid<<" tid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i ] ->src -> tid<<" trg vid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i ] ->trg -> vid
            <<" tid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i ] ->trg -> tid<<endl;
                // /*store these local eps for potential global min eps increase
                // store the edges with higher eps to be accessed later if needed */
                bigger_eps.push(fsgraph -> fsedges[size - btl_neck_vals.size()+ i]); //is this always going to be the right index of the edge?? yes because btlneck_vals is gonna stroe the last k built edges 
        }
        else { /* only store the current 3 outgoing edges, if they meet the condition;
                 refresh at each iteration */
                 cout<<"btl_neck_vals "<<i<<" : "<<btl_neck_vals[i]<<" src vid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i] ->src -> vid<<" tid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i ] ->src -> tid<<" trg vid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i ] ->trg -> vid
            <<" tid: "<<fsgraph -> fsedges[size - btl_neck_vals.size()+ i ] ->trg -> tid<<endl;
            Stack.push(fsgraph -> fsedges[size - btl_neck_vals.size()+ i ]);
        }
    }
    FSpair next_fspair;

        /* if there are no more available edges in the current cell */
    if(Stack.empty()) {
            /* case 1: if there are no more readily traversable edges in the freespace graph, update the eps (leash length) */
            next_fspair = increase_eps(bigger_eps, fsgraph, Stack);
        }    
    else { 
        /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
            the current cell, might be from the previous cells if there are no reachable nodes in this cell */
        cout<<"stack size: "<<Stack.size()<<endl;
        FSedge* back_edge = Stack.top();
        Stack.pop();
        cout<<"stack size popped: "<<Stack.size()<<endl;
        cout<<"stack empty? "<<Stack.empty()<<endl;
        cout<<"priority queue empty? "<<bigger_eps.empty()<<endl;
        if (!back_edge -> src){
            back_up_se(fsgraph, Stack, superEdges);
        }
        FSnode* next_nd = back_edge -> trg;
        
        next_fspair.first = next_nd -> vid;
        cout<<"next_fspair.first:  "<<next_fspair.first<<endl;
        next_fspair.second = next_nd -> tid;
        cout<<"next_fspair.second:  "<<next_fspair.second<<endl;
        if(fsgraph -> pair_dict.find(next_fspair) != fsgraph -> pair_dict.end()){
            auto it = fsgraph -> pair_dict.find(next_fspair);
            FSnode* existed_fnd = it -> second;
            if(existed_fnd -> visited){
                if(Stack.empty()) {
                    next_fspair = increase_eps(bigger_eps, fsgraph, Stack);
                }  
                else{//QH finish later

                }  
            }
        }
        next_nd -> visited = true; 
    }
    return next_fspair;
}
       
double min_eps(Graph* graph, Trajectory* traj, FSgraph* fsgraph, double radius, double x_scale, double y_scale){
    int m = traj -> length - 65; 

    // FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
    // FSedge* fedge = (FSedge*) malloc(sizeof(FSedge));
    priority_queue<FSedge*, vector<FSedge*>, Comp_eps> bigger_eps;
    stack <FSedge*> Stack;
    vector<FSedge*> superEdges = SearchNodes(graph, traj -> points[0], radius, x_scale, y_scale);
    if (superEdges.empty()){
        cerr << "Nothing within the required distance"<<endl;
        return -1;
    }
    
    FSedge* fedge = superEdges.back();
    fsgraph -> eps = fedge -> botlneck_val;//nodes_dist(graph -> nodes[0], traj -> nodes[0]); //change this to be the closest node /*traj -> head */
    FSnode* fnd = fedge -> trg;
    fnd -> visited = true;
    fsgraph -> fsnodes.push_back(fnd);
    superEdges.pop_back();
    FSpair pair; // = {fnd.vid, fnd.tid};
    pair.first = fnd -> vid;
    pair.second = fnd -> tid;
    cout<<"starting pair: "<<pair.first<<"  " <<pair.second<<endl;
    fsgraph -> pair_dict[pair] = fnd;

    back_up_se(fsgraph, Stack, superEdges);
    bool finished = false;
    
    while (!finished) {
        pair = traversal(fsgraph, graph, traj, pair, bigger_eps, Stack, superEdges, x_scale, y_scale);
        cout<<"current eps: "<<fsgraph -> eps<<endl;
        finished = (pair.second >= m - 1);
    }
    return fsgraph -> eps;
}

void cleanup(FSgraph* fsgraph) {
    for(int i = 0; i < fsgraph -> fsnodes.size(); i++) {
        free(fsgraph -> fsnodes[i]);
    }
    for(int i = 0; i < fsgraph -> fsedges.size(); i++) {
        free(fsgraph -> fsedges[i]);
    }
}

void write_fsgraph(FSgraph* fsgraph, string file_name) { 
    ofstream file(file_name);
    for(int i = 0; i < fsgraph -> fsedges.size(); i++) {
        //x y x y 
        int source_vid = fsgraph -> fsedges[i] -> src -> vid;
        int source_tid = fsgraph -> fsedges[i] -> src -> tid;
        int target_vid = fsgraph -> fsedges[i] -> trg -> vid;
        int target_tid = fsgraph -> fsedges[i] -> trg -> tid;

        // file << source_tid<< " " << source_vid << " " << target_tid << " " << target_vid << endl; //what we wanted it to look like originally
        file << source_vid<< " " << source_tid << " " << target_vid << " " << target_tid << endl; // what (Vi, Tj) should looks like
    }
    file.close();
}