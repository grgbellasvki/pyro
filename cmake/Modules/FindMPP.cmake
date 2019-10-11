# Mutation++ library (Used for matieral/gas property calculations)
#
# See the CMake file in Modules/FindMutation.cmake for more information about
# how this library is found by CMake.
# --

set(mpp_include_dir "$ENV{MPP_ROOT}/include")
set(mpp_library_dir "$ENV{MPP_ROOT}/lib")

find_path(Mutation_INCLUDE_DIR
  NAMES mutation++.h
  PATHS "${mpp_include_dir}/mutation++"
  NO_DEFAULT_PATH
)
if ("${Mutation_INCLUDE_DIR}" STREQUAL "Mutation_INCLUDE_DIR-NOTFOUND")
  message("Set the environment variable MPP_ROOT")
  message("$ENV{MPP_ROOT}")
  message(FATAL_ERROR "Could not find the Mutation++ library.")
endif()

find_path(Eigen_INCLUDE_DIR
  NAMES signature_of_eigen3_matrix_library
  PATHS "${mpp_include_dir}/eigen3"
  NO_DEFAULT_PATH
)
if(NOT ${Eigen_INCLUDE_DIR} STREQUAL "Eigen_INCLUDE_DIR-NOTFOUND")
  set(Mutation_INCLUDE_DIRS "${Mutation_INCLUDE_DIR};${mpp_include_dir}")
else()
  set(Mutation_INCLUDE_DIRS "${Mutation_INCLUDE_DIR}")
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(mpp_lib "$ENV{MPP_DIRECTORY}/lib/libmutation++.dylib")
  #set(mpp_lib_fortran "$ENV{MPP_DIRECTORY}/lib/libmutation++_fortran.dylib")

  find_path(Mutation_LIBRARY_DIR
    NAMES libmutation++.dylib
    PATHS ${mpp_library_dir}
    NO_DEFAULT_PATH
  )
  #set(Mutation_LIBRARIES "${Mutation_LIBRARY_DIR}/libmutation++.dylib;${Mutation_LIBRARY_DIR}/libmutation++_fortran.dylib")
  set(Mutation_LIBRARIES "${Mutation_LIBRARY_DIR}/libmutation++.dylib")

else()
  set(mpp_lib "$ENV{MPP_DIRECTORY}/lib/libmutation++.so")
  #set(mpp_lib_fortran "$ENV{MPP_DIRECTORY}/lib/libmutation++_fortran.so")

  find_path(Mutation_LIBRARY_DIR
    NAMES libmutation++.so
    PATHS ${mpp_library_dir}
    NO_DEFAULT_PATH
  )
  #set(Mutation_LIBRARIES "${Mutation_LIBRARY_DIR}/libmutation++.so;${Mutation_LIBRARY_DIR}/libmutation++_fortran.so")
  set(Mutation_LIBRARIES "${Mutation_LIBRARY_DIR}/libmutation++.so")
endif()

set(Mutation_DIR $ENV{MPP_ROOT} CACHE PATH "Mutation++ installation directory" FORCE)


