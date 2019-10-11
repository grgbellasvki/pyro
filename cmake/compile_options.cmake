# Defines any compilation flags needed in addition to
# the default flags provided by CMake. For reference, the default CMake
# flags (same for C/C++/Fortran) are:
#
#   Debug:          -g
#   Release:        -O3 -DNDEBUG
#   RelWithDebInfo: -O2 -DNDEBUG -g
#   MinSizeRel:     -Os -DNDEBUG

set(options "")

#-----------------------------------------------------------------------
# Intel
#-----------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")

    # Common
        list(APPEND options
            -real-size 64       # Default to 64bit reals
            -fpp                # Run preprocessor on all files
            -extend-source 132  # Allows long lines in source files
            -traceback          # Emit stacktrace on error
        )

    # Debug
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND options
            -check all      # Runtime bounds checking
            -fpe0           # Abort if get NaN, etc.
            -warn all       # Full warning analysis
        )
    endif()

    # Release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND options
            -pad            # Allow padding arrays for alignment
            -unroll         # Allow loop unrolling (implied by -O3?)
            -ip             # Allow inter-procedural optimization
        )
    endif()

    exec_program(${CMAKE_CXX_COMPILER}
                 ARGS --version
                 OUTPUT_VARIABLE _compiler_output)
    string(REGEX REPLACE ".* ([0-9]+\\.[0-9]+\\.[0-9]+) .*" "\\1"
           IFORT_COMPILER_VERSION ${_compiler_output})
    message(STATUS "Intel CXX compiler version: ${IFORT_COMPILER_VERSION}")
    string(CONCAT COMPILER_VERSION "intel-" "${IFORT_COMPILER_VERSION}")

#-----------------------------------------------------------------------
# GNU
#-----------------------------------------------------------------------
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    # Common
    list(APPEND options
        -fbacktrace                 # Emit stacktrace on error
    )

    # Debug
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND options
            -fcheck=all                       # Runtime bounds checking
            -ffpe-trap=invalid,zero,overflow  # Abort if NaN, etc.
            -Wall -Wextra                     # Full warning analysis
        )
    endif()

    # Release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND options
            -fstack-arrays      # Place automatic array on the stack
            -funroll-loops      # Allow loop unrolling
            # Don't see an equivalent of Intel's -pad
            # Basic ip optimization is implied with -O3
        )
    endif()

    exec_program(${CMAKE_CXX_COMPILER}
               ARGS --version
               OUTPUT_VARIABLE _compiler_output)
    string(REGEX REPLACE ".* ([0-9]+\\.[0-9]+\\.[0-9]+) .*" "\\1"
           GFORTRAN_COMPILER_VERSION ${_compiler_output})
    message(STATUS "gfortran compiler version: ${GFORTRAN_COMPILER_VERSION}")
    string(CONCAT COMPILER_VERSION "gfortran-" "${GFORTRAN_COMPILER_VERSION}")

#-----------------------------------------------------------------------
# Unknown Compiler
#-----------------------------------------------------------------------
else()
    message(
        FATAL_ERROR
        "CXX compiler '${CMAKE_CXX_COMPILER_ID}' is not currently supported."
    )
endif()

#-----------------------------------------------------------------------
# Cache Results
#-----------------------------------------------------------------------
set(COMPILE_OPTIONS ${options} CACHE STRING
    "Compiler options added to all components"
    FORCE)
mark_as_advanced(COMPILE_OPTIONS)
