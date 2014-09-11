#Project
project(GUNDRILL)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)

set(PIFACE_LIBRARY_DIRS ${CMAKE_SOURCE_DIR}/lib/libmcp23s17 ${CMAKE_SOURCE_DIR}/lib/libpifacedigital)
#set(PIFACE_LIBRARIES pifacedigital mcp23s17)

include_directories(${GLIB_PKG_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS} ${GUNDRILL_SOURCE_DIR}/include)
link_directories(${GLIB_PKG_LIBRARY_DIRS} ${GTK3_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/lib ${PIFACE_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

#SimpleMotion
file(GLOB SIMPLEMOTION_SOURCES ${GUNDRILL_SOURCE_DIR}/lib/SimpleMotion/*.c)
add_library(SimpleMotion ${SIMPLEMOTION_SOURCES})

file(GLOB SOURCES ${GUNDRILL_SOURCE_DIR}/src/*.c)

add_executable(gunDrill ${SOURCES})

target_link_libraries(gunDrill SimpleMotion ftd2xx ${GLIB_PKG_LIBRARIES} ${GTK3_LIBRARIES} ${PIFACE_LIBRARIES})

