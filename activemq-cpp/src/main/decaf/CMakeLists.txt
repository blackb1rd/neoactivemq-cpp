# Build the decaf utility library

file(GLOB_RECURSE DECAF_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

# Exclude test directories if any
list(FILTER DECAF_SOURCES EXCLUDE REGEX ".*/test/.*|.*/tests?/.*")

# Exclude platform-specific implementations that don't apply to the current
# platform. Decaf contains both `internal/security/windows/*` and
# `internal/security/unix/*` implementations; ensure we only compile the
# appropriate set for the target platform to avoid headers like <windows.h>
# being required on Linux and POSIX sources on Windows.
if(WIN32)
  list(FILTER DECAF_SOURCES EXCLUDE REGEX ".*/internal/security/unix/.*")
else()
  list(FILTER DECAF_SOURCES EXCLUDE REGEX ".*/internal/security/windows/.*")
endif()

if(AMQCPP_SHARED_LIB)
  add_library(decaf SHARED ${DECAF_SOURCES})
else()
  add_library(decaf STATIC ${DECAF_SOURCES})
endif()

# Create alias for namespaced usage
add_library(neoactivemq-cpp::decaf ALIAS decaf)

target_include_directories(decaf
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(OpenSSL_FOUND)
  target_compile_definitions(decaf PUBLIC HAVE_OPENSSL)
  target_link_libraries(decaf PUBLIC OpenSSL::SSL OpenSSL::Crypto)
endif()

# link zlib for compression support
target_link_libraries(decaf PUBLIC ZLIB::ZLIB)

set_target_properties(decaf PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

# Windows-specific settings
if(WIN32)
  # Ensure symbols are exported when building shared libs on Windows
  if(AMQCPP_SHARED_LIB)
    set_target_properties(decaf PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
  endif()
  target_link_libraries(decaf PRIVATE ws2_32)
  target_compile_definitions(decaf PRIVATE _WIN32_WINNT=0x0601)
endif()

# Ensure C++ threading is available on all platforms
find_package(Threads REQUIRED)
target_link_libraries(decaf PRIVATE Threads::Threads)

# Ensure POSIX threading macros are defined on non-Windows platforms for compatibility
if(NOT WIN32)
  # Define numeric macro so headers using `#if HAVE_PTHREAD_H` work correctly
  target_compile_definitions(decaf PUBLIC HAVE_PTHREAD_H=1)
endif()

target_link_libraries(decaf PUBLIC asio::asio)
