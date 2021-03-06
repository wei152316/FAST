﻿cmake_minimum_required(VERSION 3.5)

project(FAST)

set(VERSION_MAJOR 3)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)
set(VERSION_SO 1)   # SO version, should be incremented by 1 every time the existing API changes
set(FAST_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# set policies explicitly to avoid warnings.
# Read more on CMP0022 here: https://cmake.org/cmake/help/v3.0/policy/CMP0022.html
# This was set as a fix for windows builds where FAST was expecting the name of
# the zlib library to be 'zlib' when it was in fact 'zlibd'.
cmake_policy(SET CMP0022 NEW)

include(cmake/Macros.cmake)

#### Options
option(FAST_BUILD_TESTS "Build FAST tests." ON)
option(FAST_BUILD_EXAMPLES "Build examples." OFF)
option(FAST_BUILD_TOOLS "Build tools." ON)
option(FAST_BUILD_DOCS "Build API documentation" OFF)
option(FAST_DOWNLOAD_TEST_DATA "Download test data automatically (> 1 GB)" OFF)
option(FAST_MODULE_VTK "Enable interoperability with VTK" OFF)
option(FAST_MODULE_ITK "Enable interoperability with ITK" OFF)
option(FAST_MODULE_Visualization "Enable visualization capabilities using Qt5. Without this module, all visualization in FAST will be disabled." ON)
option(FAST_MODULE_OpenIGTLink "Build module OpenIGTLink" ON)
option(FAST_MODULE_TensorFlow "Build TensorFlow inference engine" OFF)
option(FAST_MODULE_TensorRT "Build NVIDIA TensorRT inference engine" OFF)
option(FAST_MODULE_OpenVINO "Build Intel OpenVINO inference engine" ON)
option(FAST_MODULE_Python "Build Python wrappers" OFF)
option(FAST_MODULE_Kinect "Build kinect module" OFF)
option(FAST_MODULE_RealSense "Build real sense module" ON)
option(FAST_MODULE_WholeSlideImaging "Build whole slide imaging module" ON)
option(FAST_MODULE_Clarius "Build clarius ultrasound module" ON)
option(FAST_MODULE_Dicom "Build DICOM module (DCMTK)" ON)
option(FAST_MODULE_HDF5 "Build HDF5 module" OFF)
option(FAST_MODULE_Plotting "Build plotting module (JKQTPlotter)" OFF)
option(FAST_CONTINUOUS_INTEGRATION "Used for continuous integration tests" OFF)
option(FAST_BUILD_QT5 "Download and build Qt 5. Turn this OFF if you want to use pre-built binaries you already have on your machine." ON)

# Python version
set(FAST_Python_Version "" CACHE STRING "Specify which Python version to build python wrappers with. E.g. 2 or 3")

#### Set build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("-- Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build types for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

## Set continous integration define, used to enabled auto-close application on examples
if(FAST_CONTINUOUS_INTEGRATION)
    add_definitions("-DFAST_CONTINUOUS_INTEGRATION")
endif()


## Set build folders
# First for the generic no-config case (e.g. with mingw)
cmake_policy(SET CMP0043 OLD)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    # Force libs and bins to go into bin and lib directory instead of Release/Debug
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

#### Paths
set(FAST_TEST_DATA_DIR "" CACHE PATH "Directory of test data. Default is ROOT/data/.")
set(FAST_OPENCL_KERNEL_BINARY_PATH "" CACHE PATH "Directory to put kernel binaries in. Default is build dir.")
set(FAST_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source/FAST/")

#### Module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH}) # For finding the custom Find modules

# Set debug define if debug mode is set
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    message("-- FAST Debug mode set")
    add_definitions("-DFAST_DEBUG")
endif()

# Set FAST include dirs
set(FAST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/ ${CMAKE_CURRENT_BINARY_DIR})

# Some extra build settings (C++14 and OpenMP)
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations") # Remove deprecated warnings GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes") # Remove ignored attritubes on template argument warnings from cl.hpp
endif()

# This will setup all of FAST's dependencies
include(cmake/Depdendencies.cmake)

# This will download the test data
include(cmake/DownloadTestData.cmake)

if(FAST_BUILD_DOCS)
    include(cmake/Documentation.cmake)
endif()

#### Set include dirs
include_directories(${FAST_INCLUDE_DIRS})
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(FAST_INCLUDE_DIRS ${FAST_INCLUDE_DIRS} PARENT_SCOPE)
endif()

# Set up RPATH with relative path so that binaries will find libraries in the lib folder
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

if(WIN32)
	# Parallel build in MSVC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /nologo /MD /EHsc")
endif()

#### Add all subdirs
fast_add_subdirectories(source/FAST)


if(FAST_BUILD_EXAMPLES)
    if(FAST_MODULE_Visualization)
        set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
        if(WIN32)
            file(WRITE ${PROJECT_BINARY_DIR}/runAllExamples.bat "")
        else()
            file(WRITE ${PROJECT_BINARY_DIR}/runAllExamples.sh "#!/bin/bash\n")
        endif()
        add_subdirectory(source/FAST/Examples)
    else()
        message("-- Not building examples since visualization module is disabled.")
    endif()
endif()



#### Create FAST library and executables
add_library(FAST SHARED ${FAST_SOURCE_FILES})
add_dependencies(FAST ${FAST_EXTERNAL_DEPENDENCIES})
include(GenerateExportHeader)
generate_export_header(FAST EXPORT_FILE_NAME FASTExport.hpp)

include(cmake/OpenMP.cmake)

## Link everything
message("-- FAST LIBS: ${LIBRARIES} ${FAST_SYSTEM_LIBRARIES}")
message("-- FAST INCLUDE DIRS: ${FAST_INCLUDE_DIRS}")
target_link_libraries(FAST PUBLIC ${LIBRARIES} ${FAST_SYSTEM_LIBRARIES})
set_target_properties(FAST PROPERTIES VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" SOVERSION ${VERSION_SO})

## Build test executable
if(FAST_BUILD_TESTS)
    add_executable(testFAST ${FAST_TEST_SOURCE_FILES})
    target_link_libraries(testFAST FAST)
endif()

set(CONF_FAST_LIBRARIES FAST ${LIBRARIES})

# Create config and use file
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/FASTConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/FASTConfig.cmake"
        @ONLY
)
configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/FASTUse.cmake.in"
        "${PROJECT_BINARY_DIR}/FASTUse.cmake"
        @ONLY
)

# Set paths needed at runtime to be written to config file
set(CONFIG_TEST_DATA_PATH "TestDataPath = ${CMAKE_CURRENT_SOURCE_DIR}/data/")
set(CONFIG_DOCUMENTATION_PATH "DocumentationPath = ${CMAKE_CURRENT_SOURCE_DIR}/doc/")
set(CONFIG_KERNEL_SOURCE_PATH "KernelSourcePath = ${CMAKE_CURRENT_SOURCE_DIR}/source/FAST/")
set(CONFIG_PIPELINE_PATH "PipelinePath = ${CMAKE_CURRENT_SOURCE_DIR}/pipelines/")

# If test data dir has been set by user, use that instead
if(NOT FAST_TEST_DATA_DIR STREQUAL "")
    # Remove any existing "
    string(REPLACE "\"" "" FAST_TEST_DATA_DIR ${FAST_TEST_DATA_DIR})
    message("-- FAST test data directory set to: \"${FAST_TEST_DATA_DIR}\"")
    set(CONFIG_TEST_DATA_PATH "TestDataPath = ${FAST_TEST_DATA_DIR}/")
endif()
# If OpenCL kernel binary path has been set by user, write it to the config
if(NOT FAST_OPENCL_KERNEL_BINARY_PATH STREQUAL "")
    # Remove any existing "
    string(REPLACE "\"" "" FAST_OPENCL_KERNEL_BINARY_PATH ${FAST_OPENCL_KERNEL_BINARY_PATH})
    message("-- OpenCL Kernel binary path set to: ${FAST_OPENCL_KERNEL_BINARY_PATH}")
    set(CONFIG_KERNEL_BINARY_PATH "KernelBinaryPath = ${FAST_OPENCL_KERNEL_BINARY_PATH}/")
endif()

# configure the configuration file.
configure_file(
    "${PROJECT_SOURCE_DIR}/source/fast_configuration.txt.in"
    "${PROJECT_BINARY_DIR}/fast_configuration.txt"
)

# Setup process object for pipelines setup
foreach(FILE ${FAST_PROCESS_OBJECT_HEADER_FILES})
    set(FAST_PROCESS_OBJECT_INCLUDES "${FAST_PROCESS_OBJECT_INCLUDES}#include \"${FILE}\"\n")
endforeach()

foreach(NAME ${FAST_PROCESS_OBJECT_NAMES})
    set(FAST_PROCESS_OBJECT_REGISTRATION "${FAST_PROCESS_OBJECT_REGISTRATION}FAST_REGISTER_PO(${NAME})\n")
endforeach()

configure_file(
    "${PROJECT_SOURCE_DIR}/source/ProcessObjectList.hpp.in"
    "${PROJECT_BINARY_DIR}/ProcessObjectList.hpp"
)

# Setup inference engines
foreach(ENGINE ${FAST_INFERENCE_ENGINES})
    message("---- Adding inference engnine ${ENGINE}")
    #add_dependencies(testFAST ${ENGINE})
endforeach()

if(NOT FAST_BUILD_QT5 AND WIN32)
    # Using pre-built binaries on Windows so add custom command to copy
    # dlls automatically to the bin folder.
    include(cmake/FASTCustomCommands.cmake)
endif(NOT FAST_BUILD_QT5 AND WIN32)

include(cmake/ModulePython.cmake)
include(cmake/InstallFAST.cmake)
include(cmake/Package.cmake)
