# cmake/ClangFormat.cmake
# clang-format integration for neoactivemq-cpp.
#
# Provides two targets:
#   clang-format-check  -- dry-run check, exits non-zero if any file differs (CI)
#   clang-format-fix    -- apply formatting in-place

option(AMQCPP_ENABLE_CLANG_FORMAT
    "Add clang-format check/fix targets"
    ON)

if(NOT AMQCPP_ENABLE_CLANG_FORMAT)
    return()
endif()

find_program(CLANG_FORMAT_EXE
    NAMES clang-format-18 clang-format-17 clang-format-16 clang-format
    DOC "Path to clang-format executable")

if(NOT CLANG_FORMAT_EXE)
    message(STATUS
        "clang-format not found. "
        "clang-format-check and clang-format-fix targets unavailable.")
    return()
endif()

message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")

file(GLOB_RECURSE _fmt_sources
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/main/*.cpp"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/main/*.h"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/test/*.cpp"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/test/*.h"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/test-integration/*.cpp"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/test-integration/*.h"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/examples/*.cpp"
    "${CMAKE_SOURCE_DIR}/activemq-cpp/src/examples/*.h")

# Check: exits non-zero if any file is incorrectly formatted (use in CI)
add_custom_target(clang-format-check
    COMMAND "${CLANG_FORMAT_EXE}"
        --dry-run
        --Werror
        --style=file
        ${_fmt_sources}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Checking code formatting with clang-format...")

# Fix: apply formatting in-place
add_custom_target(clang-format-fix
    COMMAND "${CLANG_FORMAT_EXE}"
        -i
        --style=file
        ${_fmt_sources}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Applying clang-format to all sources...")
