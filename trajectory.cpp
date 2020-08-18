//read the binary trajectory files to extract the polyline (sampled trajectory) 
// #include "graph.h"
#include "trajectory.h"
#include "scale_projection.h"

void add_point(Trajectory* traj, double longitude, double latitude, int timestamp) {
    Point* cur = traj -> head;
    if(cur == NULL) {
        //first ever point being added 
        traj -> head = (Point*) malloc(sizeof(Point));
        traj -> head -> longitude = longitude;
        traj -> head -> latitude = latitude;
        traj -> head -> timestamp = timestamp;
        traj -> tail = traj -> head;
        return;
    }

    while(cur -> next != NULL) {
        cur = cur -> next;
    }
    cur -> next = (Point*) malloc(sizeof(Point));
    cur -> next -> longitude = longitude;
    cur -> next -> latitude = latitude;
    cur -> next -> timestamp = timestamp;
    traj -> tail = cur -> next;
    return;
}

void read_next_k_bytes(ifstream& file, char* buffer, int k) {
    if(!file.eof()) {
        //read from file if we havent reached end of file 
        memset(buffer, 0, k);
        file.read(buffer, k);
    }
} 

// trajecctory.binTracks file: 

// nPoints traceId subId
// lat lon timestamp
// ...
// lat lon timestamp
// nPoints traceId subId
// ...
void extract_next_trajectory(ifstream& file, int offset, Trajectory* traj, double min_long, double min_lat) {
    file.seekg(offset, ios::beg);
    if(file.eof()) {
        return;
    }

    char buffer[4];

    //get the number of sampled points in the trajectory
    read_next_k_bytes(file, buffer, TRAJ_VAL_SIZE);
    traj -> length = *(int*)buffer;

    //get the trace id 
    read_next_k_bytes(file, buffer, TRAJ_VAL_SIZE);
    traj -> traceId = *(uint32_t*)buffer;

    //get the sub id 
    read_next_k_bytes(file, buffer, TRAJ_VAL_SIZE);
    traj -> subId = *(uint32_t*)buffer;

    //get the points 
    double longitude;
    double latitude; 
    int timestamp; 
    Euc_distance ed;

    for(int i = 0; i < traj -> length; i++) {
        read_next_k_bytes(file, buffer, TRAJ_VAL_SIZE);
        longitude = *(int*)buffer;
        longitude /= pow(10, LON_LAT_COMMA_SHIFT);
        /* overwrite the node's longitude in mercator projection */
        longitude = ed.lon_mercator_proj(longitude, min_long);

        read_next_k_bytes(file, buffer, TRAJ_VAL_SIZE);
        latitude = *(int*)buffer;
        latitude /= pow(10, LON_LAT_COMMA_SHIFT);
        /* overwrite the node's latitude in mercator projection */
        latitude = ed.lon_mercator_proj(latitude, min_lat);

        read_next_k_bytes(file, buffer, TRAJ_VAL_SIZE);
        timestamp = *(int*)buffer;

        add_point(traj, longitude, latitude, timestamp);
    }
}

vector<Trajectory> read_trajectories(string file_path, int k) { //extract k trajectories?  //will figure it out later 
    ifstream file;
    file.open(file_path, ios::in | ios::binary);
    file.seekg(0, ios::beg);

    Trajectory traj = DEF_TRAJ;
    vector<Trajectory> trajs;
    for(int i = 0; i < k; i++) {
        if(!file.eof()) {
            int offset = file.tellg();
            extract_next_trajectory(file, offset, &traj);
            trajs.push_back(traj);
            traj = DEF_TRAJ;
        }
        else {
            return trajs;
        }
    }
    file.close();
    return trajs;
}


int main() {
    vector<Trajectory> trajs = read_trajectories("trajectories/saarland-geq50m-clean-unmerged-2016-10-09-saarland.binTracks", 1);
    Point* cur = trajs[0].head;
    cout << trajs[0].length << endl;
    while(cur != NULL) {
        cout << cur -> longitude << " " << cur -> latitude << endl;
        cur = cur -> next;
    }
    cur = trajs[0].head;
    Point* next;
    while(cur != NULL) {
        next = cur -> next;
        free(cur);
        cur = next;
    }
    return 0;
}