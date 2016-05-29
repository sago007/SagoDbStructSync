cmake_minimum_required(VERSION 2.8.9)
project (dbsync)

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

include_directories("include")

find_package(Boost COMPONENTS program_options REQUIRED)
add_executable(dbsync src/dbsync.cpp src/sago/dbsync.o src/sago/dbsync_db.o src/sago/DbSyncDbOracle.o src/sago/DbSyncDbMySql.o)
TARGET_LINK_LIBRARIES( dbsync ${Boost_LIBRARIES} cppdb )
