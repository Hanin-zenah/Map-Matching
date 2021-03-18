#ifndef SKIP_COVER_H
#define SKIP_COVER_H

#include "../shared/shared.h"

class FSgraph_ : public FSgraph {
    public: 
        /* bool flag to indicate whether or not we have passed through a cover node */
        bool flag = false; 
        /* indicates whether or not a matching path for this source was found */
        bool done = false;
        int source; /* source node for the free space graph */ 
        int old_tree_size = 0; //temporary for testing purposes ***REMOVE LATER***

        ~FSgraph_() {
            for(FSnode* nd: fsnodes) {
                free(nd);
            }
            for(FSedge* edge: fsedges) {
                free(edge);
            }
        }
};

struct fs_pq_data_ : fs_pq_data {
    FSgraph_* fsgraph;
    SP_Tree* tree;
    /* bool flag to check whether a traversed transition's flag agrees with the
        current flag of the free space */
    bool flag;
};

pair<double, double> match(Graph* graph, Trajectory* traj, Grid* grid);

fs_pq_data_* traversal(Graph* graph, Trajectory* traj, double* global_eps, priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps,
                        stack<struct fs_pq_data*>& reachable, struct fs_pq_data_* cur_edge, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set,
                        unordered_map<int, SP_Tree*>& cover_nodes_sp);

void build_node(struct fs_pq_data_* cur_edge, FSgraph_* fsgraph, Graph* graph, Trajectory* traj, fsnode* fsnd, int neighbor_id, int up, int right,
                    double* global_eps, unordered_map<int, SP_Tree*>& cover_nodes_sp,
                    priority_queue<fs_pq_data_*, vector<fs_pq_data_*>, Comp_eps>& bigger_eps, stack<fs_pq_data*>& reachable);

struct fs_pq_data_* increase_eps(Graph* graph, priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set, 
                                    priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps,
                                    double* global_eps, unordered_map<int, SP_Tree*>& cover_nodes_sp);

void backup_super_edge(priority_queue<Gpair, vector<Gpair>, Comp_dist_to_t>& source_set,
                            priority_queue<struct fs_pq_data_*, vector<struct fs_pq_data_*>, Comp_eps>& bigger_eps,
                            Graph* graph, unordered_map<int, SP_Tree*>& cover_nodes_sp);


#endif
