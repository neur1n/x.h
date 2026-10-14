CMAKE_MINIMUM_REQUIRED(VERSION 3.18)
PROJECT(x_h_cpp_tests LANGUAGES CXX)
ENABLE_TESTING()
FIND_PACKAGE(Python3 COMPONENTS Interpreter REQUIRED)
OPTION(X_CPP_CUDA_SDK_CHECK "Compile-check C++ APIs against an installed CUDA Toolkit" OFF)
SET(X_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(ERROR_CONTEXT "$<IF:$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>,release,debug>")
IF(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  ADD_TEST(NAME x_cpp_header_layout COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_header_layout.py)
  ADD_TEST(NAME x_cpp_generator COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_generator.py)
  SET_TESTS_PROPERTIES(x_cpp_header_layout x_cpp_generator PROPERTIES
    TIMEOUT 30 LABELS "cpp;modularization")
ENDIF()

FUNCTION(x_cpp_test TARGET)
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
  IF(MSVC)
    TARGET_COMPILE_OPTIONS(${TARGET} PRIVATE /W4 /WX /Zc:preprocessor)
  ELSE()
    TARGET_COMPILE_OPTIONS(${TARGET} PRIVATE -Wall -Wextra -Werror -pedantic-errors)
  ENDIF()
  IF(WIN32)
    TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ws2_32)
  ENDIF()
ENDFUNCTION()

ADD_EXECUTABLE(x_test_cpp_check_expression check_expression.cpp)
x_cpp_test(x_test_cpp_check_expression)
ADD_TEST(NAME cpp_check_expression COMMAND x_test_cpp_check_expression)
SET_TESTS_PROPERTIES(cpp_check_expression PROPERTIES TIMEOUT 15 LABELS "cpp;error")

IF(X_CPP_CUDA_SDK_CHECK)
  FIND_PACKAGE(CUDAToolkit REQUIRED)
  ADD_LIBRARY(x_cpp_cuda_sdk_check OBJECT sdk_compile.cpp)
  TARGET_COMPILE_DEFINITIONS(x_cpp_cuda_sdk_check PRIVATE
    X_ENABLE_CUDA_DRIVER=1 X_ENABLE_CUDA_RUNTIME=1 X_ENABLE_CUBLAS=1)
  TARGET_LINK_LIBRARIES(x_cpp_cuda_sdk_check PRIVATE CUDA::cuda_driver CUDA::cudart CUDA::cublas)
  x_cpp_test(x_cpp_cuda_sdk_check)
ENDIF()

SET(OPTIONAL_MODULES logging checksum date_time filesystem hardware math memory string cuda)
SET(MODULE_LABELS log checksum time fs hw math mem str cuda)
SET(GENERATED_CASES minimal ${OPTIONAL_MODULES} all io_bundle without_cuda core_gpu_errors)
FILE(STRINGS ${X_ROOT}/cpp/x.h VERSION_LINE REGEX "^#define X_H x_version")
STRING(REGEX REPLACE ".*x_version\\(([0-9]+), ([0-9]+), ([0-9]+)\\).*"
  "\\1.\\2.\\3" VERSION "${VERSION_LINE}")
FOREACH(CASE IN LISTS GENERATED_CASES)
  SET(MODULES ${CASE})
  IF(CASE STREQUAL "io_bundle")
    SET(MODULES filesystem memory string)
  ELSEIF(CASE STREQUAL "without_cuda")
    SET(MODULES ${OPTIONAL_MODULES})
    LIST(REMOVE_ITEM MODULES cuda)
  ELSEIF(CASE STREQUAL "core_gpu_errors")
    SET(MODULES minimal)
  ENDIF()
  SET(HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/${CASE})
  SET(HEADER ${HEADER_DIR}/generated_under_test.h)
  ADD_CUSTOM_COMMAND(OUTPUT ${HEADER}
    COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/tool/generate.py
      --lang cpp --module ${MODULES} --output ${HEADER}
    DEPENDS ${X_ROOT}/cpp/x.h ${X_ROOT}/tool/generate.py VERBATIM)
  ADD_CUSTOM_TARGET(x_cpp_header_${CASE} DEPENDS ${HEADER})
  SET(TARGET x_test_modules_cpp_${CASE})
  ADD_EXECUTABLE(${TARGET} smoke.cpp generated_peer.cpp)
  ADD_DEPENDENCIES(${TARGET} x_cpp_header_${CASE})
  TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${HEADER_DIR})
  SET(DESCRIPTION "${VERSION}+core")
  FOREACH(MODULE IN LISTS OPTIONAL_MODULES)
    IF(MODULE IN_LIST MODULES OR CASE STREQUAL "all")
      STRING(TOUPPER ${MODULE} DEFINE)
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_${DEFINE}=1)
      LIST(FIND OPTIONAL_MODULES ${MODULE} MODULE_INDEX)
      LIST(GET MODULE_LABELS ${MODULE_INDEX} LABEL)
      STRING(APPEND DESCRIPTION ".${LABEL}")
    ENDIF()
  ENDFOREACH()
  TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_DESCRIPTION="${DESCRIPTION}")
  IF(CASE STREQUAL "core_gpu_errors")
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${X_ROOT}/test/c/sdk_stubs)
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_GPU_ERRORS=1
      X_ENABLE_CUDA_DRIVER=1 X_ENABLE_CUDA_RUNTIME=1 X_ENABLE_CUBLAS=1)
  ENDIF()
  x_cpp_test(${TARGET})
  ADD_TEST(NAME modules_cpp_${CASE} COMMAND ${TARGET})
  SET_TESTS_PROPERTIES(modules_cpp_${CASE} PROPERTIES TIMEOUT 15 LABELS "cpp;modularization")
ENDFOREACH()

ADD_EXECUTABLE(x_test_cpp_check_diagnostics check_diagnostics.cpp)
ADD_DEPENDENCIES(x_test_cpp_check_diagnostics x_cpp_header_minimal)
TARGET_INCLUDE_DIRECTORIES(x_test_cpp_check_diagnostics PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/generated/minimal)
x_cpp_test(x_test_cpp_check_diagnostics)
ADD_TEST(NAME cpp_check_diagnostics
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_error_handling.py
    --executable $<TARGET_FILE:x_test_cpp_check_diagnostics> --case check_minimal
    --expect success --context ${ERROR_CONTEXT})
SET_TESTS_PROPERTIES(cpp_check_diagnostics PROPERTIES
  TIMEOUT 15 LABELS "cpp;error;modularization")

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hardware_under_test.h
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/create_hardware_fixture.py
    --lang cpp --output ${CMAKE_CURRENT_BINARY_DIR}/hardware_under_test.h
  DEPENDS ${X_ROOT}/cpp/x.h ${X_ROOT}/test/create_hardware_fixture.py VERBATIM)
ADD_CUSTOM_TARGET(x_cpp_hardware_fixture
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hardware_under_test.h)
FOREACH(PLATFORM IN ITEMS windows windows_legacy windows_unspecified posix)
  SET(TARGET x_test_cpp_hardware_faults_${PLATFORM})
  ADD_EXECUTABLE(${TARGET} hardware.cpp)
  ADD_DEPENDENCIES(${TARGET} x_cpp_hardware_fixture)
  TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  IF(PLATFORM STREQUAL "posix")
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_WINDOWS=0)
  ELSE()
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_WINDOWS=1)
    IF(PLATFORM STREQUAL "windows")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE _WIN32_WINNT=0x0601)
    ELSEIF(PLATFORM STREQUAL "windows_legacy")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE _WIN32_WINNT=0x0501)
    ELSE()
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_WINDOWS_UNSPECIFIED=1)
    ENDIF()
  ENDIF()
  x_cpp_test(${TARGET})
  ADD_TEST(NAME cpp_hardware_faults_${PLATFORM} COMMAND ${TARGET})
  SET_TESTS_PROPERTIES(cpp_hardware_faults_${PLATFORM} PROPERTIES
    TIMEOUT 10 LABELS "cpp;hardware")
ENDFOREACH()

FOREACH(MODULE IN ITEMS memory string)
  FOREACH(CASE IN ITEMS full generated)
    SET(TARGET x_test_cpp_${MODULE}_${CASE})
    ADD_EXECUTABLE(${TARGET} ${MODULE}.cpp)
    IF(CASE STREQUAL "generated")
      ADD_DEPENDENCIES(${TARGET} x_cpp_header_${MODULE})
      TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated/${MODULE})
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_GENERATED=1)
    ENDIF()
    x_cpp_test(${TARGET})
    ADD_TEST(NAME cpp_${MODULE}_${CASE} COMMAND ${TARGET})
    SET_TESTS_PROPERTIES(cpp_${MODULE}_${CASE} PROPERTIES
      TIMEOUT 15 LABELS "cpp;${MODULE}")
  ENDFOREACH()
ENDFOREACH()

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp_memory_under_test.h
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/create_memory_fixture.py
    --lang cpp --output ${CMAKE_CURRENT_BINARY_DIR}/cpp_memory_under_test.h
  DEPENDS ${X_ROOT}/cpp/x.h ${X_ROOT}/test/create_memory_fixture.py
    ${X_ROOT}/test/fixture_source.py VERBATIM)
ADD_CUSTOM_TARGET(x_cpp_memory_fixture
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp_memory_under_test.h)
ADD_EXECUTABLE(x_test_cpp_memory_malloc_faults memory.cpp)
ADD_DEPENDENCIES(x_test_cpp_memory_malloc_faults
  x_cpp_header_minimal x_cpp_memory_fixture)
TARGET_INCLUDE_DIRECTORIES(x_test_cpp_memory_malloc_faults PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/generated/minimal ${CMAKE_CURRENT_BINARY_DIR})
TARGET_COMPILE_DEFINITIONS(x_test_cpp_memory_malloc_faults PRIVATE
  TEST_MALLOC_FAULTS=1)
x_cpp_test(x_test_cpp_memory_malloc_faults)
ADD_TEST(NAME cpp_memory_malloc_faults COMMAND x_test_cpp_memory_malloc_faults)
SET_TESTS_PROPERTIES(cpp_memory_malloc_faults PROPERTIES
  TIMEOUT 15 LABELS "cpp;memory")

FOREACH(CASE IN ITEMS full generated)
  SET(TARGET x_test_cpp_mathematics_${CASE})
  ADD_EXECUTABLE(${TARGET} mathematics.cpp)
  IF(CASE STREQUAL "generated")
    ADD_DEPENDENCIES(${TARGET} x_cpp_header_math)
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}/generated/math)
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_GENERATED=1)
  ENDIF()
  x_cpp_test(${TARGET})
  ADD_TEST(NAME cpp_mathematics_${CASE} COMMAND ${TARGET})
  SET_TESTS_PROPERTIES(cpp_mathematics_${CASE} PROPERTIES
    TIMEOUT 20 LABELS "cpp;mathematics")
ENDFOREACH()

FOREACH(CASE IN ITEMS full generated)
  SET(TARGET x_test_cpp_date_time_${CASE})
  ADD_EXECUTABLE(${TARGET} date_time.cpp)
  IF(CASE STREQUAL "generated")
    ADD_DEPENDENCIES(${TARGET} x_cpp_header_date_time)
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}/generated/date_time)
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_GENERATED=1)
  ENDIF()
  x_cpp_test(${TARGET})
  ADD_TEST(NAME cpp_date_time_${CASE} COMMAND ${TARGET})
  SET_TESTS_PROPERTIES(cpp_date_time_${CASE} PROPERTIES
    TIMEOUT 15 LABELS "cpp;date_time")
ENDFOREACH()

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp_date_time_under_test.h
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/create_date_time_fixture.py
    --lang cpp --output ${CMAKE_CURRENT_BINARY_DIR}/cpp_date_time_under_test.h
  DEPENDS ${X_ROOT}/cpp/x.h ${X_ROOT}/test/create_date_time_fixture.py VERBATIM)
ADD_CUSTOM_TARGET(x_cpp_date_time_fixture
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp_date_time_under_test.h)
FOREACH(PLATFORM IN ITEMS posix windows)
  SET(TARGET x_test_cpp_date_time_faults_${PLATFORM})
  ADD_EXECUTABLE(${TARGET} date_time_faults.cpp)
  ADD_DEPENDENCIES(${TARGET} x_cpp_date_time_fixture)
  TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  IF(PLATFORM STREQUAL "windows")
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_WINDOWS=1)
  ELSE()
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_WINDOWS=0)
  ENDIF()
  x_cpp_test(${TARGET})
  ADD_TEST(NAME cpp_date_time_faults_${PLATFORM} COMMAND ${TARGET})
  SET_TESTS_PROPERTIES(cpp_date_time_faults_${PLATFORM} PROPERTIES
    TIMEOUT 15 LABELS "cpp;date_time")
ENDFOREACH()

FOREACH(CASE IN ITEMS full generated faults)
  SET(TARGET x_test_cpp_filesystem_${CASE})
  ADD_EXECUTABLE(${TARGET} filesystem.cpp)
  IF(CASE STREQUAL "generated")
    ADD_DEPENDENCIES(${TARGET} x_cpp_header_filesystem)
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}/generated/filesystem)
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_GENERATED=1)
  ELSEIF(CASE STREQUAL "faults")
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_RESOLVE_FAULTS=1)
  ENDIF()
  x_cpp_test(${TARGET})
  ADD_TEST(NAME cpp_filesystem_${CASE}
    COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/run_filesystem.py
      --executable $<TARGET_FILE:${TARGET}>)
  SET_TESTS_PROPERTIES(cpp_filesystem_${CASE} PROPERTIES
    TIMEOUT 15 LABELS "cpp;filesystem")
ENDFOREACH()

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp_platform_under_test.h
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/create_cpp_platform_fixture.py
    --output ${CMAKE_CURRENT_BINARY_DIR}/cpp_platform_under_test.h
  DEPENDS ${X_ROOT}/cpp/x.h ${X_ROOT}/test/create_cpp_platform_fixture.py
    ${X_ROOT}/tool/generate.py VERBATIM)
ADD_CUSTOM_TARGET(x_cpp_platform_fixture DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp_platform_under_test.h)
FOREACH(PLATFORM RANGE 0 4)
  FOREACH(NARROW RANGE 0 1)
    # Unsupported platforms return ENOTSUP before checking memory sizes.
    IF(PLATFORM EQUAL 0 AND NARROW EQUAL 1)
      CONTINUE()
    ENDIF()
    SET(TARGET x_test_cpp_platform_${PLATFORM}_${NARROW})
    ADD_EXECUTABLE(${TARGET} platform_faults.cpp)
    ADD_DEPENDENCIES(${TARGET} x_cpp_platform_fixture)
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
      TEST_PLATFORM=${PLATFORM} TEST_NARROW=${NARROW} _WIN32_WINNT=0x0601)
    x_cpp_test(${TARGET})
    ADD_TEST(NAME cpp_platform_${PLATFORM}_${NARROW} COMMAND ${TARGET})
    SET_TESTS_PROPERTIES(cpp_platform_${PLATFORM}_${NARROW} PROPERTIES TIMEOUT 15 LABELS "cpp;platform")
  ENDFOREACH()
ENDFOREACH()
ADD_EXECUTABLE(x_test_cpp_platform_windows_legacy platform_faults.cpp)
ADD_DEPENDENCIES(x_test_cpp_platform_windows_legacy x_cpp_platform_fixture)
TARGET_INCLUDE_DIRECTORIES(x_test_cpp_platform_windows_legacy PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
TARGET_COMPILE_DEFINITIONS(x_test_cpp_platform_windows_legacy PRIVATE
  TEST_PLATFORM=3 TEST_NARROW=0 _WIN32_WINNT=0x0501)
x_cpp_test(x_test_cpp_platform_windows_legacy)
ADD_TEST(NAME cpp_platform_windows_legacy COMMAND x_test_cpp_platform_windows_legacy)
SET_TESTS_PROPERTIES(cpp_platform_windows_legacy PROPERTIES TIMEOUT 15 LABELS "cpp;platform")

SET(ERROR_CASES values platform check logging
  abort_unknown_value abort_unknown_capture abort_null_name abort_empty_name
  abort_unsupported_capture abort_unsupported_custom_capture
  abort_null_assertion abort_empty_assertion abort_message_assertion)
FOREACH(MODE IN ITEMS enabled disabled)
  SET(TARGET x_test_cpp_error_${MODE})
  ADD_EXECUTABLE(${TARGET} error_handling.cpp)
  TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE X_ENABLE_SOCKET=1)
  IF(MODE STREQUAL "disabled")
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE X_LOG_LEVEL=-1)
  ENDIF()
  x_cpp_test(${TARGET})
  # Only logging and check diagnostics need both verbosity configurations.
  IF(MODE STREQUAL "enabled")
    SET(CASES ${ERROR_CASES})
  ELSE()
    SET(CASES check logging)
  ENDIF()
  FOREACH(CASE IN LISTS CASES)
    IF(CASE MATCHES "^abort_")
      SET(EXPECT abort)
    ELSE()
      SET(EXPECT success)
    ENDIF()
    IF(CASE STREQUAL "check" OR CASE STREQUAL "logging")
      SET(TEST_NAME cpp_error_${MODE}_${CASE})
    ELSE()
      SET(TEST_NAME cpp_error_${CASE})
    ENDIF()
    ADD_TEST(NAME ${TEST_NAME}
      COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_error_handling.py
        --executable $<TARGET_FILE:${TARGET}> --case ${CASE} --expect ${EXPECT}
        --context ${ERROR_CONTEXT})
    SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES TIMEOUT 15 LABELS "cpp;error")
  ENDFOREACH()
ENDFOREACH()

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpp_native_error_under_test.h
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/create_native_error_fixture.py
    --lang cpp --output ${CMAKE_CURRENT_BINARY_DIR}/cpp_native_error_under_test.h
  DEPENDS ${X_ROOT}/cpp/x.h ${X_ROOT}/test/create_native_error_fixture.py
    ${X_ROOT}/tool/generate.py VERBATIM)
ADD_CUSTOM_TARGET(x_cpp_native_error_fixture
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpp_native_error_under_test.h)
FOREACH(PLATFORM IN ITEMS 0 2 3 4)
  SET(TARGET x_test_cpp_native_error_${PLATFORM})
  ADD_EXECUTABLE(${TARGET} native_error_faults.cpp)
  ADD_DEPENDENCIES(${TARGET} x_cpp_native_error_fixture)
  TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_PLATFORM=${PLATFORM})
  x_cpp_test(${TARGET})
  ADD_TEST(NAME cpp_native_error_${PLATFORM} COMMAND ${TARGET})
  SET_TESTS_PROPERTIES(cpp_native_error_${PLATFORM} PROPERTIES
    TIMEOUT 15 LABELS "cpp;error;platform")
ENDFOREACH()

ADD_EXECUTABLE(x_test_cpp_checksums checksums.cpp)
x_cpp_test(x_test_cpp_checksums)
ADD_TEST(NAME cpp_checksums COMMAND x_test_cpp_checksums)
SET_TESTS_PROPERTIES(cpp_checksums PROPERTIES TIMEOUT 20 LABELS "cpp;checksum")

# Also compile each optional backend by itself, and prove that removing the
# CUDA tools does not remove event or memory backends.
FOREACH(MODULE_CASE IN ITEMS date_time memory cuda without_cuda all full)
  FOREACH(BACKEND IN ITEMS driver runtime both)
    SET(TARGET x_test_cpp_gpu_${MODULE_CASE}_${BACKEND})
    ADD_EXECUTABLE(${TARGET} cuda_faults.cpp)
    IF(MODULE_CASE STREQUAL "full")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_FULL_HEADER=1)
    ELSE()
      ADD_DEPENDENCIES(${TARGET} x_cpp_header_${MODULE_CASE})
      TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated/${MODULE_CASE})
    ENDIF()
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/sdk_stubs)
    IF(NOT BACKEND STREQUAL "runtime")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE X_ENABLE_CUDA_DRIVER=1)
    ENDIF()
    IF(NOT BACKEND STREQUAL "driver")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE X_ENABLE_CUDA_RUNTIME=1)
    ENDIF()
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE X_ENABLE_CUBLAS=1)
    IF(MODULE_CASE STREQUAL "date_time" OR MODULE_CASE STREQUAL "without_cuda" OR MODULE_CASE STREQUAL "all")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_EVENTS=1)
    ENDIF()
    IF(MODULE_CASE STREQUAL "memory" OR MODULE_CASE STREQUAL "without_cuda" OR MODULE_CASE STREQUAL "all")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_QUERIES=1)
    ENDIF()
    IF(MODULE_CASE STREQUAL "cuda" OR MODULE_CASE STREQUAL "all" OR MODULE_CASE STREQUAL "full")
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_TOOLS=1)
    ENDIF()
    x_cpp_test(${TARGET})
    IF(MODULE_CASE STREQUAL "cuda" OR MODULE_CASE STREQUAL "all" OR MODULE_CASE STREQUAL "full")
      ADD_TEST(NAME cpp_gpu_${MODULE_CASE}_${BACKEND}
        COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_cuda_tools.py
          --executable $<TARGET_FILE:${TARGET}> --backend ${BACKEND})
    ELSE()
      ADD_TEST(NAME cpp_gpu_${MODULE_CASE}_${BACKEND} COMMAND ${TARGET})
    ENDIF()
    SET_TESTS_PROPERTIES(cpp_gpu_${MODULE_CASE}_${BACKEND} PROPERTIES TIMEOUT 15 LABELS "cpp;cuda")
  ENDFOREACH()
ENDFOREACH()
