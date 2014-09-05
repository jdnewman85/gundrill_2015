#Project
project(GUNDRILL)

cmake_minimum_required(VERSION 2.6)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)

include_directories(${GLIB_PKG_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS} ${GUNDRILL_SOURCE_DIR}/include)
link_directories(${GLIB_PKG_LIBRARY_DIRS} ${GTK3_LIBRARY_DIRS} ${GUNDRILL_BINARY_DIR}/lib)
add_definitions(${GTK_CFLAGS_OTHER})

#SimpleMotion
file(GLOB SIMPLEMOTION_SOURCES ${GUNDRILL_SOURCE_DIR}/lib/SimpleMotion/*.c)
add_library(SimpleMotion ${SIMPLEMOTION_SOURCES})

file(GLOB SOURCES ${GUNDRILL_SOURCE_DIR}/src/*.c)

add_executable(a.out ${SOURCES})

target_link_libraries(a.out SimpleMotion ftd2xx ${GLIB_PKG_LIBRARIES} ${GTK3_LIBRARIES})

