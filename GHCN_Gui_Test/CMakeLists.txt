cmake_minimum_required(VERSION 3.5)

project(GHCN_Gui_Test LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GHCN_Gui_Test test_main.cpp
    ../GHCN_Gui/ghcn_dataprovider.hpp   ../GHCN_Gui/station.cpp ../GHCN_Gui/station.hpp
    ../GHCN_Gui/measurement.hpp
    ../GHCN_Gui/measurement.cpp
)
add_test(NAME GHCN_Gui_Test COMMAND GHCN_Gui_Test)

set(BOOST_INCLUDE_DIR $ENV{BOOST_INCLUDE_DIR})

if (BOOST_INCLUDE_DIR STREQUAL "")
    message("BOOST_INCLUDE_DIR is not set, assuming Boost can be found automatically in your system")
elseif (EXISTS ${BOOST_INCLUDE_DIR})
    include_directories(${BOOST_INCLUDE_DIR} "../GHCN_Gui/")
endif ()
