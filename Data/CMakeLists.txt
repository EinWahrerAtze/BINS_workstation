cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)
project ("BINS_workstation")
add_executable (BINS_workstation  main.cpp file.h file.cpp
								           collection.h collection.cpp
										   interface.h interface.cpp
                                           logger.h logger.cpp
                                           utility.h)