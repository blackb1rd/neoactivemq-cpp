cmake_minimum_required(VERSION 3.15)
project(activemq-cpp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If the repo contains a local vcpkg-style install tree, add each
# triplet prefix to CMAKE_PREFIX_PATH so find_package(... CONFIG)
# can locate package config files placed there (e.g. vcpkg_installed/x64-linux).
set(_local_vcpkg_root "${CMAKE_SOURCE_DIR}/vcpkg_installed")
if(EXISTS "${_local_vcpkg_root}")
	file(GLOB _local_triplets RELATIVE "${_local_vcpkg_root}" "${_local_vcpkg_root}/*")
	foreach(_t IN LISTS _local_triplets)
		if(IS_DIRECTORY "${_local_vcpkg_root}/${_t}")
			list(APPEND CMAKE_PREFIX_PATH "${_local_vcpkg_root}/${_t}")
		endif()
	endforeach()
endif()

find_package(apr CONFIG REQUIRED)

# Options
option(AMQCPP_BUILD_EXAMPLES "Build example programs" ON)
option(AMQCPP_BUILD_TESTS "Build tests" OFF)
option(AMQCPP_SHARED_LIB "Build shared library instead of static" OFF)

# Option to enable/disable SSL support. When ON, OpenSSL is required.
option(AMQCPP_USE_SSL "Enable OpenSSL support (required when ON)" ON)

if(AMQCPP_USE_SSL)
	message(STATUS "AMQCPP: SSL support enabled; OpenSSL will be required.")
else()
	message(STATUS "AMQCPP: SSL support disabled; OpenSSL will be optional.")
endif()
if(AMQCPP_USE_SSL)
	find_package(OpenSSL REQUIRED)
else()
	find_package(OpenSSL QUIET)
endif()

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

## Inline `src/CMakeLists.txt` content so project can be configured from
## the top-level CMake without a separate `src` directory CMakeLists.

# Build component libraries in `src/main` and tests/examples under `src`

# Add component subdirectories if present (paths are relative to project root)
if(EXISTS ${CMAKE_SOURCE_DIR}/src/main/decaf)
	add_subdirectory(src/main/decaf)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/main/activemq)
	add_subdirectory(src/main/activemq)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/main/cms)
	add_subdirectory(src/main/cms)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/examples)
	add_subdirectory(src/examples)
endif()

if(AMQCPP_BUILD_TESTS)
    find_package(CppUnit CONFIG REQUIRED)
    if(EXISTS ${CMAKE_SOURCE_DIR}/src/test)
        add_subdirectory(src/test)
    endif()
endif()
