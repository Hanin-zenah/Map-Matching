#include "graph.h" // a copy of graph.h with change of nodes struct
#include "trajectory.h"
#include "disc_frechet.h"

double nodes_dist(node g_nd, node t_nd){
    double dist = sqrt(pow((t_nd.lat - g_nd.lat), 2) + pow((t_nd.longitude - g_nd.longitude), 2));
    return dist;
}
// 
// double Euc_distance::euc_dist(double x1, double y1, double x2, double y2, double x_scale, double y_scale) {
    // double dist = sqrt(pow((y2-y1)*x_scale,2.0)+pow((x2-x1)*y_scale,2.0));
    // return dist;
// }

double min_eps(FSgraph* fsgraph){
    int n = fsgraph -> n_V;
    int m = fsgraph -> n_T;
    vector<double> botlnecl_val;

    for (int i = 0; i < n; i){
        for (int j = 0; j < m; j){
            if ( i == 0 && j == 0){
                botlnecl_val.push_back( nodes_dist(v[0], t[0])); // think about if we can replace the c_array with eps annd just recalc
                                                        // eps every step -> might only able to give local optimum??
            }
            else if ( i == 0 && j > 0){
                botlnecl_val.push_back( max(cp.c_array(0,j - 1), nodes_dist(v[0], t[j]))); //unitl the length of the vector reaches m
            }
            else if ( i > 0 && j == 0){
                botlnecl_val[0] = max(cp.c_array(i - 1,0), nodes_dist(v[i], t[0]));
            }
            else{
                botlnecl_val[j] = max(min(cp.c_array(i - 1, j),cp.c_array(i, j - 1), cp.c_array(i - 1,j - 1)), nodes_dist(v[i], t[j]));
            }
            }}
    return botlnecl_val[m-1];}


            // double dist1 = nodes_dist(v[i+1], t[j]);
            // double dist2 = nodes_dist(v[i], t[j+1]);
            // double dist3 = nodes_dist(v[i+1], t[j+1]);
            // eps = std::min({eps,dist1, dist2, dist3},std::less<double>());
            // vector<int> 

            // for (int i = 0; i < n; i++){
            // if (std::min({dist1, dist2, dist3},std::less<double>()) = dist1){ //this is only going to the node that has the current shortest dist.
                // i++;
            // }

            // else if (std::min({dist1, dist2, dist3},std::less<double>()) = dist2){
                // j++;
            // }

            // else {
                // i++;
                // j++;}
        // 