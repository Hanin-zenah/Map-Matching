#include "graph.h" // a copy of graph.h with change of nodes struct
#include "disc_frechet_v2.h"

// bool compare_eps(FSedge* edge1, FSedge* edge2) {
    // return edge1 -> botlneck_val > edge2 -> botlneck_val;
// }
// 

unsigned long long int pairing(int n, int m){
    return 0.5*(n+m)*(n+m+1)+m;
}

double nodes_dist(node g_nd, node t_nd) {
    double dist = sqrt(pow((t_nd.lat - g_nd.lat), 2) + pow((t_nd.longitude - g_nd.longitude), 2));
    return dist;
}

double build_node(FSgraph* fsgraph, Graph* graph, Graph* traj, fsnode fsnd, int up, int right){
    FSnode fnd;
    FSedge fedge;

    fnd.vid = fsnd.vid + up ;
    fnd.tid = fsnd.vid + right;
    /* test if the corner/node pair already exists, if not, build a new node, but need to build a new edge regardless */
    if(fsgraph -> pair_dict.find(pairing(fnd.vid, fnd.tid)) == fsgraph -> pair_dict.end()){
        fnd.fspair = pairing(fnd.vid, fnd.tid);
        fnd.visited = false;
        fnd.dist = nodes_dist(graph -> nodes[fnd.vid], traj -> nodes[fnd.tid]);
        fnd.edgelist = {0,0,0}; // initialization? can be overwritten later??
        fsgraph -> fsnodes.push_back(fnd);
        fsgraph -> pair_dict[fnd.fspair] = &fnd;
        fedge.trg = &fnd;
    }
    else {
         fedge.trg = *fsgraph -> pair_dict.find(pairing(fnd.vid, fnd.tid));
         fnd.dist = fedge.trg -> dist; 
    }
    fedge.edgeid = fsgraph -> fsedges.size() + 2 - up - right;
    fedge.src = &fsnd;
    fedge.botlneck_val = max(fnd.dist, fsgraph -> eps); // the fnd.dist would be the same regardless the prior existence of this new corner
    fsgraph -> fsedges.push_back(fedge);
    return fedge.botlneck_val;
    }

unsigned long long int traversal(FSgraph* fsgraph, Graph* graph, Graph* traj, unsigned long long int corner, 
                         priority_queue<FSedge*, vector<FSedge*>, Comp_eps>& bigger_eps, stack <FSedge*>& Stack){
    
   FSnode fnd = *fsgraph -> pair_dict.find(corner);
   double eps1 = build_node(fsgraph, graph, traj, fnd, 1, 1); // always create diagonal move first
   double eps2 = build_node(fsgraph, graph, traj, fnd, 0, 1);
   double eps3 = build_node(fsgraph, graph, traj, fnd, 1, 0);
   int size = fsgraph -> fsnodes.size();
   fsgraph -> fsnodes[size - 4].edgelist = {fsgraph -> fsedges[size - 3].edgeid, fsgraph -> fsedges[size - 2].edgeid, fsgraph -> fsedges[size - 1].edgeid};
   vector<double> btl_neck_vals = {eps1, eps2, eps3};
   queue <FSedge*> temp_queue;
   for(int i = 0; i < btl_neck_vals.size(); i++) {
       if(btl_neck_vals[i] > fsgraph -> eps) {
            // store these local eps for potential global min eps increase
            //store the edges with higher eps to be accessed later if needed
            bigger_eps.push(&fsgraph -> fsedges[fsgraph -> fsedges.size() - i - 1]);
        }
       else { //only store the current 3 outgoing edges, if they meet the condition; refresh at each iteration
           temp_queue.push(&fsgraph -> fsedges[fsgraph -> fsedges.size() - i - 1]);
        }
    }
    //go forward with the traversal 
    unsigned long long int next_fspair;
    //if there are no more available edges in the current cell
    if(temp_queue.empty()) {
       if(Stack.empty()) {
           //case 1: 
           //if there are no more readily traversable edges in the freespace graph
           //update the eps (leash length)
           FSedge* min_eps_fedge = bigger_eps.top();
           bigger_eps.pop();
           fsgraph -> eps = min_eps_fedge -> botlneck_val;
           FSnode* next_nd = min_eps_fedge -> trg; //does the .trg need to be a pointer? does the bigger_eps need to be a queue of pointers?
           next_nd -> visited = true; //change the actual flag on memory and not the copy of it?
           next_fspair = pairing(next_nd -> vid, next_nd -> tid);
       }
       else { 
           //case 2: 
           //cannot proceed with the current cell but doesn't need to change eps yet
           FSedge* back_edge = Stack.top(); //back to the nearest edge that is still reachable and unvisited with this current eps
           Stack.pop();
           FSnode* next_nd = back_edge -> trg;
           next_nd -> visited = true; 
           next_fspair = pairing(next_nd -> vid, next_nd -> tid);

        }
    }
   else { 
       // case 3: proceed to a node within the cell, favouring diagonal movement
       FSedge* selected_edge = temp_queue.front(); //the first one in is always the diagonal move, then right move then up move.
       temp_queue.pop();
       while(!temp_queue.empty()) {
           //FSedge* reachable_edge = temp_queue.top();
           Stack.push(temp_queue.front());
           temp_queue.pop();
       }

       FSnode* next_nd = selected_edge -> trg; //.trg is a FS node.
       next_nd -> visited = true;
       next_fspair = pairing(next_nd -> vid, next_nd -> tid);
   }
   return next_fspair;
}
       
double min_eps(Graph* graph, Graph* traj, FSgraph* fsgraph){
    int n = graph -> nodes.size();
    int m = traj -> nodes.size();

    // int m = traj -> length;
    FSnode fnd;
    FSedge fedge;
    priority_queue<FSedge*, vector<FSedge*>, Comp_eps> bigger_eps;
    stack <FSedge*> Stack; 
    /* building the starting node (V0,T0) */
    fsgraph -> eps = nodes_dist(graph -> nodes[0] /* this needs to be the closest found node on the graph for the map matching*/, traj -> nodes[0]); //change this to be the closest node 
    fnd.vid = graph -> nodes[0].id;
    fnd.tid = traj -> nodes[0].id;
    fnd.fspair = pairing(fnd.vid, fnd.tid);
    fnd.visited = true;
    fnd.edgelist = {0,1,2};// can be overwritten later??
    fsgraph -> fsnodes.push_back(fnd);
    fsgraph -> pair_dict[fnd.fspair] = &fnd;
    while (fsgraph -> pair_dict.find(fnd.fspair) -> tid < m) {
         fnd.fspair = traversal(fsgraph, graph, traj, fnd.fspair, bigger_eps, Stack);
    }
    return fsgraph -> eps;
}


void read_file(string file_name, Graph* graph) {
    if(file_name.empty()) {
        return;
    }
    ifstream file; 
    file.open(file_name);
    if(!file) {
        cerr << "Unable to open file";
        return;
    }

    string buffer;
    /* skip the first five lines */
    for(int i = 0; i < IGNORE_LINES; i++) {
        getline(file, buffer);
    }
    /* read the total number of nodes and edges, store them in graph struct */
    file >> graph -> n_nodes >> graph -> n_edges;

    getline(file, buffer);
    for(int i = 0; i < graph -> n_nodes; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct node n;
        vals >> n.id >> n.osmid >> n.lat >> n.longitude;
        graph -> nodes.push_back(n);
        //check_boundaries(graph -> nodes[i].lat, graph -> nodes[i].longitude, graph);
    }
    for(int i = 0; i < graph -> n_edges; i++) {
        getline(file, buffer);
        istringstream vals(buffer);
        struct edge e;
        vals >> e.srcid >> e.trgtid;
        e.id = i;

        graph -> edges.push_back(e);
    }}


int main(int argc, char** argv){

    if(argc < 2) {
        cerr << "Not enough arguments; please provide a file name next to the program name to be read\n\nExample: ./a.out saarland.txt\n";
        return 1;
    }

    cout<<argv[2]<<endl;

     Graph graph = GRAPH_INIT;
     read_file(argv[1], &graph);

     Graph traj = GRAPH_INIT;
     read_file(argv[2], &traj);

     traj.nodes[2].lat = 16;
    
    FSgraph fsgraph = FSGRAPH_INIT;
    cout<<min_eps(&graph, &traj, &fsgraph);


    return 0;
}