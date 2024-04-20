cmake_minimum_required(VERSION 3.10)

project(pdfsm)

option(PDFSM_TEST "Enables testing" OFF)
set(CMAKE_CXX_STANDARD 20)

add_library(pdfsm INTERFACE)
set_target_properties(pdfsm PROPERTIES PUBLIC_HEADER "pdfsm.h")

if(PDFSM_TEST)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
    add_subdirectory(example EXCLUDE_FROM_ALL)
endif()

install(
    TARGETS pdfsm
    LIBRARY DESTINATION ${CMAK_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
