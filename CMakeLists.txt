cmake_minimum_required(VERSION 3.20)

project(PharmaServe)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pharmaserve
	PharmaServe/PharmaServe.cpp
	PharmaServe/OrderQueue/Order.h
	PharmaServe/OrderQueue/OrderQueue.h
	PharmaServe/OrderQueue/OrderQueue.cpp
	PharmaServe/WorkerManager/WorkerManager.cpp
	PharmaServe/WorkerManager/WorkerStatus.cpp
)

target_include_directories(pharmaserve PRIVATE PharmaServe)

enable_testing()

add_subdirectory(tests)
