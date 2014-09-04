#Project
project(GUNDRILL)

cmake_minimum_required(VERSION 2.6)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS} ${GUNDRILL_SOURCE_DIR}/include)
link_directories(${GTK3_LIBRARY_DIRS} ${GUNDRILL_BINARY_DIR}/lib)
add_definitions(${GTK_CFLAGS_OTHER})

#SimpleMotion
file(GLOB SIMPLEMOTION_SOURCES ${GUNDRILL_SOURCE_DIR}/lib/SimpleMotion/*.c)
add_library(SimpleMotion ${SIMPLEMOTION_SOURCES})

add_executable(a.out ${GUNDRILL_SOURCE_DIR}/src/main.c)

target_link_libraries(a.out SimpleMotion ftd2xx ${GTK3_LIBRARIES})

