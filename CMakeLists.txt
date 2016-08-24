cmake_minimum_required(VERSION 2.8.9)
project (dbsync)

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

include_directories("include")

find_package(Boost COMPONENTS program_options system filesystem REQUIRED)
add_library(SagoDbStructSync src/sago/dbsync.cpp src/sago/dbsync_db.cpp src/sago/DbSyncDbOracle.cpp src/sago/DbSyncDbMySql.cpp)
add_executable(dbsync src/dbsync.cpp)
TARGET_LINK_LIBRARIES( dbsync SagoDbStructSync ${Boost_PROGRAM_OPTIONS_LIBRARY} cppdb )

enable_testing()
add_subdirectory (test)