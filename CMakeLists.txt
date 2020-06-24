cmake_minimum_required(VERSION 3.10)
project(stereo)

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Boost 1.45.0 COMPONENTS filesystem system iostreams )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

ADD_LIBRARY( cimg SHARED src/CImg/CImg.hpp)

if(Boost_FOUND)
    add_executable(stereo src/main.cc)
    set_target_properties(cimg PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(stereo cimg)
    target_link_libraries(stereo Boost::filesystem Boost::system Boost::iostreams util pthread X11)
endif()
