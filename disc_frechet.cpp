#include <iostream>

#include <cmath>
#include <math.h>
#include "graph.h" // a copy of graph.h with change of nodes struct
// #include "trajectory.h"
// #include "disc_frechet.h"

double nodes_dist(node g_nd, node t_nd){
    double dist = sqrt(pow((t_nd.lat - g_nd.lat), 2) + pow((t_nd.longitude - g_nd.longitude), 2));
    return dist;
}
// 
// double Euc_distance::euc_dist(double x1, double y1, double x2, double y2, double x_scale, double y_scale) {
    // double dist = sqrt(pow((y2-y1)*x_scale,2.0)+pow((x2-x1)*y_scale,2.0));
    // return dist;
// }

double min_eps(Graph* graph, Graph* traj){
    int n = graph -> nodes.size();
    int m = traj -> nodes.size();
    vector<double> botlneck_val;
    vector<double> plc_holder;

    for (int i = 0; i < n; i++){
        if ( i > 1){
            plc_holder[m-1] = botlneck_val[m-1];
        }
        for (int j = 0; j < m; j++){
            if ( i == 0 && j == 0){
                botlneck_val.push_back( nodes_dist(graph -> nodes[0], traj -> nodes[0])); 
                plc_holder.push_back(botlneck_val[0]);
            }
            else if ( i == 0 && j > 0){
                botlneck_val.push_back(max(botlneck_val[j-1], nodes_dist(graph -> nodes[0], traj -> nodes[j]))); 
                //unitl the length of the vector reaches m.
                plc_holder.push_back(botlneck_val[j]); 
            }
            else if ( i > 0 && j == 0){
                botlneck_val[0] = max(botlneck_val[0], nodes_dist(graph -> nodes[i], traj -> nodes[0]));
            }
            else{
                botlneck_val[j] = max(min(min(plc_holder[j - 1], botlneck_val[j]), botlneck_val[j - 1]), nodes_dist(graph -> nodes[i], traj -> nodes[j]));
                plc_holder[j - 1] = botlneck_val[j - 1]; //change the min(min()) to sth else!!!!!
            }
            }}
    return botlneck_val[m-1];}


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
    for(int i = 0; i < graph -> n_nodes ; i++) {
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

    //cout<<argv[2]<<endl;

    Graph graph = GRAPH_INIT;
    read_file(argv[1], &graph);

    Graph traj = GRAPH_INIT;
    read_file(argv[2], &traj);

    traj.nodes[2].lat = 16;

    cout<<min_eps(&graph, &traj);


    return 0;
}