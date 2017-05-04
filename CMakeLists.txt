cmake_minimum_required(VERSION 3.7)
project(run)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED thread system)
if (Boost_FOUND)
    message( STATUS "Boost include was found in ${Boost_INCLUDE_DIR} " )
    message( STATUS "boostlibs: ${Boost_LIBRARIES}" )
    message( STATUS "boost thread    : ${Boost_THREAD_LIBRARY}" )
    message( STATUS "boost system    : ${Boost_SYSTEM_LIBRARY}" )
else()
    message( ERROR "Could not find Boost" )
endif()

include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)

add_executable(run ${SOURCE_FILES})

target_link_libraries(run
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        )