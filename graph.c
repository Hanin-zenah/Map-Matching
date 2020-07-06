#include "graph_c.h"

//c is the fastest way to read files of large data 



/* checks and updates the boundary box measures if necessary */
void check_boundaries(double latitude, double longitude, Graph* g) {
    if(g -> max_lat <= latitude) {
        g -> max_lat = latitude;
    }
    if( g -> min_lat >= latitude) {
        g -> min_lat = latitude;
    }
    if(g -> max_long <= longitude) {
        g -> max_long = longitude;
    }
    if( g -> min_long >= longitude) {
        g -> min_long = longitude;
    }
}


/* given a file name, open the file and read the data line by line into */
Graph* read_file(const char* file_name) {
    if(file_name == NULL) {
        return NULL;
    }
    FILE* file = fopen(file_name, "r");
    if(!file) {
        perror("COULD NOT OPEN FILE");
        return NULL;
    }
    Graph graph = {
        0, 0, NULL, NULL, INT_MAX, INT_MIN, INT_MAX, INT_MIN
    };

    char buffer[MAX_BUFF];
    memset(buffer, 0, MAX_BUFF);

    //skip the first five lines
    for(int i = 0; i < IGNORE_LINES; i++) {
        fgets(buffer, MAX_BUFF, file);
    }

    //read the total number of nodes and edges, store them in graph struct 
    fscanf(file, "%d", &graph.n_nodes);
    fscanf(file, "%d", &graph.n_edges);

    //malloc the required amount of memory 
    graph.nodes = malloc(sizeof(struct node) * graph.n_nodes);
    graph.edges = malloc(sizeof(struct edge) * graph.n_edges);

    //now read everything
    //read line into buffer, scan line number, osmid, lat, long, .. (keep what matters)
    int line_num;
    for(int i = 0; i <= graph.n_nodes; i++) {
        memset(buffer, 0, MAX_BUFF);
        fgets(buffer, MAX_BUFF, file);
        sscanf(buffer, "%d %lld %lf %lf", &line_num, &graph.nodes[i].osmid, &graph.nodes[i].lat, &graph.nodes[i].longitude);
        // printf("Node: %llu\n", graph.nodes[i].osmid);
        check_boundaries(graph.nodes[i].lat, graph.nodes[i].longitude, &graph);
    }
    fclose(file);
    printf("Max latitude: %lf\n Max longitude: %lf\n Min latitude: %lf\n Min longitude; %lf\n", graph.max_lat, graph.max_long, graph.min_lat, graph.min_long);
    return NULL;

}

int main() {
    read_file("saarland-200601.car.txt");
    return 0;
}