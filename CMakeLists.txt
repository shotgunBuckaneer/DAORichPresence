cmake_minimum_required(VERSION 3.21)

project(DAORichPresence VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(extern/DAOLib EXCLUDE_FROM_ALL)

add_daolib_plugin(richpresence SOURCES src/richpresence.cpp)