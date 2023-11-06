cmake_minimum_required(VERSION 3.10)
project(rm_serial_port)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

add_executable(rm_serial_port src/test_encode_decode.cpp)

target_include_directories(rm_serial_port PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(rm_serial_port PUBLIC include)

target_link_libraries(rm_serial_port ${Boost_LIBRARIES})
target_link_libraries(rm_serial_port pthread)
