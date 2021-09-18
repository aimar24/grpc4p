cmake_minimum_required(VERSION 3.0.0)
project(grpcPlusPromise VERSION 0.1.0)

include(CTest)
enable_testing()


include_directories(
    include/grpcPlusPromise
    3rdparty/Promise/include)
add_library(grpcPlusPromise src/grpcPlusPromise.cpp)

target_compile_features(grpcPlusPromise PRIVATE cxx_std_17)

add_subdirectory("3rdparty/Promise")

target_link_libraries(grpcPlusPromise
                      Promise )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
