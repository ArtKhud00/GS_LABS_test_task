cmake_minimum_required(VERSION 3.16)
project(TestTask)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)
set(JSON json.cpp json.h)
set(CONFIG_FILE configuration_parse.cpp configuration_parse.h)

add_executable(TestTask ${SOURCE_FILES} LoadBalancer.cpp LoadBalancer.h
                        WeightedRoundRobin.cpp WeightedRoundRobin.h
                        ${JSON} ${CONFIG_FILE})