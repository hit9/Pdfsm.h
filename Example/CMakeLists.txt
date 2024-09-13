cmake_minimum_required(VERSION 3.10)
project(pdfsm_example CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories("../Source")

# Targets
add_executable(PdfsmExample main.cpp)
