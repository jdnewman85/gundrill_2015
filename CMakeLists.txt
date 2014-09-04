#Project

cmake_minimum_required(VERSION 2.6)

project(GUNDRILL)

include_directories(${GUNDRILL_SOURCE_DIR}/include)
link_directories(${GUNDRILL_BINARY_DIR}/lib)

#SimpleMotion
file(GLOB SIMPLEMOTION_SOURCES ${GUNDRILL_SOURCE_DIR}/lib/SimpleMotion/*.c)
add_library(SimpleMotion ${SIMPLEMOTION_SOURCES})

add_executable(a.out ${GUNDRILL_SOURCE_DIR}/src/main_verysimple.c)

target_link_libraries(a.out SimpleMotion ftd2xx)

