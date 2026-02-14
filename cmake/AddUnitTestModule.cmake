# Helper function to create a unit test module executable.
# Encapsulates the boilerplate shared across all unit test modules:
#   - Shared main.cpp and TeamCityProgressListener
#   - Standard include directories
#   - MSVC /bigobj and /FORCE:MULTIPLE flags
#   - DLL import definitions for Windows shared builds
#   - Link to neoactivemq-cpp and GTest
#   - Static test discovery registration with CTest
#
# Usage:
#   add_unit_test_module(
#     NAME neoactivemq-unit-activemq-core
#     SOURCES
#       activemq/core/ActiveMQConnectionTest.cpp
#       activemq/core/ActiveMQSessionTest.cpp
#     EXTRA_SOURCES
#       activemq/mock/MockBrokerService.cpp
#   )
#
# SOURCES:       Test files containing TEST_F/TEST macros. Passed to both
#                add_executable and static_discover_tests for CTest registration.
# EXTRA_SOURCES: Support files (mocks, utilities) that must be compiled but
#                contain no test macros. Passed only to add_executable.
#
function(add_unit_test_module)
    cmake_parse_arguments(ARG "" "NAME" "SOURCES;EXTRA_SOURCES" ${ARGN})

    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_unit_test_module: NAME is required")
    endif()
    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "add_unit_test_module: SOURCES is required")
    endif()

    # Shared infrastructure files included in every module
    set(_COMMON_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/util/teamcity/TeamCityProgressListener.cpp
    )

    add_executable(${ARG_NAME}
        ${_COMMON_SRCS}
        ${ARG_SOURCES}
        ${ARG_EXTRA_SOURCES}
    )

    target_include_directories(${ARG_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../main
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    )

    # Add /bigobj flag for MSVC to handle large object files with many sections
    if(MSVC)
        target_compile_options(${ARG_NAME} PRIVATE /bigobj)
        # Allow duplicate symbols - templates instantiated in both DLL and test exe
        target_link_options(${ARG_NAME} PRIVATE /FORCE:MULTIPLE)
    endif()

    # Define DLL import macros when linking against shared library on Windows
    if(AMQCPP_SHARED_LIB AND WIN32)
        target_compile_definitions(${ARG_NAME} PRIVATE
            DECAF_DLL
            CMS_DLL
            AMQCPP_DLL
        )
    endif()

    target_link_libraries(${ARG_NAME} PRIVATE neoactivemq-cpp GTest::gtest)

    # Register tests with CTest -- pass only test source files (SOURCES),
    # not support files (EXTRA_SOURCES) which contain no TEST_F macros.
    include(StaticTestDiscovery)
    static_discover_tests(${ARG_NAME}
        TEST_PREFIX "unit/"
        SOURCES ${ARG_SOURCES}
        PROPERTIES
            TIMEOUT 330
            ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
    )
endfunction()
