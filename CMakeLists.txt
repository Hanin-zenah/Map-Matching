cmake_minimum_required(VERSION 3.19)

project(sp-geo-map-matching)

add_executable(preprocess preprocessing/scale_projection.cpp preprocessing/graph.cpp preprocessing/main_preprocessing.cpp)
set_property(TARGET preprocess PROPERTY CXX_STANDARD 17)
