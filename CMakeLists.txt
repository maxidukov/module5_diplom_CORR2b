cmake_minimum_required(VERSION 3.5)

project(module5_diplom_CORR2b_QC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(module5_diplom_CORR2b_QC main.cpp)

include(GNUInstallDirs)
install(TARGETS module5_diplom_CORR2b_QC
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
