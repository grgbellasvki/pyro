
set(pyro_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pyrolysis_gas.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/gas_table.cpp
                      CACHE INTERNAL "" FORCE)

set(pyro_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pyrolysis_gas.h
                      ${CMAKE_CURRENT_SOURCE_DIR}/gas_table.h
                      ${CMAKE_CURRENT_SOURCE_DIR}/table_entry.h
                      ${CMAKE_CURRENT_SOURCE_DIR}/icaruspyro.h
                      CACHE INTERNAL "" FORCE)         

set(pyro_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/testing/TestCaseDriver.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/testing/test_pyrolysis_gas.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/testing/test_gas_table.cpp
                    CACHE INTERNAL "" FORCE)
