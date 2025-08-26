CMAKE_MINIMUM_REQUIRED(VERSION 3.18 FATAL_ERROR)
PROJECT(x_test LANGUAGES C)

IF(MSVC)
  # NOTE: `/Zc:preprocessor` is required to make x_narg work with MSVC.
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /WX /Zc:preprocessor /Zc:__STDC__")
ELSE()
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra")
ENDIF()

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED ON)

ADD_COMPILE_DEFINITIONS(X_ENABLE_SOCKET=1)

INCLUDE_DIRECTORIES(
  ../..
)

SET(TEST_TARGETS
  test_x_error
  test_x_event
  test_x_gadget
  test_x_logging
  test_x_memory
)

FOREACH(TARGET ${TEST_TARGETS})
  ADD_EXECUTABLE(${TARGET} ${TARGET}.c)
ENDFOREACH()
