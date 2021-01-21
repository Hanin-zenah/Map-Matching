#ifndef HMM_H
#define HMM_H

#include <iostream> 
#include <algorithm>
#include <string>
#include <vector> 
#include <cstdlib>
#include <cmath>
#include <numeric>
#include <limits>
#include <queue>
#include <stack> 
#include "graph.h" 
#include "scale_projection.h"
#include "graph_grid.h"
#include "graph_grid_starting_node.h"


using namespace std;


// an emission probability for each road segment 𝑟𝑖, 𝑝(𝑧𝑡|𝑟𝑖)

//  sigma is the standard deviation of Zs
 
// R is a road segment, Z is a GPS measurement point, X is a point(or an actual vertex on the graph) that is on R and closest to Z

// Then the distance of Z to X we can see it as the GPS measurement error

// the 2009 paper used the median absolute deviation (MAD) technique to estimate the sigma of Z, which is a robust estimator of standard deviation

// double emission(Z, X, sigma);

// t is for timestamp on the trajectory and the i, j are for the node ids of the vertices.

// double transition(Zt, Zt+1, Xt-i, );

typedef struct state{
    // Do I need to initialize this?
    vector<int> cdd_nd_id;
    vector<double> state_prob; // store transition probability here first then update it to state prob?
    vector<int> prdc_state; // predeccessor state ID
} State;


typedef struct all_states{
    vector<State*> states_vec;
} All_states;

struct comp_travel_cost {
    bool operator()(struct node nd1, struct node nd2) {
        return nd1.dist > nd2.dist; 
    }
};


class HMM{
    public:
        vector<Gpair> candidates(Graph* graph, Grid* grid, Point* traj_nd, int n, double radius);

        /* calculating parameters */
        double getMedian(vector<double> array); // delete if delete sigma

        /* delete the sigma, either use a default value or the user-defined value */
        double sigma_est(Graph* graph, Grid* grid, Trajectory* traj);

        /* calculating beta */
        double beta_est(double alpha, double t, double radius);

        /* calculating the emission probability for one candidate */
        double emission(double sigma, double dist);

        /* calculating the transition probability between two candidate given their SP to each other*/
        double transition(double beta, Point*  T1, Point* T2, double SP);

        /* comparing distance */
        bool compare_pair_dist(pair<int, double> pair1, pair<int, double> pair2); 

        /* calculating the emission probabilities for entire set of candidates */
        vector<pair<int, double>>  emission_set(Graph* graph, Grid* grid, Point* traj_nd, int n, double sigma, double radius);

        /* finding all the incoming nodes on the graph from a given node */
        vector<pair<int,double>> get_inv_incident_pair(Graph* graph, int node_id);

        /* number of source candidates????????????????????????? */
        int src_candidate(Graph* graph, vector<Gpair> candidates);

        /* Compute a sequence of node IDs that gives the shortest path between two nodes*/
        stack<int> node_to_node_dijkstra(Graph* graph, int node_id, int node_id2);

        /* running the node_to_node_dijkstra between each previous candidate and a current candidate */
        vector<pair<int, double>> tran_dijkstra(Graph* graph, int node_id, vector<Gpair> prev_candidates);

        vector<pair<int, double>> tran_matrix(Graph* graph, vector<Gpair> curr_candidates, vector<Gpair> prev_candidates, State prev_state);

        State state_prob(Graph* graph, Grid* grid, Point* T1, Point* T2, double beta, double sigma, 
        int n, State prev_state, vector<pair<int, double>> emi_probs, double radius);

        State create_state0(vector<pair<int, double>>  emi_set, double num_can);

        vector<int> best_path(Graph* graph, Grid* grid, Trajectory* traj, int n, double sigma, double beta, double radius);

        vector<int> best_path_dijkstra(Graph* graph, vector<int> best_path);

        void write_HMM_graph(Graph* graph, vector<int> complete_path, string file_name);

        double HMM_path_cost(Graph* graph, vector<int> path);

        vector<int> path_closest_can(Graph* graph, Grid* grid, Trajectory* traj); // delete

        Gpair cloest_can(Graph* graph, Grid* grid, Point* traj_nd); // delete

        /* convert the path to a graph so that we can calculate the frechet distance between it and the trajectory */
        void make_a_HMM_graph(Graph* graph, vector<int> complete_path, Graph* HMM_graph);
};

#endif
