cmake_minimum_required (VERSION 2.8)
project (CreateVideo)
# The version number.
set (CreateVideo_VERSION_MAJOR 1)
set (CreateVideo_VERSION_MINOR 0)
set (PYLON_ROOT /opt/pylon5)
execute_process (COMMAND ${PYLON_ROOT}/bin/pylon-config --cflags OUTPUT_VARIABLE  PYLON_COMPILE_TMP)
string (STRIP ${PYLON_COMPILE_TMP} PYLON_COMPILE)
execute_process (COMMAND ${PYLON_ROOT}/bin/pylon-config --libs OUTPUT_VARIABLE PYLON_LINK_TMP)
string (STRIP ${PYLON_LINK_TMP} PYLON_LINK)
execute_process (COMMAND ${PYLON_ROOT}/bin/pylon-config --libs-rpath OUTPUT_VARIABLE PYLON_LINK_TMP)
string (STRIP ${PYLON_LINK_TMP} PYLON_RPATH)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/CreateVideoConfig.h.in"
  "${PROJECT_BINARY_DIR}/CreateVideoConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find CreateVideoConfig.h
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

FIND_PACKAGE( OpenMP )
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PYLON_COMPILE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PYLON_COMPILE}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PYLON_LINK}")

message("Pylonroot is ${PYLON_ROOT}")
message("CFLAGS is ${CMAKE_C_FLAGS}")
message("CXXFLAGS is ${CMAKE_CXX_FLAGS}")
message("LDFLAGS is ${CMAKE_EXE_LINKER_FLAGS}")


FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )

SET(Pylon_LIBS 
		pylonbase
		pylonutility
		GenApi_gcc_v3_0_Basler_pylon_v5_0
		GCBase_gcc_v3_0_Basler_pylon_v5_0
		Log_gcc_v3_0_Basler_pylon_v5_0
		MathParser_gcc_v3_0_Basler_pylon_v5_0
		#Xerces-C_gcc40_v2_7_1
		log4cpp_gcc_v3_0_Basler_pylon_v5_0
)

add_executable(CreateVideo createvideo.cpp )

target_link_libraries (
	CreateVideo
		${OpenCV_LIBS}
		${Pylon_LIBS}
		${PYLON_LINK}
		${PYLON_RPATH}
	)
