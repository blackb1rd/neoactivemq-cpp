# Discovers GTest tests by parsing source files at configure time.
# This avoids runtime discovery (gtest_discover_tests) which requires
# running the test executable, making it compatible with split build/test CI
# pipelines where artifacts are built on one runner and tested on another.
#
# Usage:
#   static_discover_tests(my_test_target
#     TEST_PREFIX "unit/"
#     SOURCES testRegistry.cpp other_test.cpp
#     PROPERTIES
#       TIMEOUT 300
#       ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
#   )
function(static_discover_tests TARGET)
    cmake_parse_arguments(ARG "" "TEST_PREFIX" "SOURCES;PROPERTIES" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "static_discover_tests: SOURCES is required")
    endif()

    set(test_count 0)

    foreach(source IN LISTS ARG_SOURCES)
        if(NOT IS_ABSOLUTE "${source}")
            set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        endif()

        file(STRINGS "${source}" lines REGEX "^[ \t]*TEST(_F)?[ \t]*\\(")

        foreach(line IN LISTS lines)
            if(line MATCHES "TEST(_F)?[ \t]*\\([ \t]*([A-Za-z_][A-Za-z0-9_]*)[ \t]*,[ \t]*([A-Za-z_][A-Za-z0-9_]*)[ \t]*\\)")
                set(suite "${CMAKE_MATCH_2}")
                set(name "${CMAKE_MATCH_3}")
                set(test_name "${ARG_TEST_PREFIX}${suite}.${name}")

                add_test(NAME "${test_name}" COMMAND ${TARGET} "--gtest_filter=${suite}.${name}")

                if(ARG_PROPERTIES)
                    set_tests_properties("${test_name}" PROPERTIES ${ARG_PROPERTIES})
                endif()

                math(EXPR test_count "${test_count} + 1")
            endif()
        endforeach()
    endforeach()

    message(STATUS "static_discover_tests: Found ${test_count} tests for target ${TARGET}")
endfunction()
