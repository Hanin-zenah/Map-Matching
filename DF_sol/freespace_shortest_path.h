#include "disc_frechet_grid.h"
#include <limits>

#define INF_D (numeric_limits<double>::max())

using namespace std;


struct KeyHash {
    size_t operator()(const FSnode* node) const {
        using std::size_t;
        using std::hash;
        using std::string;

        return ((hash<int>()(node -> tid)
                ^ (hash<int>()(node -> vid) << 1)) >> 1);
    }
};

struct Comp_dijkstra_pq {
    bool operator()(const pair<FSedge*, double>& pair1, const pair<FSedge*, double>& pair2) const {
        return pair1.second > pair2.second; //descending order
    }
};

class Freespace_Dijkstra{
    public:
    
    /* returns a vector of the freespace graph with tid = given tid */
    vector<FSnode*> get_corresponding_FSnodes(FSgraph* fsgraph, int tid);

    /* returns the edge cost (either 0 or the cost of the original graph edge */
    double edge_cost(FSedge* fedge, Graph* graph);

    /* runs dijkstra on the freespace */
    FSnode* dijkstra(FSgraph* fsgraph, Graph* graph, int m,
                    priority_queue<pair<FSedge*, double>, vector<pair<FSedge*, double>>, Comp_dijkstra_pq>& PQ);

    /* returns the shortest matching path of the freespace */
    stack<FSnode*> find_shortest_path(FSgraph* fsgraph, Graph* graph, int m, string file_name);

    void print_dijk_path(stack<FSnode*> SP, Graph* graph, string file_name);
};