cmake_minimum_required(VERSION 2.6)
project(texpp)

include(CTest)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.34.0 REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
find_package(BoostPython)

find_program(TEX_EXECUTABLE "tex")
if(TEX_EXECUTABLE STREQUAL "TEX_EXECUTABLE-NOTFOUND")
    message("tex executable NOT found - tests/tex will NOT be build.")
else(TEX_EXECUTABLE STREQUAL "TEX_EXECUTABLE-NOTFOUND")
    set(TEX_FOUND ON)
endif(TEX_EXECUTABLE STREQUAL "TEX_EXECUTABLE-NOTFOUND")
mark_as_advanced(TEX_EXECUTABLE)

if(NOT BOOST_PYTHON_FOUND)
    message("boost.python NOT found - texpy will NOT be build.")
endif(NOT BOOST_PYTHON_FOUND)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wunused -Woverloaded-virtual -Wwrite-strings")
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(texpp)

if(BOOST_PYTHON_FOUND)
    add_subdirectory(texpy)
    add_subdirectory(hrefkeywords)
endif(BOOST_PYTHON_FOUND)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif(BUILD_TESTING)

