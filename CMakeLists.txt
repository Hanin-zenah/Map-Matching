cmake_minimum_required(VERSION 3.19)

project(sp-geo-map-matching)

add_executable(main preprocessing/scale_projection.cpp preprocessing/graph.cpp preprocessing/main_preprocessing.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 11)
