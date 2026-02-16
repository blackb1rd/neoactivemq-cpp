# Discovers GTest tests by parsing source files at configure time.
# This avoids runtime discovery (gtest_discover_tests) which requires
# running the test executable, making it compatible with split build/test CI
# pipelines where artifacts are built on one runner and tested on another.
#
# By default, tests are grouped by suite (one CTest entry per GTest suite),
# which dramatically reduces process startup overhead. Use GROUP_MODE INDIVIDUAL
# to register one CTest entry per test case instead.
#
# Tests are grouped by namespace, producing hierarchical names like:
#   unit/activemq/commands/BrokerInfoTest   (GROUP_MODE SUITE, default)
#   unit/activemq/commands/BrokerInfoTest.test  (GROUP_MODE INDIVIDUAL)
#
# Optionally writes a JSON file listing unique namespace groups for CI matrix:
#   ["activemq/commands","activemq/core","decaf/io",...]
#
# Usage:
#   static_discover_tests(my_test_target
#     TEST_PREFIX "unit/"
#     SOURCES testRegistry.cpp other_test.cpp
#     GROUP_MODE SUITE          # SUITE (default) or INDIVIDUAL
#     GROUPS_OUTPUT_FILE "${CMAKE_BINARY_DIR}/test-groups.json"
#     LABELS unit activemq core  # CTest labels for filtering (ctest -L activemq)
#     PROPERTIES
#       TIMEOUT 300
#       ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
#   )
function(static_discover_tests TARGET)
    cmake_parse_arguments(ARG "" "TEST_PREFIX;GROUP_MODE;GROUPS_OUTPUT_FILE" "SOURCES;PROPERTIES;LABELS" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "static_discover_tests: SOURCES is required")
    endif()

    if(NOT ARG_GROUP_MODE)
        set(ARG_GROUP_MODE "INDIVIDUAL")
    endif()

    # Collect all suites with their namespace groups.
    # For SUITE mode we deduplicate; for INDIVIDUAL we register each test.
    set(test_count 0)
    set(_all_suites "")
    set(_all_groups "")

    foreach(source IN LISTS ARG_SOURCES)
        if(NOT IS_ABSOLUTE "${source}")
            set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        endif()

        file(READ "${source}" _content)

        # Find all TEST_F(Suite, Name) matches using whole-file regex.
        # This avoids CMake's semicolon/list issues with line-by-line parsing.
        # The match pattern captures "TEST_F(Suite, Name)" without semicolons,
        # so results are safe to store in a CMake list.
        string(REGEX MATCHALL "TEST(_F)?\\([A-Za-z_][A-Za-z0-9_]*[ \t]*,[ \t]*[A-Za-z_][A-Za-z0-9_]*[ \t]*\\)" _tf_matches "${_content}")

        if(NOT _tf_matches)
            continue()
        endif()

        # Determine the namespace group for this file.
        # Strategy 1: Find namespace declarations immediately preceding TEST_F
        # blocks (e.g., "namespace activemq { namespace commands {\nTEST_F(").
        # Strategy 2 (fallback): Derive from file path relative to source dir.
        # For "activemq/commands/BrokerIdTest.cpp" -> "activemq/commands".
        set(_file_group "")
        string(REGEX MATCHALL "namespace[^\n}]*\\{[^\n]*\nTEST(_F)?\\(" _ns_tf_matches "${_content}")
        if(_ns_tf_matches)
            # Use the last namespace-TEST_F match (handles files with code
            # above the TEST_F block that also has namespace declarations)
            list(LENGTH _ns_tf_matches _ns_count)
            math(EXPR _last_idx "${_ns_count} - 1")
            list(GET _ns_tf_matches ${_last_idx} _last_ns_match)

            string(REGEX MATCHALL "namespace[ \t]+[A-Za-z_][A-Za-z0-9_]*" _ns_parts "${_last_ns_match}")
            if(_ns_parts)
                foreach(ns IN LISTS _ns_parts)
                    string(REGEX REPLACE "namespace[ \t]+" "" ns_name "${ns}")
                    if(_file_group)
                        set(_file_group "${_file_group}/${ns_name}")
                    else()
                        set(_file_group "${ns_name}")
                    endif()
                endforeach()
            endif()
        endif()

        # Fallback: derive group from file path relative to source directory
        if(NOT _file_group)
            file(RELATIVE_PATH _rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
            # Convert backslashes to forward slashes (Windows paths)
            string(REPLACE "\\" "/" _rel_path "${_rel_path}")
            # Strip the filename, keeping only the directory part
            get_filename_component(_rel_dir "${_rel_path}" DIRECTORY)
            if(_rel_dir)
                set(_file_group "${_rel_dir}")
            endif()
        endif()

        # Collect unique namespace groups for CI matrix output
        if(_file_group AND NOT "${_file_group}" IN_LIST _all_groups)
            list(APPEND _all_groups "${_file_group}")
        endif()

        # Process each TEST_F match
        foreach(tf IN LISTS _tf_matches)
            # Extract Suite and Name from "TEST_F(Suite, Name)" or "TEST(Suite, Name)"
            string(REGEX REPLACE "TEST(_F)?\\(([A-Za-z_][A-Za-z0-9_]*)[ \t]*,[ \t]*([A-Za-z_][A-Za-z0-9_]*)[ \t]*\\)" "\\2" _suite "${tf}")
            string(REGEX REPLACE "TEST(_F)?\\(([A-Za-z_][A-Za-z0-9_]*)[ \t]*,[ \t]*([A-Za-z_][A-Za-z0-9_]*)[ \t]*\\)" "\\3" _name "${tf}")

            if(ARG_GROUP_MODE STREQUAL "INDIVIDUAL")
                if(_file_group)
                    set(test_name "${ARG_TEST_PREFIX}${_file_group}/${_suite}.${_name}")
                else()
                    set(test_name "${ARG_TEST_PREFIX}${_suite}.${_name}")
                endif()

                add_test(NAME "${test_name}" COMMAND ${TARGET} "--gtest_filter=${_suite}.${_name}")

                if(ARG_PROPERTIES)
                    set_tests_properties("${test_name}" PROPERTIES ${ARG_PROPERTIES})
                endif()
                if(ARG_LABELS)
                    set_tests_properties("${test_name}" PROPERTIES LABELS "${ARG_LABELS}")
                endif()

                math(EXPR test_count "${test_count} + 1")
            else()
                # SUITE mode: collect unique (group, suite) pairs
                if(_file_group)
                    set(suite_key "${_file_group}/${_suite}")
                else()
                    set(suite_key "${_suite}")
                endif()

                if(NOT "${suite_key}" IN_LIST _all_suites)
                    list(APPEND _all_suites "${suite_key}")
                endif()
            endif()
        endforeach()
    endforeach()

    # In SUITE mode, register one CTest entry per suite
    if(ARG_GROUP_MODE STREQUAL "SUITE")
        foreach(suite_key IN LISTS _all_suites)
            set(test_name "${ARG_TEST_PREFIX}${suite_key}")

            # Extract the suite class name (last path component)
            string(REGEX REPLACE ".*/([^/]+)$" "\\1" suite_class "${suite_key}")
            # Handle case with no namespace prefix
            if(suite_class STREQUAL suite_key AND NOT suite_key MATCHES "/")
                set(suite_class "${suite_key}")
            endif()

            add_test(NAME "${test_name}" COMMAND ${TARGET} "--gtest_filter=${suite_class}.*")

            if(ARG_PROPERTIES)
                set_tests_properties("${test_name}" PROPERTIES ${ARG_PROPERTIES})
            endif()
            if(ARG_LABELS)
                set_tests_properties("${test_name}" PROPERTIES LABELS "${ARG_LABELS}")
            endif()

            math(EXPR test_count "${test_count} + 1")
        endforeach()
    endif()

    # Write unique namespace groups to JSON file for CI matrix consumption
    if(ARG_GROUPS_OUTPUT_FILE)
        list(SORT _all_groups)
        set(_json "[")
        set(_first TRUE)
        foreach(group IN LISTS _all_groups)
            if(_first)
                set(_first FALSE)
            else()
                string(APPEND _json ",")
            endif()
            string(APPEND _json "\"${group}\"")
        endforeach()
        string(APPEND _json "]")
        file(WRITE "${ARG_GROUPS_OUTPUT_FILE}" "${_json}\n")
        message(STATUS "static_discover_tests: Wrote ${_json} to ${ARG_GROUPS_OUTPUT_FILE}")
    endif()

    message(STATUS "static_discover_tests: Found ${test_count} tests for target ${TARGET}")
endfunction()
