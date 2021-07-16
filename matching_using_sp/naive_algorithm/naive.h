#ifndef NAIVE_H
#define NAIVE_H

#include "../shared/shared.h"

class FSgraph_ : public FSgraph {
    public:
        double eps; //local eps (leash length value) for the free space graph
};

pair<double, double> naive_matching(Graph* graph, Trajectory* traj, Grid* grid);

fs_pq_data* increase_eps(priority_queue<fs_pq_data*, vector<fs_pq_data*>, Comp_eps>& bigger_eps, FSgraph_* fsgraph);

double build_node(FSgraph* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right);

fs_pq_data* traversal(FSgraph_* fsgraph, Graph* graph, Trajectory* traj, fs_pq_data* cur_edge, priority_queue<fs_pq_data*, vector<fs_pq_data*>, 
                Comp_eps>& bigger_eps, stack <fs_pq_data*>& reachable, SP_Tree* tree, double global_eps);

#endif