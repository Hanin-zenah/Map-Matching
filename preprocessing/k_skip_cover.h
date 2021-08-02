/*
    Implementing the kskip cover::: 
    1 - Define a k-skip cover structure V* (in our case we are using a boolean flag added to graph nodes to indicate if a node is a cover node or not)
    2 - randomly permutate the vertices in V of G
    3 - for every vertex u in V: 
    4 -     if T(k-1)(u) is not covered by V* then: 
    5-          add u to V*
*/

/*
    To compute T(k-1)(u):::
    1 - compute V(k-1)(u);; (i.e. all the vertices v in V that can be reached by u by crossing AT MOST k-1 edges)
        and put them in G(k-1)(u) (which has all the edges and nodes)
    2 - compute the shortest paths inside G(k-1)(u) from u to all the other vertices 
*/

/*
    Checking if T(k-1)(u) is covered by V*:::
    1 - for every v in T(K-1)(u): 
    2 -     if u-v path with edges >= (k-1) does not pass through at least 1 vertex in V* then: 
    3 -         it is NOT covered -> return 
    4 - it is covered 
    (ie EVERY u-v path with hops (edges) >= k-1 MUST path through at least 1 vertex in V*)
*/

#ifndef K_SKIP_COVER_H
#define K_SKIP_COVER_H

#include "graph.h"
#include <unordered_map> 
#include <queue>
#include <cmath>
#include <time.h>
#define DOUBLE_CMP_EPS (0.000000000001)
#define NEW_LEVEL (INT_MIN)
#define K (10)

enum Node_flag {
    NOT_VISITED = -1, 
    NOT_COVERED = 0, 
    COVERED = -1
};

struct spTreeNodeData {
    bool settled;
    int depth;
    double distance;
    Node_flag covered;
};

struct pq_edge { 
    int src; 
    int trg; 
    double dist; 
    int n_pre; //number of predecessors nodes
    public:
        pq_edge(int src, int trg, double dist, int n_pre): 
            src(src), trg(trg), dist(dist), n_pre(n_pre) {};
};

struct Comp_sp_pq {
    bool operator()(const pq_edge& edge1, const pq_edge& edge2) const {
        return edge1.dist > edge2.dist; //ascending order by their distance to the root
    }
};

class SP_Tree {
    public:
        int root; //the node id of the tree root in the graph
        unordered_map<int, spTreeNodeData*> nodes; //this will store the distances to each node from the root 
        priority_queue<pq_edge, vector<pq_edge> , Comp_sp_pq> sp_pq;
        SP_Tree(int root_id): root(root_id) {
            //add root to tree with distance 0
            nodes[root] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
            nodes.at(root)->settled = false;
            nodes.at(root)->distance = 0.0;
            nodes.at(root)->depth = 0;
            nodes.at(root)->covered = NOT_COVERED;

            //first source node; add super edge (-1, s.id), distance is 0
            sp_pq.emplace(-1, root, 0.0, 0); 
        }
        bool is_sp_edge(Graph* graph, int vid, int neighbour_id) {
            double edge_cost = get_edge_cost(graph, vid, neighbour_id);
            bool result = false;
            try {
                result = (edge_cost - (nodes.at(neighbour_id) -> distance - nodes.at(vid) -> distance) <= DOUBLE_CMP_EPS);
            } catch(const out_of_range& oor) {
                return false;
            }
            return result;
        }
        //define node_settled function: returns true if given node is settled in the tree, false otherwise (if node does not exist or if node is not settled) 
        bool node_settled(int node) {
            if(nodes.find(node) == nodes.end() || !nodes.at(node)->settled) {
                //node does not exist or node is not settled
                return false;
            }
            return true;
        }

        void BFS(Graph* graph, int depth);
        ~SP_Tree() {
            for(const auto &it: nodes) {
                free(it.second);
            }
        }
};

vector<node> random_permutate(vector<node> list); 

/* 
    runs Dijkstra's shortest path algorithm on a given subset of the graph, and for every SP computed that has k-1 edges, 
    checks if the path passes through a cover node or not
    returns true if the (k-1)-hop tree for a root is covered, otherwise returns false
*/
bool dijkstra(Graph* graph, SP_Tree* tree, int hops);

/* computes the k-skip cover on the graph and returns the number of cover nodes */
int k_skip_cover(int k, Graph* graph);

#endif