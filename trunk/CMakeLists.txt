cmake_minimum_required(VERSION 2.6)
project(texpp)

enable_testing()

#set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.34.0 REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
find_package(BoostPython)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(texpp)
add_subdirectory(tests)

if(BOOST_PYTHON_FOUND)
    add_subdirectory(texpy)
endif(BOOST_PYTHON_FOUND)

