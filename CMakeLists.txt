project(FISHCAT)

cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(MAKE_SHARED_LIBRARIES "SHARED")
    message(STATUS "All libraries are built to shared libraries in the Release mode.")
else()
    set(MAKE_SHARED_LIBRARIES "")
    message(STATUS "All libraries are built to static libraries in the Debug mode.")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set outputs file path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# #####################################################
# For the Third-Party Libraries.
# #####################################################
# find the parallel compute lib
find_package(OpenMP)

if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-D_USE_OPENMP)
    set(_USE_OPENMP TRUE)

    # cmake only check for separate OpenMP library on AppleClang 7+
    # https://github.com/Kitware/CMake/blob/42212f7539040139ecec092547b7d58ef12a4d72/Modules/FindOpenMP.cmake#L252
    if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" AND(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0"))
        set(OpenMP_LIBS ${OpenMP_libomp_LIBRARY})
        list(APPEND OpenMVS_EXTRA_LIBS ${OpenMP_LIBS})
    endif()
else()
    message("-- Can't find OpenMP. Continuing without it.")
endif()

# #####################################################
# For The Third-Party
# #####################################################
# set(PoseLib_DIR /home/mvs18/Desktop/dhn/didi_3d_reconstruction/third_party/PoseLib/_install/lib/cmake/PoseLib)
# find_package(PoseLib REQUIRED)
find_package(Glog REQUIRED)
find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    ADD_DEFINITIONS(${OpenCV_DEFINITIONS})
    set(_USE_OPENCV TRUE)
    MESSAGE(STATUS "OpenCV ${OpenCV_VERSION} found (include: ${OpenCV_INCLUDE_DIRS})")
else()
    MESSAGE("-- Can't find OpenCV. Please specify OpenCV directory using OpenCV_DIR variable")
endif()

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# #####################################################
# For Ours Codes.
# #####################################################
include_directories(include)

aux_source_directory(src/base BASE_SRC)
aux_source_directory(src/calibration CALIBRATION_SRC)
aux_source_directory(src/panoramic_process PANORAMIC_SRC)
file(GLOB EXE_SRCS "src/exe/*.cc")

add_library(base ${MAKE_SHARED_LIBRARIES} ${BASE_SRC})
add_library(calibration ${MAKE_SHARED_LIBRARIES} ${CALIBRATION_SRC})
add_library(panoramic ${MAKE_SHARED_LIBRARIES} ${PANORAMIC_SRC})

set(FISHCAT_EXTERNAL_LIBRARIES
    ${GLOG_LIBRARIES}
    ${CERES_LIBRARIES}
    ${OpenCV_LIBS}
)

set(FISHCAT_INTERNAL_LIBRARIES
    base
    calibration
    panoramic
)

add_executable(fishcat ${EXE_SRCS})
target_link_libraries(fishcat ${FISHCAT_EXTERNAL_LIBRARIES} ${FISHCAT_INTERNAL_LIBRARIES})
