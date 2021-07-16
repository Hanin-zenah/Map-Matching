#ifndef SHARED_H
#define SHARED_H

#include <iostream>
#include <cmath>
#include <math.h>
#include <utility>
#include <string>
#include <vector> 
#include <algorithm>
#include <queue> 
#include <stack>
#include <unordered_map> 
#include <cstdlib>
#include <chrono>
#include "../../grid/graph_grid.h"
#include "../../grid/graph_grid_starting_node.h"
#include "../../trajectories/trajectory.h"
#include "../../trajectories/trajectory_split.h"
#include "../../preprocessing/graph.h"
#include "../../preprocessing/scale_projection.h"
#include "float.h"

#define FSGRAPH_INIT {0}
#define DOUBLE_CMP_EPS (0.000000000001)
#define GRID_SIZE (250.00)

using namespace std;

typedef struct fsnode {
    int vid;
    int tid;
    double dist;
    bool visited;
    struct fsnode* parent;
} FSnode;

typedef struct fsedge {
    FSnode* src; //super edge will have no source 
    FSnode* trg;
    double botlneck_val;
} FSedge;

//this pair will store Vid and Tid as a pair to be used as a key for the hashmap 
typedef struct FSPair_key {
    int first; //vid
    int second; //tid

    bool operator==(const struct FSPair_key& other) const { 
        return (first == other.first
            && second == other.second);
    }
} FSpair;

struct KeyPairHash {
    size_t operator()(const FSpair& k) const {
        using std::size_t;
        using std::hash;
        using std::string;

        return ((hash<int>()(k.first)
                ^ (hash<int>()(k.second) << 1)) >> 1);
    }
};

class FSgraph {
    public: 
        unordered_map<FSpair, FSnode*, KeyPairHash> pair_dict;
        vector<FSnode*> fsnodes; 
        vector<FSedge*> fsedges;
        ~FSgraph() {
            for(int i = 0; i < fsnodes.size(); i++) {
                free(fsnodes[i]);
            }
            for(int i = 0; i < fsedges.size(); i++) {
                free(fsedges[i]);
            }
        }
};

struct spTreeNodeData {
    bool settled;
    double distance;
};

struct Comp_sp_pq {
    bool operator()(const pair<pair<int, int>, double>& pair1, const pair<pair<int, int>, double>& pair2) const {
        return pair1.second > pair2.second; //ascending order by their distance to the root
    }
};

/* to be stored in the free space priority queue */
struct fs_pq_data {
    FSedge* fsedge;
}; 

/* to sort the min priority queue */
struct Comp_eps { 
    bool operator()(const fs_pq_data* data1, const fs_pq_data* data2) const {
        return data1->fsedge->botlneck_val > data2->fsedge->botlneck_val;
    }
};

class SP_Tree {
    public:
        /* the node id of the tree root on the graph */
        int root; 
        /* this will store the distances to each node from the root */
        unordered_map<int, spTreeNodeData*> nodes; 
        /* this PQ will store edges sorted in ascending order to their "distance" so far from the root for the tree construction 
            ((src, trg), dist(trg))*/
        priority_queue<pair<pair<int,int>,double>, vector<pair<pair<int, int>, double> >, Comp_sp_pq> sp_pq; 
        SP_Tree(int root_id): root(root_id) {
            /* the first SP node (root); 
                add super edge (-1, s.id), distance is 0 */
            nodes[root] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
            nodes.at(root)->distance = 0.0;
            nodes.at(root)->settled = false;
            sp_pq.push(make_pair(make_pair(-1, root), 0.0));
        }
        bool is_sp_edge(Graph* graph, int vid, int neighbour_id) {
            double edge_cost = get_edge_cost(graph, vid, neighbour_id);
            return (edge_cost - (nodes.at(neighbour_id) -> distance - nodes.at(vid) -> distance) <= DOUBLE_CMP_EPS);
        }
        /* returns true if given node is settled in the tree,
             false otherwise (if node does not exist or if node is not settled) */
        bool node_settled(int node) {
            if(nodes.find(node) == nodes.end() || !nodes.at(node)->settled) {
                return false;
            }
            return true;
        }
        /* Deconstruct tree; deallocate all allocated memory */
        ~SP_Tree() {
            for(auto& node: nodes) {
                free(node.second);
            }
        }
};

/* returns the distance between a node on G and a point on the trajectory */
double nodes_dist(node g_nd, Point* t_nd);

/* returns the next reachable free space transition */
struct fs_pq_data* travel_reachable(stack <struct fs_pq_data*>& reachable);

/* runs Dijkstra's shortest path algorithm on graph to continue constructing the SP tree/DAG structure
    until a node on graph with vid = target is settled */
void Dijkstra(Graph* graph, SP_Tree* tree, int target);

/* returns the real cost of a matching path on graph */
double matching_path_cost(Graph* graph, FSnode* final_node);

/* outputs matching path to .dat file to be able to vizualise (e.g. using Gnuplot) */
void print_path(Graph* graph, string file_name, FSnode* final_node);

/* outputs all the nodes traversed on G for a free space graph */
void write_sur_graph(FSgraph* fsgraph, Graph* graph, string file_name);

#endif