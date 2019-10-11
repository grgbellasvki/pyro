cmake_minimum_required(VERSION 3.4)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(pyro
        VERSION 1.0
        LANGUAGES CXX)

# --
# Define policies and compiler standards
# --
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This CMake policy uses <PackageName>_ROOT to find the libraries/packages.
#
cmake_policy(SET CMP0074 NEW)

# Set path to CMake modules, macros, and functions
# --
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)

#message(STATUS "Configuring Icarus Version: ${ICARUS_VERSION}")

# ------------------------------------------------------------------------------
# Icarus Global Defaults
# ------------------------------------------------------------------------------

# Set the prefix path for installing Icarus
# --
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install"
        CACHE PATH "Icarus installation directory"
        FORCE)
endif()
message(STATUS "Icarus installation prefix path: ${CMAKE_INSTALL_PREFIX}")

# Use the GNUInstallDirs to define standard installation paths
# --
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}"
   CACHE PATH "The directory relative to CMAKE_PREFIX_PATH where header files are installed"
)
set(CMAKEPACKAGE_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake"
  CACHE PATH "The directory relative to CMAKE_PREFIX_PATH where IcarusConfig.cmake is installed."
)
set(PKGCONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/pkgconfig"
  CACHE PATH "The directory relative to CMAKE_PREFIX_PATH where Icarus.pc is installed."
)
set(TEST_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/test"
  CACHE PATH "The directory relative to CMAKE_PREFIX_PATH where test data are installed."
)
set(DOCS_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/docs"
  CACHE PATH "The directory relative to CMAKE_PREFIX_PATH where documentation is installed."
)

set(IcarusPyro_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
set(IcarusPyro_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
set(IcarusPyro_TEST_DIR "${CMAKE_INSTALL_PREFIX}/${TEST_INSTALL_DIR}")
set(IcarusPyro_DOCS_DIR "${CMAKE_INSTALL_PREFIX}/${DOCS_INSTALL_DIR}")

# Set the build type. By default, the ${CMAKE_BUILD_TYPE} is set to Release.
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "CMake build type is: ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------------------------
# Configuration options
# ------------------------------------------------------------------------------

option(IcarusPyro_WITH_MUTATION  "Build Icarus with support for Mutation++ (Required for GSI physics)"  OFF)

# --
# Find external packages
# --
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Mutation++ : find_package() will set the include and library directoies
# --
find_package(MPP REQUIRED)
add_library(Mutation INTERFACE)
set_target_properties(Mutation PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Mutation_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Mutation_LIBRARIES}"
)

# HDF5
# --
set(LIB_TYPE STATIC)
string(TOLOWER ${LIB_TYPE} SEARCH_TYPE)
find_package(HDF5 NAMES hdf5 COMPONENTS CXX ${SEARCH_TYPE})
add_library(hdf5 INTERFACE IMPORTED)
set(HDF5_LIBRARIES ${HDF5_LIBRARIES} ${HDF5_CXX_${LIB_TYPE}_LIBRARY})
set_target_properties(hdf5 PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${HDF5_INCLUDE_DIR}"
                      INTERFACE_LINK_LIBRARIES "${HDF5_LIBRARIES}")

# Two options: If you want to use a system-installed version of Catch2, then use the
# find_package command. Otherwise the target is provided as a subdirectory and the
# CMAKE_MODULE_PATH is updated to include the Catch.cmake and the AddCatchTests.cmake
# files.
#
add_subdirectory(thirdparty/Catch2)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/thirdparty/Catch2/contrib)

# --
# Gather the source files and the unit tests
# --
add_subdirectory(src)
add_subdirectory(apps)

# --
# Create the library target
# --
add_library(pyro_lib ${pyro_SOURCE_FILES})

set_target_properties(pyro_lib
  PROPERTIES
    OUTPUT_NAME icaruspyro
)

# --
# Linking
# --
target_link_libraries(pyro_lib
  PRIVATE
     Mutation
     Eigen3::Eigen
     hdf5
)

target_include_directories(pyro_lib
  PUBLIC
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
)

install(FILES ${pyro_HEADER_FILES}
        DESTINATION ${INCLUDE_INSTALL_DIR}
)

# --
# Create an application driver
# --
add_executable(pyro ${pyro_APP_FILES})

set_target_properties(pyro
     PROPERTIES
     OUTPUT_NAME pyro
)

target_include_directories(pyro
  PUBLIC
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
)

target_link_libraries(pyro
  PRIVATE
     Catch2::Catch2
     Mutation
     Eigen3::Eigen
     hdf5
  PUBLIC
     pyro_lib
)

# --
# Create the driver for the unit tests
# --
add_executable(pyro_test ${pyro_TEST_FILES})

set_target_properties(pyro_test
     PROPERTIES
     OUTPUT_NAME test_icaruspyro
)

target_link_libraries(pyro_test
  PRIVATE
     Catch2::Catch2
     Mutation
     Eigen3::Eigen
     hdf5
  PUBLIC
     pyro_lib
)

# --
# Hook in the unit testing
# --
enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(pyro_test)

# --
# Install the targets
# --
install(TARGETS
        pyro_lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(TARGETS
        pyro_test
        RUNTIME DESTINATION bin
)
