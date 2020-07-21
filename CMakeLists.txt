cmake_minimum_required(VERSION 2.4)

project(sp-geo-map-matching)

add_executable(main scale_projection.cpp graph.cpp main.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 11)
