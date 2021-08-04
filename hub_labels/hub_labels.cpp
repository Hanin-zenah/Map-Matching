#include "hub_labels.h"

#define BUFFER_SIZE (1 * 1024 * 1024 * 1024)

char buffer[BUFFER_SIZE];

void HubLabelOffsetList::importHubLabels(string label_filename, string order_filename)
{
    auto t1 = chrono::high_resolution_clock::now(); 

    // Open the order file
    std::ifstream input_file;
    input_file.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);

    input_file.open(order_filename);
    if(!input_file) {
        std::cerr << "Unable to open order file." << std::endl;
        return;
    }

    // Read the number of vertices
    input_file >> n_vertices;
    std::cout << "Reading " << n_vertices << " vertices." << std::endl;

    // Initialise array to store the order
    std::vector<int32_t> order;
    order.reserve(n_vertices);

    // Store the order in the array
    for (int32_t i=0; i<n_vertices; i++)
    {
        int32_t order_index;
        input_file >> order_index;
        order.push_back(order_index);
    }
    input_file.close();

    // Open the labels file
    input_file.open(label_filename);
    if(!input_file) {
        std::cerr << "Unable to open labels file." << std::endl;
        return;
    }

    // Read the number of vertices in the label file and check that it is the same as before
    int32_t n_vertices_label_file;
    input_file >> n_vertices_label_file;
    assert(n_vertices_label_file == n_vertices);
    
    forward_offsets.reserve(n_vertices + 1);
    backward_offsets.reserve(n_vertices + 1);

    int32_t max_label_size = -1;
    int32_t total_label_size = 0;

    for (int32_t i=0; i<n_vertices; i++)
    {
        int32_t n_labels;
        int32_t hub_vertex_id;
        int32_t hub_distance;

        int32_t current_label_size = 0;

        for (int32_t i=0; i<2; i++)
        {
            std::vector<HubLabel>& hub_labels = (i == 0) ? backward_labels : forward_labels;
            std::vector<int32_t>& offsets = (i == 0) ? backward_offsets : forward_offsets;

            input_file >> n_labels;
            current_label_size += n_labels;
            offsets.push_back(hub_labels.size());

            for (int32_t j=0; j<n_labels; j++)
            {
                input_file >> hub_vertex_id >> hub_distance;
                int32_t actual_hub_vertex_id = order[hub_vertex_id];
                hub_labels.push_back({actual_hub_vertex_id, hub_distance});
            }

            std::sort(hub_labels.begin() + offsets.back(), hub_labels.end(), hubLabelComparator);
        }

        total_label_size += current_label_size;
        max_label_size = std::max(max_label_size, current_label_size);
    }
    forward_offsets.push_back(forward_labels.size());
    backward_offsets.push_back(backward_labels.size());

    std::cout << forward_offsets.size() << backward_offsets.size() << n_vertices << std::endl;
    assert(forward_offsets.size() == n_vertices + 1 && backward_offsets.size() == n_vertices + 1);

    
    auto t2 = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    std::cout << "Read hub label files in " << duration << "ms." << std::endl;
    std::cout << "Average label size: " << (float)total_label_size / n_vertices << std::endl;
    std::cout << "Max label size: " << max_label_size << std::endl;

}


void HubLabelOffsetList::printHubLists()
{
    for (int32_t i=0; i<n_vertices; i++)
    {
        std::cout << "Vertex ID: " << i << "\n";

        int32_t hl_list_size;
        std::vector<HubLabel>::iterator hl_iterator;

        std::cout << "Backward Labels: ";
        hl_iterator = getVertexHubList(&hl_list_size, i, false);
        for (int32_t j=0; j<hl_list_size; j++)
        {
            // std::cout << "(" << hl_iterator[j].vertex_id << "," << hl_iterator[j].distance << ") ";
        }

        std::cout << "\nForward Labels: ";
        hl_iterator = getVertexHubList(&hl_list_size, i, true);
        for (int32_t j=0; j<hl_list_size; j++)
        {
            // std::cout << "(" << hl_iterator[j].vertex_id << "," << hl_iterator[j].distance << ") ";
        }
        // std::cout << "\n\n";
    }
}

void HubLabelOffsetList::printHubList(int32_t vertex_id, bool forward)
{
    std::cout << "Vertex ID: " << vertex_id << "\n";

    int32_t hl_list_size;
    std::vector<HubLabel>::iterator hl_iterator;
    hl_iterator = getVertexHubList(&hl_list_size, vertex_id, false);
    for (int32_t j=0; j<hl_list_size; j++)
    {
        std::cout << "(" << hl_iterator[j].vertex_id << "," << hl_iterator[j].distance << ") ";
    }
    std::cout << "\n\n";
}


std::vector<HubLabel>::iterator HubLabelOffsetList::getVertexHubList(int32_t* hl_list_size, int32_t vertex_id, bool forward)
{
    std::vector<HubLabel>& hub_labels = forward ? forward_labels : backward_labels;
    std::vector<int32_t>& offsets = forward ? forward_offsets : backward_offsets;

    int32_t hl_list_start_index = offsets[vertex_id];
    int32_t hl_list_end_index = offsets[vertex_id + 1];

    if (hl_list_size != NULL)
    {
        *hl_list_size = hl_list_end_index - hl_list_start_index;
    }

    return hub_labels.begin() + hl_list_start_index;
}


double HubLabelOffsetList::getShortestDistance(int32_t source_vertex_id, int32_t target_vertex_id)
{
    int32_t n_source_hubs;
    std::vector<HubLabel>::iterator source_hubs = getVertexHubList(&n_source_hubs, source_vertex_id, true);
    std::vector<HubLabel>::iterator source_end = source_hubs + n_source_hubs;

    int32_t n_target_hubs;
    std::vector<HubLabel>::iterator target_hubs = getVertexHubList(&n_target_hubs, target_vertex_id, false);
    std::vector<HubLabel>::iterator target_end = target_hubs + n_target_hubs;

    auto source_it = source_hubs;
    auto target_it = target_hubs;

    int32_t n_hubs_checked = 2;

    int32_t min_distance = std::numeric_limits<int32_t>::max();

    while (true)
    {
        // Bounds check
        if (source_it >= source_hubs + n_source_hubs || target_it >= target_hubs + n_target_hubs)
        {
            break;
        }
        if (source_it->vertex_id == target_it->vertex_id)
        {
            min_distance = std::min(min_distance, source_it->distance + target_it->distance);
            source_it++;
            target_it++;
        }
        else if (source_it->vertex_id < target_it->vertex_id)
        {
            source_it++;
        }
        else
        {
            target_it++;
        }
        n_hubs_checked++;
    }

    if (min_distance == std::numeric_limits<int32_t>::max())
    {
        throw "No common hub label found for the two vertices.";
    }

    // TODO: Add timing for query
    // std::cout << "Distance Query: " << n_hubs_checked << " hub labels checked." << '\n';

    // return (double)(source_it->distance + target_it->distance);
    return EDGE_LENGTH_MULTIPLIER * (double)min_distance;
}

/*
Binary Format:
n_vertices -> (1) * int32_t
backward_offsets -> (n_vertices + 1) * int32_t
forward_offsets -> (n_vertices + 1) * int32_t
backward_labels -> (backward_offsets[-1]) * sizeof(HubLabel)
forward_labels -> (forward_offsets[-1]) * sizeof(HubLabel)
*/
void HubLabelOffsetList::writeHubLabelsBinary(std::string target_filename)
{
    auto t1 = chrono::high_resolution_clock::now();

    std::ofstream output_file;
    output_file.open(target_filename, ios::binary);

    output_file.write((char*)(&n_vertices), sizeof(int32_t));
    output_file.write((char*)backward_offsets.data(), sizeof(int32_t) * backward_offsets.size());
    output_file.write((char*)forward_offsets.data(), sizeof(int32_t) * forward_offsets.size());
    output_file.write((char*)backward_labels.data(), sizeof(HubLabel) * backward_offsets.back());
    output_file.write((char*)forward_labels.data(), sizeof(HubLabel) * forward_offsets.back());

    output_file.close();

    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "Wrote binary hub labels file in " << duration << "ms" << std::endl; 
}


void HubLabelOffsetList::importHubLabelsBinary(std::string input_filename)
{
    auto t1 = chrono::high_resolution_clock::now();
    
    std::ifstream input_file;
    input_file.open(input_filename, ios::binary);

    input_file.read((char*)(&n_vertices), sizeof(int32_t));

    backward_offsets.resize(n_vertices + 1);
    forward_offsets.resize(n_vertices + 1);

    input_file.read((char*)backward_offsets.data(), sizeof(int32_t) * (n_vertices + 1));
    input_file.read((char*)forward_offsets.data(), sizeof(int32_t) * (n_vertices + 1));

    uint32_t backward_labels_size = backward_offsets.back();
    uint32_t forward_labels_size = forward_offsets.back();

    backward_labels.resize(backward_labels_size);
    forward_labels.resize(forward_labels_size);

    input_file.read((char*)backward_labels.data(), sizeof(HubLabel) * backward_labels_size);
    input_file.read((char*)forward_labels.data(), sizeof(HubLabel) * forward_labels_size);

    input_file.close();

    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "Read binary hub labels file in " << duration << "ms" << std::endl; 
}


void HubLabelOffsetList::sortByDistance()
{
    auto t1 = chrono::high_resolution_clock::now();

    for (int32_t i=0; i<n_vertices; i++)
    {
        std::sort(backward_labels.begin()+backward_offsets[i], backward_labels.begin()+backward_offsets[i+1], hubLabelDistanceComparator);

        std::sort(forward_labels.begin()+forward_offsets[i], forward_labels.begin()+forward_offsets[i+1], hubLabelDistanceComparator);
        

        // std::vector<HubLabel>::iterator start_iterator;
        // std::vector<HubLabel>::iterator end_iterator;

        // start_iterator = getVertexHubList(NULL, i, false);
        // if (i == n_vertices-1)
        // {
        //     end_iterator = backward_labels.end();
        // }
        // else {
        //     end_iterator = getVertexHubList(NULL, i+1, false);
        // }
        // std::sort(start_iterator, end_iterator, hubLabelDistanceComparator);

        // start_iterator = getVertexHubList(NULL, i, true);
        // if (i == n_vertices-1)
        // {
        //     end_iterator = forward_labels.end();
        // }
        // else {
        //     end_iterator = getVertexHubList(NULL, i+1, true);
        // }
        // std::sort(start_iterator, end_iterator, hubLabelDistanceComparator);
    }

    printHubList(0, true);

    auto t2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "Sorted hub label lists in " << duration << "ms" << std::endl; 
}


void HubLabelOffsetList::writeDistances()
{
    std::ofstream output_file;
    output_file.open("hub_distances.dat");

    for (int32_t i=0; i<1000; i++)
    {
        int32_t vertex = std::rand() % n_vertices;

        int32_t hl_size;
        auto it = getVertexHubList(&hl_size, vertex, true);

        for (int j=0; j<hl_size; j++, it++)
        {
            output_file << it->distance * EDGE_LENGTH_MULTIPLIER << '\n';
        }
    }

    output_file.close();
}


// int main(int32_t argc, char** argv)
// {
//     string label_filename = argv[1];
//     string order_filename = argv[2];

//     HubLabelOffsetList hl;
//     // hl.importHubLabels(label_filename, order_filename

//     // hl.printHubLists();

//     // hl.writeHubLabelsBinary("saarland_x100_binary");

//     hl.importHubLabelsBinary("saarland_x100_binary");

//     hl.writeDistances();

//     // hl.printHubLists();

//     double dist = hl.getShortestDistance(244, 578973);
//     std::cout << dist << std::endl;

//     dist = hl.getShortestDistance(0, 3);
//     std::cout << dist << std::endl;

//     hl.printHubList(244, true);
//     hl.printHubList(578973, false);

//     return 0;
// }