#include "k_skip_cover.h"

int my_random(int i) {
    return rand() % i;
}
vector<node> random_permutate(vector<node> list) {
    vector<node> random(list); 
    random_shuffle(random.begin(), random.end(), my_random);
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
                //we have visited all the nodes we need to visit 
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
                spTreeNodeData* new_node = (spTreeNodeData*) malloc(sizeof(spTreeNodeData));
                new_node -> settled = false; 
                new_node -> distance = INFINITY; 
                new_node -> depth = level; 
                new_node -> covered = NOT_VISITED;

                nodes[incidents[i]] = new_node;
                q.push(incidents[i]);
            }
        }
        
    }
}

bool dijkstra(Graph* graph, SP_Tree* tree, int hops) {
    while(!tree->sp_pq.empty()) {
        pq_edge cur_edge = tree -> sp_pq.top();
        tree -> sp_pq.pop();

        int edge_source = cur_edge.src; 
        int edge_target = cur_edge.trg; 
        int n_pre = cur_edge.n_pre;

        spTreeNodeData* trgt_node = tree->nodes.at(edge_target);

        if(trgt_node->distance < cur_edge.dist) {
            //if we have found a shorter path, skip this edge 
            continue;
        }

        trgt_node -> settled = true;

        // if node is a cover node then path is covered 
        if(graph->nodes[edge_target].cover_node) {
            trgt_node -> covered = COVERED;
        } else {
            if(edge_target != tree->root) {
                //if the path leading to this node (target) is covered
                if(tree->nodes.at(edge_source)->covered) {
                    //if this is first time going through this node
                    if(trgt_node -> covered != NOT_COVERED) {
                        //overwrite the target node's flag to be covered 
                        trgt_node -> covered = COVERED;
                    }
                } else {
                    trgt_node -> covered = NOT_COVERED;
                }
            }
        }

        if(trgt_node -> covered == NOT_COVERED) {
            if(n_pre >= hops) {
                return false;
            }
        }

        for(int i = 0; i < get_outdeg(graph, edge_target); i++) { 
            int out_edge = get_out_edge(graph, edge_target, i);
            double dist = trgt_node -> distance + graph->edges[out_edge].cost;
            int neighbour = graph->edges[out_edge].trgtid;
            
            if(tree->nodes.find(neighbour) == tree->nodes.end()) {
                //if node does not exist in the tree it means its not in the (k-1)-hop vicinity of the root 
                continue;
            }

            spTreeNodeData* neighbour_node = tree->nodes.at(neighbour);
            if(neighbour_node->settled) {
                if(tree->is_sp_edge(graph, edge_target, neighbour)) { 
                    if(neighbour_node->covered == COVERED && !graph->nodes[neighbour].cover_node) {
                        if(trgt_node->covered == NOT_COVERED) {
                            neighbour_node->covered = NOT_COVERED;
                        }
                    }
                    if(neighbour_node->covered == NOT_COVERED && trgt_node->covered == NOT_COVERED) {
                        if(n_pre + 1 >= hops) {
                            return false;
                        }
                        //add edge to pq for neighbour to be traversed again with the NOT COVERED path
                        tree->sp_pq.emplace(edge_target, neighbour, dist, n_pre + 1);
                    }
                }
                continue;
            }

            if(dist <= neighbour_node->distance) { //should add if same length
                neighbour_node->distance = dist;
                tree->sp_pq.emplace(edge_target, neighbour, dist, n_pre + 1);
            }
        }
    }
    return true;
}

int k_skip_cover(int k, Graph* graph) {
    if(k <= 0) 
        return 0; 
    if(k == 1) {
        for(node v: graph -> nodes) {
            graph -> nodes[v.id].cover_node = true;
        }
        return graph -> n_nodes;
    }
    int total_cover_nodes = 0;
    vector<node> vertices = random_permutate(graph->nodes);

    int hops = k-1;
    int i = 0;
    for(node v : vertices) {
        //create T(k-1)(v) 
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
