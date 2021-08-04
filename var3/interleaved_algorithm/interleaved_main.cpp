#include "interleaved.h"

void test_queries(Graph* graph, HubLabelOffsetList& hl);
void construct_path(Graph* graph);
vector<Trajectory> generate_trajectories(Graph* graph, int max_num_trajectories, int init_dijkstra_rank, int dijkstra_rank_multiplier, double add_noise);


#if USE_HUB_LABELS
    HubLabelOffsetList hl;
#endif


// TODO: trajectory generator using djikstra rank
// problem: gps measurement, what is the path in the graph?
// shortest path mm -> more robust results but increases runtime
// 
// Generator function: takes number of vertices, djikstra rank granularity e.g. 32, doubles each time,
// Noise function: uniform noise for some number of metres
// Use seed for random vertices
// short, medium, large distances
// small, medium, large errors

// motivation/background, problem formulation, present algorithm , results
// problem: djikstra takes very long on these huge graphs, plot of graph, gnuplot of visited nodes
// comparing with interleaved (and naive?) for baseline
// can't store distance between pairs, use hub labels

// Future work: combine with other speedup techniques e.g. kskip
// also sequences of shortest paths

// after presentation: try distance sorted again


std::ofstream tmp_output_file;

int main(int argc, char** argv) {

    #if USE_HUB_LABELS
        hl.importHubLabelsBinary("../../saarland_x100_binary");
        hl.sortByDistance();
    #endif

    /* read processed graph from a given file */
    Graph after_graph = GRAPH_INIT;

    auto t1 = chrono::high_resolution_clock::now(); 
    read_processed_graph("../saarland_all_preprocessed.txt", &after_graph);
    auto t2 = chrono::high_resolution_clock::now(); 
    int graph_read_time = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    cout << "Read graph in " << graph_read_time << "ms" << endl;

    
    double lat_min = after_graph.original_min_lat;
    double lon_min = after_graph.original_min_long;
    double lat_scale = after_graph.lat_scale;
    double lon_scale = after_graph.lon_scale;

    Build_grid builder;
    Grid grid = GRID_INIT;
    double grid_size = 250.00;
    builder.make_grids(&after_graph, &grid, grid_size);
    

    // construct_path(&after_graph);

    // test_queries(&after_graph, hl);
    // exit(0);


    // vector<int> indices;
    // indices.push_back(0);
    // indices.push_back(7);
    // indices.push_back(8);
    // indices.push_back(9);
    // indices.push_back(23);
    // indices.push_back(31);
    // indices.push_back(19);
    // indices.push_back(24);
    // indices.push_back(15);
    // indices.push_back(458);
    // indices.push_back(69);
    // indices.push_back(91);
    // indices.push_back(4);

    // write_graph(&after_graph, "write_graph_output.dat");


    Traj t;
    Traj_subsample ts;


    // vector<Trajectory> trajectories = generate_trajectories(&after_graph, 50, 100, 2, 50);
    // t.write_processed_trajectories(trajectories, "saarland_trajectories.txt");

    vector<Trajectory> trajectories = t.read_processed_trajectories("saarland_trajectories.txt", 0);


    int total_duration = 0;
    for (int i=0; i<trajectories.size(); i++) {
    // for(int i = 0; i < indices.size(); i++) {

        cout << "\n\nTrajectory: " << i << endl;

        Trajectory traj;

        traj = trajectories[i];

        // auto t1 = chrono::high_resolution_clock::now(); 
        // traj = t.read_trajectory_k("../trajectories/saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", indices[i], lon_min, lat_min, lat_scale, lon_scale);
        // auto t2 = chrono::high_resolution_clock::now();

        // vector<Trajectory> test;
        // // t.write_processed_trajectories(test, "test_trajectories.txt");
        // test = t.read_processed_trajectories("saarland_long_trajectory.txt", 40);
        // traj = test[0];

        // t.write_traj(&traj, "saarland_long_trajectory_plot.dat");

        t.calc_traj_edge_cost(&traj);
        ts.subsample_traj(&traj, 15);
        // t.write_traj(&traj, "trajectory.dat");
        cout << "traj size: " << traj.length << endl;

        t1 = chrono::high_resolution_clock::now(); 
        pair<double, double> eps_cost = interleaved_matching(&after_graph, &traj, &grid);
        t2 = chrono::high_resolution_clock::now();
        int duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

        total_duration += (int)duration;

        cout<<"final eps: "<< eps_cost.first <<  " final path cost: "<< eps_cost.second << endl;
        cout << "Duration in milliseconds: " << duration << endl << endl;
        grid.curr_range = 0;

        t.cleanup_trajectory(&traj);
    }
    // float average_duration = (float)total_duration / indices.size();
    // cout << "average_duration: " << average_duration << "ms." << endl;

    cout << "visit_count: " << visit_count << endl;

    return 0;
}


void construct_path(Graph* graph)
{
    Traj t;

    int start = 0;
    int target = 850583;

    SP_Tree* tree = new SP_Tree(start);
    Dijkstra(graph, tree, target);

    vector<int> path;
    tree->get_shortest_path(path, target, graph);

    Trajectory traj = DEF_TRAJ;

    for (int node : path)
    {
        t.add_point(&traj, graph->nodes[node].longitude, graph->nodes[node].lat, 0);
    }

    vector<Trajectory> write_out = {traj};
    t.write_processed_trajectories(write_out, "saarland_long_trajectory.txt");
    t.write_traj(&traj, "saarland_long_trajectory_plot.dat");

    delete tree;
}

vector<Trajectory> generate_trajectories(Graph* graph, int max_num_trajectories, int init_dijkstra_rank, int dijkstra_rank_multiplier, double add_noise)
{
    srand(0);
    Traj t;

    vector<Trajectory> trajectories;
    trajectories.reserve(max_num_trajectories);

    int curr_num_trajectories = 0;
    int curr_dijkstra_rank = init_dijkstra_rank;
    bool finished = false;
    while (curr_num_trajectories < max_num_trajectories && !finished)
    {
        int start_node = rand() % graph->n_nodes;
        SP_Tree* tree = new SP_Tree(start_node);
        bool all_vertices_searched;
        int end_node = DijkstraUntilRankFound(graph, tree, curr_dijkstra_rank, &all_vertices_searched);
        if (all_vertices_searched)
        {
            cout << "Found final Dijkstra rank: " << graph->n_nodes - 1 << '\n';
            finished = true;
        }
        else
        {
            cout << "Found Dijkstra rank: " << curr_dijkstra_rank << '\n';
        }

        vector<int> path;
        tree->get_shortest_path(path, end_node, graph);

        Trajectory traj = DEF_TRAJ;

        for (int node : path)
        {
            double lon_noise = add_noise * (2*((double)rand() / RAND_MAX)-1);
            double lat_noise = add_noise * (2*((double)rand() / RAND_MAX)-1);
            t.add_point(&traj, graph->nodes[node].longitude + lon_noise, graph->nodes[node].lat + lat_noise, 0);
        }
        trajectories.push_back(traj);

        delete tree;

        curr_dijkstra_rank *= dijkstra_rank_multiplier;
        curr_num_trajectories++;
    }

    return trajectories;
}


void test_queries(Graph* graph, HubLabelOffsetList& hl)
{
    // TODO: Change all graph edge weights to same precision?

    int num_nodes = graph->n_nodes;

    // SP_Tree* hub_tree = new SP_Tree(0);
    // for (int n=num_nodes-1; n >= num_nodes-1 - 100; n--)
    // {
    //     double hub_dist = hl.getShortestDistance(0, n);
    //     cout << "path from 0 to " << n << "\t dist: " << hub_dist << endl;
    // }
   


    // int start_id = 0;
    // int target_id = 850583;
    // cout << "START ID: " << start_id << "\tTARGET ID: " << target_id << endl;

    // visit_count = 0;
    // SP_Tree* djikstra_tree = new SP_Tree(start_id);

    // auto t1 = chrono::high_resolution_clock::now(); 
    // Dijkstra(graph, djikstra_tree, target_id);
    // auto t2 = chrono::high_resolution_clock::now();

    // double djikstra_dist = djikstra_tree->nodes[target_id]->distance;
    // int djikstra_time = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    // cout << "Djikstra: " << djikstra_dist << "\tTime: " << djikstra_time << "microseconds" << endl;
    // cout << "visit_count: " << visit_count << endl;
    // delete djikstra_tree;
    
    // visit_count = 0;
    // SP_Tree* hub_tree = new SP_Tree(start_id);
    
    // t1 = chrono::high_resolution_clock::now(); 
    // // double hub_dist = hl.getShortestDistance(start_id, target_id);
    // hubLabelShortestPath(graph, hub_tree, target_id, hl);
    // t2 = chrono::high_resolution_clock::now(); 

    // double hub_dist = hub_tree->nodes[target_id]->distance;
    // int hub_time = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    // cout << "HL: " << hub_dist << "\tTime: " << hub_time << "microseconds\n" << endl;
    // cout << "visit_count: " << visit_count << endl;     
    // delete hub_tree;



    // TODO: run this a very large number of times, write output to file and plot time taken and nodes visited for both methods against the shortest path length (in num nodes)
    // each line format: path_num_nodes, path_dist, djikstra_time, hub_time, djikstra_visited, hub_visited

    for (int i=0; i<10; i++)
    {
        int start_id = rand() % num_nodes;
        int target_id = rand() % num_nodes;
        cout << "START ID: " << start_id << "\tTARGET ID: " << target_id << endl;

        visit_count = 0;
        SP_Tree* djikstra_tree = new SP_Tree(start_id);

        auto t1 = chrono::high_resolution_clock::now(); 
        Dijkstra(graph, djikstra_tree, target_id);
        auto t2 = chrono::high_resolution_clock::now();

        double djikstra_dist = djikstra_tree->nodes[target_id]->distance;
        int djikstra_time = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        cout << "Djikstra: " << djikstra_dist << "\tTime: " << djikstra_time << "microseconds" << endl;
        cout << "visit_count: " << visit_count << endl;  
        delete djikstra_tree;
        
        visit_count = 0;
        SP_Tree* hub_tree = new SP_Tree(start_id);
        
        t1 = chrono::high_resolution_clock::now(); 
        // double hub_dist = hl.getShortestDistance(start_id, target_id);
        hubLabelShortestPath(graph, hub_tree, target_id, hl);
        t2 = chrono::high_resolution_clock::now(); 

        double hub_dist = hub_tree->nodes[target_id]->distance;
        int hub_time = chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        cout << "HL: " << hub_dist << "\tTime: " << hub_time << "microseconds" << endl;     
        cout << "visit_count: " << visit_count << endl;  
        delete hub_tree;

        cout << endl;
    }
}