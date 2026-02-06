# Build benchmarks under src/test-benchmarks

set(BENCHMARK_SRCS
  # Top level launcher / registry
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testRegistry.cpp

  # Benchmark utilities
  benchmark/PerformanceTimer.cpp

  # ActiveMQ benchmarks
  activemq/util/PrimitiveMapBenchmark.cpp

  # Decaf I/O benchmarks
  decaf/io/BufferedInputStreamBenchmark.cpp
  decaf/io/ByteArrayInputStreamBenchmark.cpp
  decaf/io/ByteArrayOutputStreamBenchmark.cpp
  decaf/io/DataInputStreamBenchmark.cpp
  decaf/io/DataOutputStreamBenchmark.cpp

  # Decaf language benchmarks
  decaf/lang/BooleanBenchmark.cpp
  decaf/lang/ThreadBenchmark.cpp

  # Decaf utility benchmarks
  decaf/util/HashMapBenchmark.cpp
  decaf/util/LinkedListBenchmark.cpp
  decaf/util/PropertiesBenchmark.cpp
  decaf/util/QueueBenchmark.cpp
  decaf/util/SetBenchmark.cpp
  decaf/util/StlListBenchmark.cpp
  decaf/util/StlMapBenchmark.cpp
)

set(BENCHMARK_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Build a single benchmark harness driven by the top-level main.cpp
list(REMOVE_ITEM BENCHMARK_SRCS ${BENCHMARK_MAIN})
add_executable(neoactivemq-benchmark ${BENCHMARK_MAIN} ${BENCHMARK_SRCS})
target_include_directories(neoactivemq-benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../main
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  # Include test directory for shared test utilities (TestWatchdog.h)
  ${CMAKE_CURRENT_SOURCE_DIR}/../test
)

# Add /bigobj flag for MSVC to handle large object files with many sections
if(MSVC)
  target_compile_options(neoactivemq-benchmark PRIVATE /bigobj)
  # Allow duplicate symbols - templates instantiated in both DLL and test exe
  target_link_options(neoactivemq-benchmark PRIVATE /FORCE:MULTIPLE)
endif()

# Define DLL import macros when linking against shared library on Windows
# This is required to import static const members from the DLL
# On Linux/Unix, symbol visibility is handled differently (no __declspec needed)
if(AMQCPP_SHARED_LIB AND WIN32)
  target_compile_definitions(neoactivemq-benchmark PRIVATE
    DECAF_DLL
    CMS_DLL
    AMQCPP_DLL
  )
endif()

# Link to the consolidated library and Google Test
target_link_libraries(neoactivemq-benchmark PRIVATE neoactivemq-cpp GTest::gtest)

include(StaticTestDiscovery)
static_discover_tests(neoactivemq-benchmark
  TEST_PREFIX "benchmark/"
  SOURCES testRegistry.cpp
  PROPERTIES
    ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
)
