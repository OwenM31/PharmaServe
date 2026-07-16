cmake_minimum_required(VERSION 3.20)

project(PharmaServe)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pharmaserve 
	PharmaServe/Order.h
	PharmaServe/OrderQueue.h
	PharmaServe/OrderQueue.cpp
	PharmaServe/PharmaServe.cpp
)

target_include_directories(pharmaserve PRIVATE PharmaServe)

enable_testing()

add_subdirectory(tests)
