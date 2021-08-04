#ifndef INTERLEAVED_H
#define INTERLEAVED_H

#include "../shared/shared.h"

#define USE_HUB_LABELS (true)

#if USE_HUB_LABELS
    extern HubLabelOffsetList hl;
#endif

struct fs_pq_data_ : public fs_pq_data {
    FSgraph* fsgraph;
    SP_Tree* tree;
};

pair<double, double> interleaved_matching(Graph* graph, Trajectory* traj, Grid* grid);

void backup_super_edge(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps);

fs_pq_data_* increase_eps(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps, double* global_eps);

double build_node(FSgraph* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right, double global_eps);

struct fs_pq_data_* traversal(Graph* graph, Trajectory* traj, double* global_eps, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps,
                    stack<struct fs_pq_data*>& reachable, struct fs_pq_data_* cur_edge, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set);

#endif