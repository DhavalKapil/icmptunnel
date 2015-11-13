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

# Doxygen documentation 
option(DOCUMENTATION "Generate API documentation with Doxygen" ON)
find_package(Doxygen)
if(DOCUMENTATION AND DOXYGEN_FOUND)
   # Set variables
   set(PACKAGE libftdi)
   set(VERSION ${VERSION_STRING})
   set(top_srcdir ${CMAKE_SOURCE_DIR})
   # Find doxy config
   message(STATUS "Doxygen found.")
   set(DOXY_DIR "${CMAKE_SOURCE_DIR}/doc")
   set(DOXY_CONFIG "${DOXY_DIR}/Doxyfile.in")
   # Copy doxy.config.in
   configure_file("${DOXY_CONFIG}" "${CMAKE_BINARY_DIR}/doxy.config")
   # Create doc directory
   add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/doc
   COMMAND rm -rf ${CMAKE_BINARY_DIR}/doc/{html,man}
   COMMAND mkdir -p ${CMAKE_BINARY_DIR}/doc
   # DEPENDS ftdi ftdipp
   )
   # Run doxygen
   add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/doc/html/index.html
   COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/doxy.config"
   DEPENDS "${CMAKE_BINARY_DIR}/doxy.config" "${CMAKE_BINARY_DIR}/doc"
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc
   )
   add_custom_target(docs ALL DEPENDS ${CMAKE_BINARY_DIR}/doc/html/index.html)
   message(STATUS "Generating API documentation with Doxygen")
else(DOCUMENTATION AND DOXYGEN_FOUND)
   message(STATUS "Not generating API documentation")
endif(DOCUMENTATION AND DOXYGEN_FOUND)