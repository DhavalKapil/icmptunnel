cmake_minimum_required(VERSION 2.8.3)
project(icmptunnel)
include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(HEADER_FILES include/icmp.h include/tunnel.h)

add_library(icmp src/icmp.c ${HEADER_FILES})
add_library(tunnel src/tunnel.c ${HEADER_FILES})

add_executable(icmptunnel src/icmptunnel.c ${HEADER_FILES})
add_executable(test_server src/test_server.c ${HEADER_FILES})
add_executable(test_client src/test_client.c ${HEADER_FILES})

target_link_libraries(tunnel icmp)
target_link_libraries(icmptunnel icmp tunnel)
target_link_libraries(test_server icmp)
target_link_libraries(test_client icmp)
