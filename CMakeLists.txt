cmake_minimum_required (VERSION 2.8)
project (CreateVideo)
# The version number.
set (PylonImage_VERSION_MAJOR 1)
set (PylonImage_VERSION_MINOR 0)
set (PYLON_ROOT /opt/pylon5)
set (GENICAM_ROOT ${PYLON_ROOT}/genicam)
add_definitions(-DUSE_GIGE)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/PylonImageConfig.h.in"
  "${PROJECT_BINARY_DIR}/PylonImageConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find PylonImageConfig.h
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

FIND_PACKAGE( OpenMP )
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

message("Pylonroot is ${PYLON_ROOT}")

INCLUDE_DIRECTORIES (
	"${PROJECT_SOURCE_DIR}/src"
	"${GENICAM_ROOT}/library/CPP/include"
	"${PYLON_ROOT}/include"
	)
LINK_DIRECTORIES(
	"${PYLON_ROOT}/lib64"
	"${GENICAM_ROOT}/bin/Linux64_x64"
	"${GENICAM_ROOT}/bin/Linux64_x64/GenApi/Generic"
	)

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )

add_executable(CreateVideo createvideo.cpp)
target_link_libraries (
	CreateVideo
		pylonbase
		${OpenCV_LIBS}
		pylonutility
		GenApi_gcc40_v2_3
		GCBase_gcc40_v2_3
		Log_gcc40_v2_3
		MathParser_gcc40_v2_3
		Xerces-C_gcc40_v2_7_1
		log4cpp_gcc40_v2_3
	)
