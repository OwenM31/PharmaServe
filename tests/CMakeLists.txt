include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(pharmaserve_tests
	OrderQueueTests.cpp
	../PharmaServe/OrderQueue.cpp
)

target_link_libraries(pharmaserve_tests
	GTest::gtest_main
)

add_test(NAME PharmaServeTests COMMAND pharmaserve_tests)