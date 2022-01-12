# It defines the following variables
#  NeuH_INCLUDE_DIRS - include directories
#  NeuH_LOG_LEVEL    - log verbosity level

SET(NeuH_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  SET(NeuH_LOG_LEVEL 6 CACHE STRING "Log verbosity level")
ELSE()
  SET(NeuH_LOG_LEVEL "" CACHE STRING "Log verbosity level")
ENDIF()

SET(LEVEL_LIST 0 1 2 3 4 5 6)
SET_PROPERTY(CACHE NeuH_LOG_LEVEL PROPERTY STRINGS ${LEVEL_LIST})

IF(NeuH_LOG_LEVEL STREQUAL "")
  MESSAGE(FATAL_ERROR "NeuH_LOG_LEVEL is not specified!")
ELSE()
  ADD_DEFINITIONS(-DNLOG_LEVEL=${NeuH_LOG_LEVEL})
ENDIF()
