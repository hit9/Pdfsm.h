cmake_minimum_required(VERSION 3.10)

project(PdfsmTests)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

include_directories("../Source")

find_package(Catch2 CONFIG REQUIRED)

# Targets
file(GLOB TEST_SOURCES *.cpp)
add_executable(PdfsmTests ${TEST_SOURCES})

target_link_libraries(PdfsmTests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(PdfsmTests)
