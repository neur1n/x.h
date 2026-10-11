CMAKE_MINIMUM_REQUIRED(VERSION 3.18)
PROJECT(x_h LANGUAGES C CXX)

OPTION(X_BUILD_TESTS "Build x.h C and C++ tests" ON)

FIND_PACKAGE(Python3 COMPONENTS Interpreter REQUIRED)

ADD_CUSTOM_TARGET(x_generate
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tool/generate.py
      --lang c --module all --output ${CMAKE_CURRENT_BINARY_DIR}/generated/x.h
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tool/generate.py
      --lang cpp --module all --output ${CMAKE_CURRENT_BINARY_DIR}/generated/cpp/x.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Extracting the complete C and C++ distributions into the build directory"
  VERBATIM
)

IF(X_BUILD_TESTS)
  ENABLE_TESTING()

  ADD_SUBDIRECTORY(test/c)
  ADD_SUBDIRECTORY(test/cpp)
ENDIF()
