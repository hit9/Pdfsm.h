cmake_minimum_required(VERSION 3.10)

project(pdfsm)
set(CMAKE_CXX_STANDARD 20)

add_library(pdfsm INTERFACE)
set_target_properties(pdfsm PROPERTIES PUBLIC_HEADER "pdfsm.h")

install(
  TARGETS pdfsm
  LIBRARY DESTINATION ${CMAK_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
