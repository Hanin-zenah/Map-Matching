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
#include "../preprocessing/graph.h" 
#include "../preprocessing/scale_projection.h"
#include "../grid/graph_grid.h"
#include "../grid/graph_grid_starting_node.h"
#include "nlohmann/json.hpp" 


using namespace std;

typedef struct state{
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

        /* number of source candidates */
        int src_candidate(Graph* graph, vector<Gpair> candidates);

        /* Compute a sequence of node IDs that gives the shortest path between two nodes*/
        stack<int> node_to_node_dijkstra(Graph* graph, int node_id, int node_id2);

        /* running the node_to_node_dijkstra between each previous candidate and a current candidate */
        vector<pair<int, double>> tran_dijkstra(Graph* graph, int node_id, vector<Gpair> prev_candidates);

        /* compute transition matrix/probabilities */
        vector<pair<int, double>> tran_matrix(Graph* graph, vector<Gpair> curr_candidates, vector<Gpair> prev_candidates, State prev_state);
        
        /* compute state probabilities */
        State state_prob(Graph* graph, Grid* grid, Point* T1, Point* T2, double beta, double sigma, 
        int n, State prev_state, vector<pair<int, double>> emi_probs, double radius);

        /* create state 0 where the state probabilities are the emission probabilities*/
        State create_state0(vector<pair<int, double>>  emi_set, double num_can);

        /* compute the best path */
        vector<int> best_path(Graph* graph, Grid* grid, Trajectory* traj, int n, double sigma, double beta, double radius);

        /* compute the dijkstra SP between all the vertices on the best path, the best path vertices and the vertices on the SPs togethere is the as final path */
        vector<int> best_path_dijkstra(Graph* graph, vector<int> best_path);

        /* write HMM solution out to a .dat file*/
        void write_HMM_graph(Graph* graph, vector<int> complete_path, string file_name);

        /* compute HMM solution cost*/
        double HMM_path_cost(Graph* graph, vector<int> path);

        /* convert the path to a graph so that we can calculate the frechet distance between it and the trajectory */
        void make_a_HMM_graph(Graph* graph, vector<int> complete_path, Graph* HMM_graph);

        /* write HMM solution out to a .json file*/
        void write_hmm_json(Graph* graph, vector<int> complete_path, string file_name, vector<double> stats);
};

#endif
