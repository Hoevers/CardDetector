cmake_minimum_required(VERSION 2.8)

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "DEBUG" ) 
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG" )
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99" )

#set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}"
#"/Volumes/Downloads/opencv-2.4.2-release/share/OpenCV")
#"/Users/jerry/Macports/share/cmake-2.8/Modules")

#set(ENV{PKG_CONFIG_PATH} "/Users/jerry/Macports/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(OpenCV REQUIRED opencv)

# @TODO Ugly hack for now @TODO
project( example )
link_directories( ${OpenCV_LIBRARY_DIRS} )

add_executable( example example.c cards.c )
target_link_libraries( example ${OpenCV_LDFLAGS} )

#add_executable( match match.cpp )
#target_link_libraries( match ${OpenCV_LDFLAGS} )

message( STATUS "OpenCV found: ${OpenCV_FOUND}" )
message( STATUS "OpenCV version: ${OpenCV_VERSION}" )
message( STATUS "OpenCV libraries: ${OpenCV_LDFLAGS}" )

include_directories(
	${CMAKE_INCLUDE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}
	${OpenCV_INCLUDE_DIRS}
)

#message( STATUS ${OpenCV_LIBS} )
#message( STATUS ${OpenCV_INCLUDE_DIRS} )
