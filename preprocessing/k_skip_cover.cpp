#include "k_skip_cover.h"

vector<node> random_permutate(vector<node> list) {
    vector<node> random(list); 
    random_shuffle(random.begin(), random.end());
    return random;
}

void SP_Tree::BFS(Graph* graph, int depth) {
    queue<int> q; 
    q.push(root);
    q.push(NEW_LEVEL);
    int level = 1;
    while(!q.empty()) {
        int nd = q.front();
        q.pop();
        if(nd == NEW_LEVEL) {
            level++;
            if(level > depth) {
                //no need to traverse any further
                return;
            }
            q.push(NEW_LEVEL);
            if(q.front() == NEW_LEVEL) {
                //we have visited all the nodes tat we need to visit 
                break;
            }
            else {
                continue;
            }
        }
        vector<int> incidents = get_incident(graph, nd);
        for(int i = 0; i < incidents.size(); i++) {
            if(nodes.find(incidents[i]) == nodes.end()) {
                //if node has not been visited before 
                nodes[incidents[i]] = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
                nodes.at(incidents[i])->settled = false;
                nodes.at(incidents[i])->distance = INFINITY;
                nodes.at(incidents[i])->depth = level;
                nodes.at(incidents[i])->covered = NOT_VISITED;

                q.push(incidents[i]);
            }
        }
        
    }
}

bool dijkstra(Graph* graph, SP_Tree* tree, int hops) {
    /*
        Checking if T(k-1)(u) is covered by V*:::
        1 - for every v in T(K-1)(u): 
        2 -     if u-v path with edges >= (k-1) does not pass through at least 1 vertex in V* then: 
        3 -         it is NOT covered -> return 
        4 - it is covered 
        (ie EVERY u-v path with hops (edges) >= k-1 MUST path through at least 1 vertex in V*)
    */
    while(!tree->sp_pq.empty()) {
        //source, target, dist, n_predecessors
        pair<pair<int, int>, pair<double, int> > cur_edge = tree->sp_pq.top();
        tree->sp_pq.pop();

        int edge_source = cur_edge.first.first;
        int edge_target = cur_edge.first.second;
        int n_pre = cur_edge.second.second;
        if(tree->nodes.at(edge_target)->settled) {
            //if node is already settled no need to traverse it again
            continue;
        }
        tree->nodes.at(edge_target) -> settled = true;

        /*
        check if covered: 
        is current edge_target a cover node? if yes we dont really need to check (overwrite flag to true in this case only)
        check if the path leading to it has passed through a cover node (if no: overwrite node's flag no matter what, if yes: check if node's flag is false (if existig flg is false leave it, otherwise mark as true))
        if final node flag is false -> check if the number of predecessor edges is >= hops and see if path is covered or not 
        */

        //check if node is a cover node and overwrite its "covered" flag to true 
        if(graph->nodes[edge_target].cover_node) {
            tree->nodes.at(edge_target)->covered = true;
        }
        else {
            if(edge_target != tree->root) {
                if(tree->nodes.at(edge_source)->covered) {
                    //if the path leading to this node (target) is covered
                    if(tree->nodes.at(edge_target)->covered != NOT_COVERED) {
                        //overwrite the target node's flag to be covered 
                        tree->nodes.at(edge_target)->covered = COVERED;
                    }
                }
                else {
                    tree->nodes.at(edge_target)->covered = NOT_COVERED;
                }
            }
        }
        //if final flag as false and the number of edges for the path leading to this node is >= k-1 ==> tree is not covered
        if(tree->nodes.at(edge_target)->covered == NOT_COVERED) {
            if(n_pre >= hops) {
                return false;
            }
        }

        for(int i = 0; i < get_outdeg(graph, edge_target); i++) {
            int out_edge = get_out_edge(graph, edge_target, i);
            double dist = tree->nodes.at(edge_target)->distance + graph->edges[out_edge].cost;
            int neighbour = graph->edges[out_edge].trgtid;
            
            if(tree->nodes.find(neighbour) == tree->nodes.end()) {
                //if node does not exist in the tree it means its not in the (k-1)-hop vicinity of the root -> no need to include it in the dijkstra sp computation
                continue;
            }
            if(tree->nodes.at(neighbour)->settled) {
                if(tree->is_sp_edge(graph, edge_target, neighbour)) {
                    //only if it is a shortest path edge
                    //check neighbour's flag 
                    //if it is NOT COVERED -> leave it 
                    //if it is COVERED -> (if current node's flag is NOT COVERED: overwrite it to NOT COVERED)
                    if(tree->nodes.at(neighbour)->covered == COVERED) {
                        if(tree->nodes.at(edge_target)->covered == NOT_COVERED) {
                            tree->nodes.at(neighbour)->covered = NOT_COVERED;
                            //if flag changed to NOT COVERED ==> add edge to pq for neighbour to be traversed again with the NOT COVERED path
                            tree->sp_pq.push(make_pair(make_pair(edge_target, neighbour), make_pair(dist, n_pre+1)));
                        }
                    }
                    //if final flag as false and the number of edges for the path leading to this node is >= k-1 ==> tree is not covered
                    if(tree->nodes.at(neighbour)->covered == NOT_COVERED && tree->nodes.at(edge_target)->covered == NOT_COVERED) {
                        if(n_pre+1 >= hops) {
                            return false;
                        }
                    }
                }
                continue;
            }

            if(dist < tree->nodes.at(neighbour)->distance) {
                tree->nodes.at(neighbour) -> distance = dist;
                tree->sp_pq.push(make_pair(make_pair(edge_target, neighbour), make_pair(dist, n_pre+1)));
            }
        }
    }
    return true;
}

int k_skip_cover(int k, Graph* graph) {
    int total_cover_nodes = 0;
    vector<node> vertices = random_permutate(graph->nodes);

    int hops = k-1;
    int i = 0;
    for(node v : vertices) {
        //create T(k-1)(v) 
        cout << i << endl;
        i++;
        SP_Tree* tree = new SP_Tree(v.id);
        /* this will add ALL the nodes in the (k-1)-hop vicinity of the root v in the tree structure 
        so when we run dijkstra we know not to run it further than the nodes that already exist in the tree */
        tree->BFS(graph, hops); 
        if(!dijkstra(graph, tree, hops)) {
            graph -> nodes[v.id].cover_node = true;
            total_cover_nodes++;
        }
        delete tree;
    }
    return total_cover_nodes;
}
