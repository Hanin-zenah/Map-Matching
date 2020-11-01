#include "disc_frechet_grid.h"
#include "graph_grid.h"

double nodes_dist(struct node g_nd, Point* t_nd) {
    double dist = sqrt(pow((t_nd -> latitude - g_nd.lat), 2.0) + pow((t_nd -> longitude - g_nd.longitude), 2.0));
    return dist;
}

bool found_fsnode_vec(FSgraph* fsgraph, FSnode* node) {
    for(FSnode* cur: fsgraph -> fsnodes) {
        if(node == cur) {
            return true;
        }
    }
    return false;
}


void back_up_se(FSgraph* fsgraph, priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, 
priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& PQ) {
    if(!PQ.empty()) {
        Gpair gp = PQ.top();
        PQ.pop();
        double dist = gp.second;
        FSedge* se = (FSedge*) malloc(sizeof(FSedge));
        FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
        se -> src = NULL;
        start_nd -> vid = gp.first;
        start_nd -> tid = 0;
        start_nd -> dist = dist;
        start_nd -> visited = false;
        se -> trg = start_nd;
        se -> botlneck_val = dist;

        bigger_eps.push(se);
        
        FSnode* fnd_b = se -> trg;
        fnd_b -> parent = NULL;
        FSpair back_up_pair; // = {fnd.vid, fnd.tid};
        back_up_pair.first = fnd_b -> vid;
        back_up_pair.second = fnd_b -> tid;
        fsgraph -> pair_dict[back_up_pair] = fnd_b;
        if(!found_fsnode_vec(fsgraph, fnd_b)) {
            fsgraph -> fsnodes.push_back(fnd_b);
        }
        // fsgraph -> fsnodes.push_back(fnd_b);
        vector<FSedge*> vec;
        fsgraph -> adj_list[fnd_b] = vec;
    }
    return;
}

FSnode* increase_eps(priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, FSgraph* fsgraph, 
priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& PQ, Point* traj_nd, Graph* graph, Grid* grid){
    FSedge* min_eps_fedge = bigger_eps.top();
    bigger_eps.pop(); 
    /* check if it is a super edge and put the next super edge in the queue  */
    /* if the edge is a super edge */
    if(!min_eps_fedge -> src) {
        if (!PQ.empty()){
            back_up_se(fsgraph, bigger_eps, PQ);
        }
        else{
            while (PQ.empty()){
                ExtendGrid(graph, grid, traj_nd, PQ);
                }
            back_up_se(fsgraph, bigger_eps, PQ);
        }
    }
    fsgraph -> eps = min_eps_fedge -> botlneck_val;
    FSnode* next_nd = min_eps_fedge -> trg;
    
    return next_nd;
}

FSnode* travel_reachable(FSgraph* fsgraph, stack <FSedge*>& Stack){
    /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
        the current cell, might be from the previous cells if there are no reachable nodes in this cell */
    // cout<<"stack size: "<<Stack.size()<<endl;
    FSedge* next_edge = Stack.top();
    Stack.pop();
    FSnode* next_nd = next_edge -> trg;

    return next_nd;
    }

double build_node(FSgraph* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right) {
    // FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
    FSedge* fedge = (FSedge*) malloc(sizeof(FSedge));

    FSpair pair; // = {fnd.vid, fnd.tid};
    // pair.first = fnd -> vid;
    // pair.second = fnd -> tid;

    if(up == 0) {
        pair.first = fsnd -> vid;
        // fnd -> vid = fsnd -> vid;
    } else {
        pair.first = neighbor_id;
        // fnd -> vid = neighbor_id;
    }
    pair.second = fsnd -> tid + right;

    // fnd -> tid = fsnd -> tid + right; 
    /* test if the corner/node pair already exists, if not, build a new node, but need to build a new edge regardless */

    if(fsgraph -> pair_dict.find(pair) == fsgraph -> pair_dict.end()) {
        //if pair not in map 
        FSnode* fnd = (FSnode*) malloc(sizeof(FSnode));
        fnd -> vid = pair.first; 
        fnd -> tid = pair.second;
        fnd -> visited = false; 
        // fnd -> reachable = false; 
        fnd -> dist = nodes_dist(graph -> nodes[fnd -> vid], traj -> points[fnd -> tid]); //error is here
        fnd -> parent = fsnd; 
        fsgraph -> fsnodes.push_back(fnd);
        fsgraph -> pair_dict[pair] = fnd; //fsgraph -> pair_dict.insert({pair, &fnd});

        fedge -> trg = fnd; 
        vector<FSedge*> vec;
        fsgraph -> adj_list[fnd] = vec;
    }
    else { 
         // /* pair already exists on graph */
        auto it = fsgraph -> pair_dict.find(pair);
        // /* if (it -> visited){ } won't build this node nor edge */
        fedge -> trg = it -> second;
        // it -> second -> parent = fsnd;
        // fedge -> trg -> parent = fsnd;
        // fnd -> dist = fedge -> trg -> dist; 
        // fnd -> parent = fsnd; //QH
    }

    fedge -> src = fsnd; ///and fix this

    //add to adjacency list here ... 
    // if(fsgraph -> adj_list.find(fsnd) == fsgraph -> adj_list.end()) {
    //     cout << "ADJACENCY LIST NOT FOUND\n";
    // }
    // else {
        // cout << "FOUND ADJACENCY LIST\n";
        fsgraph -> adj_list.at(fsnd).push_back(fedge);

    // }

    fedge -> botlneck_val = max(fedge -> trg -> dist, fsgraph -> eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);
    int size = fsgraph -> fsedges.size();

    return fedge -> botlneck_val;
}

FSpair traversal(FSgraph* fsgraph, Graph* graph, Trajectory* traj, FSpair corner, priority_queue<FSedge*, vector<FSedge*>, 
                Comp_eps>& bigger_eps, stack <FSedge*>& Stack, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& PQ, 
                Point* traj_nd, Grid* grid) {
    auto it = fsgraph -> pair_dict.find(corner);
    FSnode* fnd = it -> second;
    vector<int> incidents = get_incident(graph, fnd -> vid);
    vector<double> btl_neck_vals; 
    double eps = build_node(fsgraph, graph, traj, fnd, fnd -> vid, 0, 1);
    //fsgraph
    btl_neck_vals.push_back(eps); // QH: maybe we can sort this and make the diagonal ones always traverse last?
    // cout<<"incidents.size(): "<<incidents.size()<<endl;
    for(int i = 0; i < incidents.size(); i++) {
        int neighbour_id  = incidents[i];
        double eps1 = build_node(fsgraph, graph, traj, fnd, neighbour_id, 1, 1); // build diagonal node //change to return edges 
        btl_neck_vals.push_back(eps1);
        double eps2 = build_node(fsgraph, graph, traj, fnd, neighbour_id, 1, 0); //build upwards node 
        btl_neck_vals.push_back(eps2);
    }

    int size = fsgraph -> fsedges.size();
    for(int i = 0; i < btl_neck_vals.size(); i++) {
        if(btl_neck_vals[i] > fsgraph -> eps) {
            bigger_eps.push(fsgraph -> fsedges[size - btl_neck_vals.size()+ i]); //is this always going to be the right index of the edge?? yes because btlneck_vals is gonna stroe the last k built edges 
        }
        else {  
            Stack.push(fsgraph -> fsedges[size - btl_neck_vals.size() + i ]);
        }
    }
    FSnode* next_nd = (FSnode*) malloc(sizeof(FSnode));
    next_nd -> visited = true;// maybe where the problem is
    // cout<<"next_nd -> visited: "<<next_nd -> visited<<endl;
    while(next_nd -> visited) {
        // cout<<"Stack.empty(): "<<Stack.empty()<<endl;
        if(Stack.empty()) {
                /* case 1: if there are no more readily traversable edges in the freespace graph, update the eps (leash length) */
                next_nd = increase_eps(bigger_eps, fsgraph, PQ, traj_nd, graph, grid); 
        }    
        else { 
            /* case 2: proceed to the next reachable node, favouring diagonal movement. this node might be from 
                the current cell, might be from the previous cells if there are no reachable nodes in this cell */
            
            next_nd = travel_reachable(fsgraph, Stack);
        }
    } 
    next_nd -> visited = true;
    FSpair next_fspair;
    next_fspair.first = next_nd -> vid;
    next_fspair.second = next_nd -> tid;

    // cout<<"next_fspair.first: "<<next_fspair.first<<" next_fspair.second: "<<next_fspair.second<<endl;
    return next_fspair;
}
       
FSpair min_eps(Graph* graph, Trajectory* traj, FSgraph* fsgraph, Grid* grid) {
    int m = traj -> length;
    Point* traj_nd = traj -> points[0];

    priority_queue<FSedge*, vector<FSedge*>, Comp_eps> bigger_eps;
    stack <FSedge*> Stack;
    /*
     * find the initial closest nodes to the first point in the trajectory using the grid
     * sorted by descending distance 
     */

    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> grid_PQ;
    grid_PQ = GridSearch(graph, grid, traj -> points[0]);

    if(grid_PQ.empty()) {
        cerr << "Error -- couldn't find any node in the grid"<<endl;
        // return -1;
    }

    Gpair gp = grid_PQ.top();
    grid_PQ.pop();
    double dist = gp.second;
    FSedge* se = (FSedge*) malloc(sizeof(FSedge));
    FSnode* start_nd = (FSnode*) malloc(sizeof(FSnode));
    se -> src = NULL;
    start_nd -> vid = gp.first;
    start_nd -> tid = 0;
    start_nd -> dist = dist;
    start_nd -> visited = true;
    start_nd -> parent = NULL;
    se -> trg = start_nd;
    se -> botlneck_val = dist;

    fsgraph -> eps = se -> botlneck_val;
    fsgraph -> fsnodes.push_back(start_nd);

    FSpair pair; // = {fnd.vid, fnd.tid};
    pair.first  = start_nd -> vid;
    pair.second = start_nd -> tid;
    // cout<<"starting pair: "<<pair.first<<"  " <<pair.second<<endl; 
    fsgraph -> pair_dict[pair] = start_nd;
    vector<FSedge*> vec;
    fsgraph -> adj_list[start_nd] = vec;

    back_up_se(fsgraph, bigger_eps, grid_PQ);
    bool finished = false;
    
    int i = 0;
    while (!finished) {
        pair = traversal(fsgraph, graph, traj, pair, bigger_eps, Stack, grid_PQ, traj_nd, grid);
        cout<<"current eps: "<<fsgraph -> eps<<" iteration: "<< i <<" "<<pair.first<<" "<<pair.second<<endl;
        i++;
        finished = (pair.second >= m - 1);
        
    }
    return pair;
}

double path_cost(FSgraph* fsgraph, Graph* graph, FSpair pair) {
    FSnode* cur = fsgraph -> pair_dict.at(pair);
    double path_cost = 0;
    while(cur -> parent) {
        FSnode* cur_parent = cur -> parent;
        int src_id = cur_parent -> vid;
        int trg_id = cur -> vid;
        struct node src_node = graph -> nodes[src_id];
        struct node trg_node = graph -> nodes[trg_id];
        Euc_distance ed;
        double cost = ed.euc_dist(src_node.lat, src_node.longitude, trg_node.lat, trg_node.longitude);
        path_cost += cost;
        cur = cur -> parent;
    }
    return path_cost;
}


void print_path(FSgraph* fsgraph, Trajectory* traj, Graph* graph, string file_name, FSpair pair) {
    ofstream file(file_name);
    FSnode* cur = fsgraph -> pair_dict.at(pair);
    while(cur -> parent) {
        // path.push(cur);
        file<< graph -> nodes[cur -> vid].longitude <<" "<<graph -> nodes[cur -> vid].lat
        <<" "<<graph -> nodes[cur -> parent -> vid].longitude <<" "<<graph -> nodes[cur -> parent -> vid].lat<<endl;
        cur = cur -> parent;
    }
    file.close();
    return;
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

void write_sur_graph(FSgraph* fsgraph, Graph* graph, string file_name) { 
    ofstream file(file_name);
    for(int i = 0; i < fsgraph -> fsedges.size(); i++) {
        //x y x y 
        int source_vid = fsgraph -> fsedges[i] -> src -> vid;
        int target_vid = fsgraph -> fsedges[i] -> trg -> vid;

        double src_lat, src_lon, trg_lat, trg_lon;
        src_lat = graph -> nodes[source_vid].lat;
        src_lon = graph -> nodes[source_vid].longitude;
        trg_lat = graph -> nodes[target_vid].lat;
        trg_lon = graph -> nodes[target_vid].longitude;


        // file << source_tid<< " " << source_vid << " " << target_tid << " " << target_vid << endl; //what we wanted it to look like originally
        file << src_lon<< " " << src_lat << " " << trg_lon << " " << trg_lat << endl; // what (Vi, Tj) should looks like
    }
    file.close();
}