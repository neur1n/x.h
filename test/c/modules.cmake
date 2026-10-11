# Both the standalone C tests and the root build exercise generated headers.
SET(X_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
ADD_TEST(NAME x_header_layout
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_header_layout.py
)
ADD_TEST(NAME x_generator
  COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/test/check_generator.py
)
SET_TESTS_PROPERTIES(x_header_layout x_generator PROPERTIES
  TIMEOUT 30 LABELS modularization
)

SET(OPTIONAL_MODULES logging checksum date_time filesystem hardware math memory string cuda)
SET(MODULE_LABELS log checksum time fs hw math mem str cuda)
SET(GENERATED_CASES minimal ${OPTIONAL_MODULES} all io_bundle without_cuda core_gpu_errors)
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
  ADD_CUSTOM_COMMAND(
    OUTPUT ${HEADER}
    COMMAND ${Python3_EXECUTABLE} ${X_ROOT}/tool/generate.py
      --lang c --module ${MODULES} --output ${HEADER}
    DEPENDS ${X_ROOT}/c/x.h ${X_ROOT}/tool/generate.py
    VERBATIM
  )
  ADD_CUSTOM_TARGET(x_header_${CASE} DEPENDS ${HEADER})

  # Expected metadata is obtained from the maintained source's version, rather
  # than introducing another version definition into the build configuration.
  FILE(STRINGS ${X_ROOT}/c/x.h VERSION_LINE REGEX "^#define X_H x_version")
  STRING(REGEX REPLACE ".*x_version\\(([0-9]+), ([0-9]+), ([0-9]+)\\).*"
    "\\1.\\2.\\3" VERSION "${VERSION_LINE}")
  SET(DESCRIPTION "${VERSION}+core")
  FOREACH(MODULE IN LISTS OPTIONAL_MODULES)
    IF(MODULE IN_LIST MODULES OR CASE STREQUAL "all")
      LIST(FIND OPTIONAL_MODULES ${MODULE} MODULE_INDEX)
      LIST(GET MODULE_LABELS ${MODULE_INDEX} LABEL)
      STRING(APPEND DESCRIPTION ".${LABEL}")
    ENDIF()
  ENDFOREACH()

  FOREACH(STANDARD IN ITEMS 99 11)
    SET(TARGET x_test_modules_${CASE}_c${STANDARD})
    ADD_EXECUTABLE(${TARGET} smoke.c generated_peer.c)
    ADD_DEPENDENCIES(${TARGET} x_header_${CASE})
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${HEADER_DIR})
    TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_DESCRIPTION="${DESCRIPTION}")
    FOREACH(MODULE IN LISTS OPTIONAL_MODULES)
      IF(MODULE IN_LIST MODULES OR CASE STREQUAL "all")
        STRING(TOUPPER ${MODULE} DEFINE)
        TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE TEST_${DEFINE}=1)
      ENDIF()
    ENDFOREACH()
    IF(CASE STREQUAL "core_gpu_errors")
      TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/sdk_stubs)
      TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
        X_ENABLE_CUDA_DRIVER=1 X_ENABLE_CUDA_RUNTIME=1 X_ENABLE_CUBLAS=1
      )
    ENDIF()
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES
      C_STANDARD ${STANDARD} C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    )
    IF(MSVC)
      TARGET_COMPILE_OPTIONS(${TARGET} PRIVATE /W4 /WX)
    ELSE()
      # x_assert intentionally uses the compiler's extended function signature.
      TARGET_COMPILE_OPTIONS(${TARGET} PRIVATE -Wall -Wextra -Werror)
    ENDIF()
    IF(UNIX)
      TARGET_LINK_LIBRARIES(${TARGET} PRIVATE m)
    ENDIF()
    ADD_TEST(NAME modules_${CASE}_c${STANDARD} COMMAND ${TARGET})
    SET_TESTS_PROPERTIES(modules_${CASE}_c${STANDARD} PROPERTIES
      TIMEOUT 15 LABELS modularization
    )
  ENDFOREACH()
ENDFOREACH()
