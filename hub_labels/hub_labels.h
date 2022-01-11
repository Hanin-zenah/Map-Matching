#ifndef HUB_LABELS_H
#define HUB_LABELS_H

#include <vector>
#include <string>
#include <assert.h>
#include <iostream> 
#include <fstream> 
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <iterator>
#include <limits>

// Check file reading time
// Print average and max label sizes
// Print distance query times, how many labels checked
// Sort hl lists
// interleaved: check if a vertex has a HL list and use it for distance query if list exists
// issue: hl distance outputs are integers -> change to ints
// gnuplot

// Allocate larger buffer for IO


using namespace std;

struct HubLabel
{
    int32_t vertex_id;
    int32_t distance;

    friend std::ostream& operator<<(std::ostream &output, const HubLabel &h) { 
         output << h.vertex_id << h.distance;
         return output;            
    }

    friend std::istream& operator>>(std::istream &input, HubLabel &h) { 
         input >> h.vertex_id >> h.distance;
         return input;
    }
};

struct HubLabelOffsetList
{
    // Rounds edge length precisions to 10^n in metres, where n is the edge length precision
    // const int32_t edge_length_precision = -2;
    const double EDGE_LENGTH_MULTIPLIER = 0.001;

    std::vector<HubLabel> forward_labels;
    std::vector<HubLabel> backward_labels;

    int32_t n_vertices;

    // The offsets list defines the starting index for each vertex's hub label list.
    // Each offset list should have length n_vertices + 1, as an extra offset value is
    // added to the end of the vector.
    std::vector<int32_t> forward_offsets;
    std::vector<int32_t> backward_offsets;
    
    // Reads in hub label data from a label file and an order file to construct the offset list.
    void importHubLabels(std::string label_filename, std::string order_filename);

    // Returns the forwards or backwards hub label list for a particular vertex.
    std::vector<HubLabel>::iterator getVertexHubList(int32_t* hl_list_size, int32_t vertex_id, bool forward);

    // Returns the shortest path distance from a source vertex to a target vertex.
    double getShortestDistance(int32_t source_vertex_id, int32_t target_vertex_id);

    void printHubLists();

    void printHubList(int32_t vertex_id, bool forward);

    void writeHubLabelsBinary(std::string target_filename);

    void importHubLabelsBinary(std::string input_filename);

    static bool hubLabelComparator(HubLabel h1, HubLabel h2) { return (h1.vertex_id < h2.vertex_id); }

    static bool hubLabelDistanceComparator(HubLabel h1, HubLabel h2) { return (h1.distance < h2.distance); }

    void sortByDistance();

    void writeDistances();
};

#endif