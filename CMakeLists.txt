#Project

cmake_minimum_required(VERSION 2.6)

project(GUNDRILL)

include_directories(${GUNDRILL_SOURCE_DIR}/include)
link_directories(${GUNDRILL_BINARY_DIR}/lib)

add_subdirectory(SimpleMotion_lib)

target_link_libraries(a.out GUNDRILL)
