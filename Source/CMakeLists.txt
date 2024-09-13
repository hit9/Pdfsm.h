cmake_minimum_required(VERSION 3.10)

project(Pdfsm)
set(CMAKE_CXX_STANDARD 20)

add_library(Pdfsm INTERFACE)
set_target_properties(Pdfsm PROPERTIES PUBLIC_HEADER "pdfsm.h")

install(
  TARGETS Pdfsm
  LIBRARY DESTINATION ${CMAK_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
