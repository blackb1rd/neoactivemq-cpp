cmake_minimum_required(VERSION 3.15)

# Enable MSVC debug information format control (requires CMake 3.25+)
# This allows CMAKE_MSVC_DEBUG_INFORMATION_FORMAT to work properly
# Without this, /Z7 (embedded debug info) won't be applied and parallel builds
# with sccache will fail due to PDB file locking conflicts
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

# Try to get version from git tag first, fall back to vcpkg.json
# This allows the build to use git tags (e.g., v1.2.0) when available
find_package(Git QUIET)
set(VERSION_FROM_GIT FALSE)

if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # Check if working directory is dirty (has uncommitted changes)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status --porcelain
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(GIT_STATUS)
        # Working directory is dirty - use development version
        set(PROJECT_VERSION "0.0.0")
        set(VERSION_FROM_GIT TRUE)
        message(STATUS "Version: ${PROJECT_VERSION} (dirty working directory - uncommitted changes)")
    else()
        # Try to get version from git describe (looks for tags like v1.2.3 or 1.2.3)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --match "v[0-9]*" --abbrev=0
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            RESULT_VARIABLE GIT_RESULT
        )

        if(GIT_RESULT EQUAL 0 AND GIT_TAG)
            # Remove 'v' prefix if present (v1.2.3 -> 1.2.3)
            string(REGEX REPLACE "^v" "" PROJECT_VERSION "${GIT_TAG}")
            set(VERSION_FROM_GIT TRUE)
            message(STATUS "Version from git tag: ${PROJECT_VERSION}")
        endif()
    endif()
endif()

if(NOT VERSION_FROM_GIT)
    # Fall back to vcpkg.json
    file(READ "${CMAKE_SOURCE_DIR}/vcpkg.json" VCPKG_JSON)
    string(JSON PROJECT_VERSION GET "${VCPKG_JSON}" "version-string")

    # Check if vcpkg.json has placeholder version (0.0.0 means "use git tag")
    if(PROJECT_VERSION STREQUAL "0.0.0")
        # Use development version when no git tag and placeholder in vcpkg.json
        set(PROJECT_VERSION "0.0.0")
        message(STATUS "Version: ${PROJECT_VERSION} (development build - no git tag found)")
    else()
        message(STATUS "Version from vcpkg.json: ${PROJECT_VERSION}")
    endif()
endif()

if(NOT PROJECT_VERSION)
    message(FATAL_ERROR "Failed to determine project version from git tag or vcpkg.json")
endif()

project(neoactivemq-cpp VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

# Configure version header - extract version components for the header
if(NOT DEFINED PROJECT_VERSION_PATCH)
    set(PROJECT_VERSION_PATCH 0)
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/activemq-cpp/src/main/activemq/util/Version.h.in
    ${CMAKE_BINARY_DIR}/generated/activemq/util/Version.h
    @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable compiler cache (ccache/sccache)
# Default OFF - vcpkg and other build systems manage their own compiler cache
option(AMQCPP_WITH_CCACHE "Enable compiler cache (ccache/sccache) for faster rebuilds" OFF)

# Enable compiler caching if available (ccache or sccache)
# Only when:
# - Explicitly enabled via AMQCPP_WITH_CCACHE
# - Compiler launcher is not already configured by user or build system
if(AMQCPP_WITH_CCACHE AND NOT CMAKE_C_COMPILER_LAUNCHER)
    # Prefer sccache if both are available
    find_program(SCCACHE_PROGRAM sccache)
    find_program(CCACHE_PROGRAM ccache)

    if(SCCACHE_PROGRAM)
        set(CACHE_PROGRAM ${SCCACHE_PROGRAM})
        set(CACHE_NAME "sccache")
    elseif(CCACHE_PROGRAM)
        set(CACHE_PROGRAM ${CCACHE_PROGRAM})
        set(CACHE_NAME "ccache")
    endif()
endif()

if(CACHE_PROGRAM)
    message(STATUS "Found ${CACHE_NAME}: ${CACHE_PROGRAM}")
    # Verify the cache program is actually executable before setting it as launcher
    execute_process(
        COMMAND ${CACHE_PROGRAM} --version
        RESULT_VARIABLE CACHE_TEST_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(CACHE_TEST_RESULT EQUAL 0)
        set(CMAKE_C_COMPILER_LAUNCHER "${CACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CACHE_PROGRAM}")
        message(STATUS "${CACHE_NAME} enabled for compiler caching")
        # Set cache directory for local builds only
        # In CI, sccache-action manages the cache via SCCACHE_GHA_ENABLED
        if(NOT DEFINED ENV{SCCACHE_GHA_ENABLED} AND NOT DEFINED ENV{CI})
            if(CACHE_NAME STREQUAL "sccache")
                set(ENV{SCCACHE_DIR} "${CMAKE_SOURCE_DIR}/.cache/sccache")
            elseif(CACHE_NAME STREQUAL "ccache")
                set(ENV{CCACHE_DIR} "${CMAKE_SOURCE_DIR}/.cache/ccache")
            endif()
        endif()
    else()
        message(STATUS "${CACHE_NAME} found but not working - builds will proceed without caching")
    endif()
else()
    message(STATUS "No compiler cache (ccache/sccache) found - builds will proceed without caching")
endif()

# Platform-specific debug flags
if(MSVC)
    # Only set debug information format when using compiler cache
    # This avoids PDB locking issues with parallel builds
    if(AMQCPP_WITH_CCACHE)
        # Windows/MSVC: Use /Z7 (embedded debug info in .obj files) for all builds
        # This avoids PDB locking issues with parallel builds and works better with sccache
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,Release>:Embedded>")

        # Remove default /Zi flags to avoid "overriding /Zi with /Z7" warning (D9025)
        foreach(flag_var CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
                         CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO)
            string(REGEX REPLACE "/Z[iI]" "" ${flag_var} "${${flag_var}}")
        endforeach()
    endif()
    # Note: CMake already sets /Od for Debug and /O2 for Release by default
else()
    # -rdynamic is Linux-specific (exports symbols for backtraces)
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-rdynamic")
endif()

# Output directories for binaries and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(asio CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

# Options
option(AMQCPP_BUILD_EXAMPLES "Build example programs" ON)
option(AMQCPP_BUILD_TESTS "Build all tests (unit, benchmark, integration)" OFF)
option(AMQCPP_SHARED_LIB "Build shared library instead of static" ON)

# Option to enable/disable SSL support. When ON, OpenSSL is required.
# Default is OFF to make SSL opt-in rather than opt-out.
option(AMQCPP_USE_SSL "Enable OpenSSL support (required when ON)" OFF)

if(AMQCPP_USE_SSL)
	find_package(OpenSSL REQUIRED)
	message(STATUS "AMQCPP: SSL support enabled; OpenSSL will be required.")

	# If OpenSSL was found, define HAVE_OPENSSL for the code so sources
	# can use `#ifdef HAVE_OPENSSL` to enable OpenSSL-dependent features.
	if(OpenSSL_FOUND)
		message(STATUS "AMQCPP: Found OpenSSL ${OPENSSL_VERSION}")
		add_compile_definitions(HAVE_OPENSSL)
		set(AMQCPP_HAVE_OPENSSL ON CACHE INTERNAL "OpenSSL available")
	else()
		message(STATUS "AMQCPP: OpenSSL not found; HAVE_OPENSSL will not be defined")
		set(AMQCPP_HAVE_OPENSSL OFF CACHE INTERNAL "OpenSSL available")
	endif()
else()
	find_package(OpenSSL QUIET)
	message(STATUS "AMQCPP: SSL support disabled; OpenSSL will be optional.")
endif()

## Inline `src/CMakeLists.txt` content so project can be configured from
## the top-level CMake without a separate `src` directory CMakeLists.

# Build component libraries in `src/main` and tests/examples under `src`

# Add the main library (consolidated build from activemq-cpp/src/main)
add_subdirectory(activemq-cpp/src/main)

# The main library target is now named neoactivemq-cpp (defined in activemq-cpp/src/main/CMakeLists.txt)
# The alias neoactivemq-cpp::neoactivemq-cpp is already created there

add_subdirectory(activemq-cpp/src/examples)

if(AMQCPP_BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)

	list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
	enable_testing()

	add_subdirectory(activemq-cpp/src/test)
	add_subdirectory(activemq-cpp/src/test-benchmarks)
	add_subdirectory(activemq-cpp/src/test-integration)
	add_subdirectory(activemq-cpp/src/test-integration-benchmarks)

	# =========================================================================
	# Integration & System Test Workflow Targets
	# These targets automate the full Docker lifecycle + test execution.
	# =========================================================================

	# Integration tests: start single broker, run tests, stop broker
	add_custom_target(integration-full
		COMMAND docker compose up -d
		COMMAND ${CMAKE_COMMAND} -E sleep 10
		COMMAND ${CMAKE_COMMAND} -E echo "Running integration tests..."
		COMMAND $<TARGET_FILE:neoactivemq-integration-test>
		COMMAND docker compose down
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS neoactivemq-integration-test
		COMMENT "Full integration test workflow (start broker, test, stop broker)..."
	)

	# Integration benchmark tests: start all brokers (failover profile), run tests, stop brokers
	add_custom_target(integration-benchmark-full
		COMMAND docker compose --profile failover up -d
		COMMAND ${CMAKE_COMMAND} -E sleep 15
		COMMAND ${CMAKE_COMMAND} -E echo "Running integration benchmark tests..."
		COMMAND $<TARGET_FILE:neoactivemq-integration-benchmark-test>
		COMMAND docker compose --profile failover down
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS neoactivemq-integration-benchmark-test
		COMMENT "Full integration benchmark test workflow (start all brokers, test, stop brokers)..."
	)

	message(STATUS "Test workflow targets:")
	message(STATUS "  cmake --build --preset <preset> --target integration-full            # Integration tests (auto Docker)")
	message(STATUS "  cmake --build --preset <preset> --target integration-benchmark-full  # Integration benchmark tests (auto Docker)")
endif()

# Installation rules
include(GNUInstallDirs)

# Install library
install(TARGETS neoactivemq-cpp
	EXPORT neoactivemq-cpp-targets
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install header files
install(DIRECTORY
	${CMAKE_SOURCE_DIR}/activemq-cpp/src/main/activemq
	${CMAKE_SOURCE_DIR}/activemq-cpp/src/main/cms
	${CMAKE_SOURCE_DIR}/activemq-cpp/src/main/decaf
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install generated headers (Version.h)
install(DIRECTORY
	${CMAKE_BINARY_DIR}/generated/activemq
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING PATTERN "*.h"
)

# Install CMake config files for find_package() support
include(CMakePackageConfigHelpers)

# Generate the config file that includes the exports
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/neoactivemq-cpp-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/neoactivemq-cpp-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/neoactivemq-cpp
)

# Generate the version file for the config file
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/neoactivemq-cpp-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

# Install the configuration files
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/neoactivemq-cpp-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/neoactivemq-cpp-config-version.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/neoactivemq-cpp
)

# Export the targets to a script
install(EXPORT neoactivemq-cpp-targets
	FILE neoactivemq-cpp-targets.cmake
	NAMESPACE neoactivemq-cpp::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/neoactivemq-cpp
)

