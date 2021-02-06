#include "HMM.h"

#include "../DF_sol/starting_node_look_up.h"

Grid_search gs;

vector<Gpair> HMM::candidates(Graph* graph, Grid* grid, Point* traj_nd, int n, double radius){
    grid -> curr_range = 0;
    vector<Gpair> next_n = gs.next_n_nodes(graph, grid, traj_nd, n, radius); //in ascending order by the distance to trajectory node;
    return next_n;
}

Gpair HMM::cloest_can(Graph* graph, Grid* grid, Point* traj_nd){
    grid -> curr_range = 0;
    priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> dist_PQ  = gs.GridSearch(graph, grid, traj_nd);
    Gpair close = dist_PQ.top();
    return close;
}

vector<int> HMM::path_closest_can(Graph* graph, Grid* grid, Trajectory* traj){
    vector<int> path;
    for (int i = 0; i < traj ->length; i++){
        Gpair best_can = cloest_can(graph, grid, traj -> points[i]);
        path.push_back(best_can.first);
        cout<<"node ID: "<<best_can.first<<" distance "<<best_can.second<<endl;

    }
    return path;
}

double HMM::emission(double sigma, double dist){ //change the dist to a T and a candidate and calculate the dist within the function
    double prob = 1/ (sqrt(2 * M_PI) * sigma) * exp( - 0.5 * pow((dist/sigma),2.0));
    return prob;
}

double HMM::transition(double beta, Point*  T1, Point* T2, double SP){
    double dist = sqrt(pow((T2 -> longitude - T1 -> longitude),2.0)+pow((T2 -> latitude - T1 -> latitude),2.0));
    double prob = 1/beta * exp(-abs(SP - dist)/beta);
    return prob;
}


double HMM::sigma_est(Graph* graph, Grid* grid, Trajectory* traj){
    vector<double> error_array;
    for (int i = 0; i < traj -> length; i++){
        grid -> curr_range = 0;
        priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t> PQ  = gs.GridSearch(graph, grid, traj -> points[i]);
        /* the second component in the pair is the euclidean distance of the current vertex to the current trajectory point */
        if(!PQ.empty()){
            Gpair p = PQ.top();
            error_array.push_back(p.second);
        }
    }
    /* finding the median of the error_array */
    double med = getMedian(error_array);

    double sigma = 1.4826 * med; /* based on the formula proposed by Newson and Krumm in 2009 */
    return sigma;
}

double HMM::getMedian(vector<double> array){
    int n = array.size();
    // sort the array
    sort(array.begin(), array.end());
    // check for even case
    if (n % 2 != 0){
        return array[n / 2];
        }
    return (array[(n - 1) / 2] + array[n / 2]) / 2.0;
}
bool HMM::compare_pair_dist(pair<int, double> pair1, pair<int, double> pair2) {
    return pair1.second < pair2.second;
}


vector<pair<int, double>>  HMM::emission_set(Graph* graph, Grid* grid, Point* traj_nd, int n, double sigma, double radius){ //with candidate node ids
    grid -> curr_range = 0;
    vector<Gpair> next_n = candidates(graph, grid, traj_nd, n, radius); // in ascending order by the distance to trajectory node;
    vector<pair<int, double>> emissions;
    double sum_emi =0;
    for (int i = 0; i < next_n.size(); i++){
        double dist = next_n[i].second;
        double emi_prob = emission(sigma, dist);
        pair<int, double> emi_pair; // sorted by the V's distance to T, in increasing order;
        emi_pair.first = next_n[i].first;
        emi_pair.second = emi_prob;
        emissions.push_back(emi_pair);
        sum_emi += emi_prob;
    }
    // normalization for the probabilities
    for (int i =0; i < emissions.size();i++){
        emissions[i].second = emissions[i].second/sum_emi;
        // cout<<"emission probability after normalization: "<<emissions[i].second<<endl;
    }
    return emissions; // sorted by the V's distance to T, in increasing order;
}


vector<pair<int,double>> HMM::get_inv_incident_pair(Graph* graph, int node_id){
    vector<pair<int,double>> incidents;

    int n_neighbours = graph -> in_offsets[node_id + 1] - graph -> in_offsets[node_id];
    int index = graph -> in_offsets[node_id];

    for(int i = index; i < index + n_neighbours; i++) {
        int edge_id = graph -> in_off_edges[i];
        int neighbour_id = graph -> edges[edge_id].srcid;
        pair<int,double> p;
        p.first = neighbour_id;
        p.second = graph -> edges[edge_id].cost;
        incidents.push_back(p);
    }
    return incidents;
}


int HMM::src_candidate(Graph* graph, vector<Gpair> candidates){
    int num_tar = 0;
    for (int i = 0; i < candidates.size(); i++){
        Gpair p = candidates[i];
        graph -> nodes[p.first].target =true;
        // cout<<"flagging: "<<graph -> nodes[p.first].id<<" "<<graph -> nodes[p.first].target<<endl;
        num_tar++;
    }
    return num_tar;
}


double HMM::beta_est(double alpha, double t, double radius){
    double b = alpha/(1.0 - alpha) * radius/ t; 
    /* d is the search radius used to identify candidate roads around the observations zi.
    where t is the maximal ratio between great circle distance and route distance which can be considered plausible.
    The calibration parameter β can be calculated intuitively */
    return b;
}

vector<pair<int, double>> HMM::tran_dijkstra(Graph* graph, int node_id, vector<Gpair> prev_candidates){ // run dijkstra for transition probability calculation
    /* all distance set to infinity while reading the graph */
    int num_tar = src_candidate(graph, prev_candidates);
    priority_queue<struct node, vector<struct node>, comp_travel_cost> PQ;
    vector <int> dirty_nodes; 
    graph -> nodes[node_id].dist = 0; 
    struct node src_nd = graph -> nodes[node_id]; // making a copy of the graph node
    PQ.push(src_nd);
    dirty_nodes.push_back(node_id);
    while(!PQ.empty()){
        struct node nd = PQ.top();
        PQ.pop(); 
        if(nd.settled) { 
            continue;
        }
        nd.settled = true;
        if (nd.target == true){
            nd.target = false;
            num_tar--;
            // cout<<"---------hit one previous candidate: "<<nd.id<<endl;
        }
        if(num_tar == 0){
            break;
        }
        vector<pair<int,double>> incidents = get_inv_incident_pair(graph, nd.id);
        for (pair<int,double> adj : incidents){
            int id = adj.first;
            double distance = adj.second;
            if (graph -> nodes[adj.first].dist > nd.dist + adj.second){
                graph -> nodes[adj.first].dist = nd.dist + adj.second;
                graph -> nodes[adj.first].parent_id = nd.id;
                PQ.push(graph -> nodes[adj.first]);
                dirty_nodes.push_back(graph -> nodes[adj.first].id);
            }
        }
    }
    // extracting the SP from each previous candidate node to this current candidate node
    vector<pair<int, double>> SP;
    for (int i = 0; i < prev_candidates.size(); i++){
        Gpair can = prev_candidates[i];
        SP.push_back(make_pair(graph -> nodes[can.first].id,graph -> nodes[can.first].dist));
    }
    // reset everything touch was touch in this dijkstra for the next run.
    for (int i = 0; i < dirty_nodes.size(); i++){
        graph -> nodes[dirty_nodes[i]].dist = INFINITY;
        graph -> nodes[dirty_nodes[i]].target = false;
        graph -> nodes[dirty_nodes[i]].settled = false;
        graph -> nodes[dirty_nodes[i]].parent_id = -1;
    }
    return SP;
}

State HMM::state_prob(Graph* graph, Grid* grid, Point* T1, Point* T2, double beta, double sigma, 
int n, State prev_state, vector<pair<int, double>> emi_probs, double radius){ 
    // emission vector sorted by the V's distance to T, in increasing order;
    vector<Gpair> curr_candidates = candidates(graph, grid, T2, n, radius); // don't forget trans matric normalization
    vector<Gpair> prev_candidates = candidates(graph, grid, T1, n, radius);
    State curr_state;
    int num_can = curr_candidates.size();
    curr_state.cdd_nd_id.resize(num_can, 0.0);
    curr_state.prdc_state.resize(num_can, -1);
    curr_state.state_prob.resize(num_can, 0.0);

    for (int i = 0; i < curr_candidates.size(); i++){
        Gpair gp = curr_candidates[i]; 
        curr_state.cdd_nd_id[i] = gp.first;
        
        vector<pair<int, double>> trans_vec = tran_dijkstra(graph, gp.first, prev_candidates);
        /* max{P(i,j)*T(k,j)*Q(i-1,k)} for each P(i,j)/Q(i,j)/candidate cell */
        double sum_trans = 0.0;
        for (int j = 0; j < trans_vec.size(); j++){
            pair<int, double> SP = trans_vec[j];
            double trans_prob = transition(beta, T1, T2, SP.second);
            trans_vec[j].second = trans_prob;
            sum_trans += trans_prob;           
        }
        // multiply while normalizing transition probabilities
        for (int j = 0; j < trans_vec.size(); j++){
            pair<int, double> SP = trans_vec[j];
            double pair_prob= prev_state.state_prob[j] * SP.second/sum_trans * emi_probs[i].second;// picking the maximum so the constant emission prob isn't neccesary
            if( pair_prob >= curr_state.state_prob[i]){
                curr_state.state_prob[i] = pair_prob; // curr_state and curr_candidates has the same length, use the same index i
                curr_state.prdc_state[i] = SP.first;
            }
        }
        sum_trans = 0.0; // to reset
    }

    /* normalize the state probability */
    double sum_state_prob = 0.0;// std::accumulate(curr_state.state_prob.begin(), curr_state.state_prob.end(), 0);
    for (int i = 0; i < curr_state.state_prob.size(); i++){
        sum_state_prob += curr_state.state_prob[i];
    }

    // calculatet the sum in a loop so can also figure out what's the max state probablitie and flag the predecessor 
    // -- need to flag predecessor for each state cell not just the max!!!!
    for (int i = 0; i < curr_state.state_prob.size(); i++){
        curr_state.state_prob[i] = curr_state.state_prob[i]/sum_state_prob;
    }
    return curr_state;
}


State HMM::create_state0(vector<pair<int, double>>  emi_set, double num_can){
    State state_0;
    state_0.cdd_nd_id.resize(num_can, 0.0);
    state_0.prdc_state.resize(num_can, -1);
    state_0.state_prob.resize(num_can, 0.0);
    for (int i = 0; i < emi_set.size(); i++){
     state_0.cdd_nd_id[i] = emi_set[i].first ;   
    }
    for (int i = 0; i < emi_set.size(); i++){
     state_0.state_prob[i] = emi_set[i].second;
    }
    return state_0;
}


vector<int> HMM::best_path(Graph* graph, Grid* grid, Trajectory* traj, int n, double sigma, double beta, double radius){ 
    
    vector<pair<int, double>> emi_0 = emission_set(graph, grid, traj->points[0], n, sigma, radius);
    State state0 = create_state0(emi_0, emi_0.size());
    stack<State> state_stack; // want last in first out -- stack
    state_stack.push(state0);
    // State prev_state = state0; 
    int state_num = 1; //delete
    for (int i =  0; i < traj -> length - 1; i++){ // change back to i < traj -> length - 1
        State prev_state = state_stack.top();
        vector<pair<int, double>> curr_emi = emission_set(graph, grid, traj->points[i + 1], n, sigma, radius);
        State cur_state = state_prob(graph, grid, traj->points[i], traj->points[i + 1], beta, sigma, n, prev_state, curr_emi, radius);
        state_stack.push(cur_state);
        prev_state = cur_state;
        state_num++;
    }
    vector<int> best_path; 
    while(!state_stack.empty()){
        State curr = state_stack.top();
        state_stack.pop();
        double max_prob = 0.0;
        int pre_can = -1; //maybe change to -1??
        for (int i = 0; i < curr.prdc_state.size(); i++){
            if( curr.state_prob[i] >= max_prob ){
                pre_can = curr.prdc_state[i];
            }
        }
    best_path.push_back(pre_can); // remove nodes that have the same node ID if they're consective
    state_num--;
    }
    reverse(best_path.begin(), best_path.end());
    /* stack is LIFO, so the match at the traj end is the first one poped, 
    we need to reverse the order, so that the last item pushed 
    from best path is traj[0] match and will be index 0 in the returned path*/
    for (int i = 0; i < best_path.size() - 1; i++){
        if(best_path[i] == best_path[i+1] || best_path[i] == -1){
            // cout<<"erasing: best_path[i]"<<best_path[i]<<endl;
        best_path.erase(best_path.begin() + i);
        i--;
        }
    }
    /* the previous loop cannot check the last one so add an additional check here */
    if (best_path[best_path.size() - 1] == -1){
        best_path.erase(best_path.begin() + best_path.size() - 1);
    }

    return best_path;
}

stack<int> HMM::node_to_node_dijkstra(Graph* graph, int node_id, int node_id2){ 
    priority_queue<struct node, vector<struct node>, comp_travel_cost> PQ;
    vector <int> dirty_nodes; 
    graph -> nodes[node_id].dist = 0; 
    struct node src_nd = graph -> nodes[node_id]; 

    PQ.push(src_nd);
    dirty_nodes.push_back(node_id);
    while(!PQ.empty()){
        struct node nd = PQ.top();
        PQ.pop(); 
        if(nd.id == node_id2){
            break;
        }
        if(nd.settled) { 
            continue;
        }
        nd.settled = true;
        vector<pair<int,double>> incidents = get_inv_incident_pair(graph, nd.id); //
        for (pair<int,double> adj : incidents){
            int id = adj.first;
            double distance = adj.second;
            if (graph -> nodes[adj.first].dist > nd.dist + adj.second){
                graph -> nodes[adj.first].dist = nd.dist + adj.second;
                graph -> nodes[adj.first].parent_id = nd.id;
                PQ.push(graph -> nodes[adj.first]);
                dirty_nodes.push_back(graph -> nodes[adj.first].id);
            }
        }
    }
    stack<int> SP; // Stack: Last In First Out (LIFO)
    int curr_nd = node_id2;
    while(curr_nd != node_id){
        SP.push(curr_nd);
        curr_nd = graph -> nodes[curr_nd].parent_id;
    }
    // reset everything touch was touch in this dijkstra for the next run.
    for (int i = 0; i < dirty_nodes.size(); i++){
        graph -> nodes[dirty_nodes[i]].dist = INFINITY;
        graph -> nodes[dirty_nodes[i]].target = false;
        graph -> nodes[dirty_nodes[i]].settled = false;
        graph -> nodes[dirty_nodes[i]].parent_id = -1;
    }
    return SP;
}

vector<int> HMM::best_path_dijkstra(Graph* graph, vector<int> best_path){
    vector<int> complete_path;
    stack<int> SP;
    for (int i = 0; i < best_path.size() - 1; i++){
        int id = best_path[i];
        int id2 = best_path[i+1];
        SP = node_to_node_dijkstra(graph, id, id2);
        while(!SP.empty()){
            complete_path.push_back(SP.top());
            SP.pop();
        }
    }
    return complete_path;
}

double HMM::HMM_path_cost(Graph* graph, vector<int> path) {
    double path_cost = 0;
    for (int i = 0; i < path.size() - 1; i++){
        int id = path[i];
        int id2 = path[i+1];
        double src_lat, src_lon, trg_lat, trg_lon;
        src_lat = graph -> nodes[id].lat;
        src_lon = graph -> nodes[id].longitude;
        trg_lat = graph -> nodes[id2].lat;
        trg_lon = graph -> nodes[id2].longitude;
        Euc_distance ed;
        double cost = ed.euc_dist(src_lat, src_lon, trg_lat, trg_lon);
        path_cost += cost;
    }
    return path_cost;
}


void HMM::write_HMM_graph(Graph* graph, vector<int> complete_path, string file_name){ 
    ofstream file(file_name);
    for (int i =  0; i < complete_path.size() - 1; i++){
        int id = complete_path[i];
        int id2 = complete_path[i+1];
        double src_lat, src_lon, trg_lat, trg_lon;
        src_lat = graph -> nodes[id].lat;
        src_lon = graph -> nodes[id].longitude;
        trg_lat = graph -> nodes[id2].lat;
        trg_lon = graph -> nodes[id2].longitude;
        // cout<<i<<" "<<id<<" "<<id2<<" "<<src_lon<< " " << src_lat << " " << trg_lon << " " << trg_lat << endl;
        file << src_lon<< " " << src_lat << " " << trg_lon << " " << trg_lat << endl;
    }
    file.close();
}

void HMM::make_a_HMM_graph(Graph* graph, vector<int> complete_path, Graph* HMM_graph){ 
    // HMM_graph = graph;
    // HMM_graph -> edges.clear();
    // HMM_graph -> nodes.clear();
    for (int i =  0; i < complete_path.size(); i++){
        struct node nd;
        nd.id = i;
        nd.lat = graph -> nodes[complete_path[i]].lat;
        nd.longitude = graph -> nodes[complete_path[i]].longitude;
        HMM_graph -> nodes.push_back(nd);
    //    double dist = dist_from_Traj0(traj_nd, HMM_graph -> nodes[i]);
    }
    for (int i =  0; i < complete_path.size() - 1; i++){
        int id = complete_path[i];
        int id2 = complete_path[i+1];
        double src_lat, src_lon, trg_lat, trg_lon;
        src_lat = graph -> nodes[id].lat;
        src_lon = graph -> nodes[id].longitude;
        trg_lat = graph -> nodes[id2].lat;
        trg_lon = graph -> nodes[id2].longitude;
        struct edge e;
        e.srcid = i ;
        e.trgtid = i+1;
        e.id = i;
        Euc_distance ed;
        e.cost = ed.euc_dist(src_lat, src_lon, trg_lat, trg_lon);
        HMM_graph -> edges.push_back(e);
    }
    HMM_graph -> lat_scale = graph -> lat_scale;
    HMM_graph -> lon_scale = graph -> lon_scale;
    HMM_graph -> min_lat = graph -> min_lat;
    HMM_graph -> min_long = graph -> min_long;
    HMM_graph -> max_lat = graph -> max_lat;
    HMM_graph -> max_long = graph -> max_long;
    HMM_graph -> n_nodes = HMM_graph -> nodes.size();
    HMM_graph -> n_edges = HMM_graph -> edges.size();
    outedge_offset_array(HMM_graph);
    inedge_offset_array(HMM_graph);
    
    // output_graph(HMM_graph, file_name, graph -> lat_scale, graph -> lon_scale, 
// graph -> min_lat, graph -> max_lat, graph -> min_long, graph -> max_long);
    return;
}


