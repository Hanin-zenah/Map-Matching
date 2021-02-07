cmake_minimum_required(VERSION 3.19)

project(sp-geo-map-matching)

add_executable(preprocess preprocessing/scale_projection.cpp preprocessing/graph.cpp main_preprocessing.cpp)
set_property(TARGET preprocess PROPERTY CXX_STANDARD 17)

add_executable(mapmatching preprocessing/graph.cpp grid/graph_grid.cpp preprocessing/scale_projection.cpp trajectories/trajectory.cpp trajectories/trajectory_split.cpp grid/graph_grid_starting_node.cpp HMM/HMM.cpp main_HMM.cpp)
set_property(TARGET mapmatching PROPERTY CXX_STANDARD 17)