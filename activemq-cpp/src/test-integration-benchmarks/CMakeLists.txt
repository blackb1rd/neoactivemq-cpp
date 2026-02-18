# Build integration benchmark tests under src/test-integration-benchmarks
#
# IMPORTANT: Integration benchmark tests require multiple running ActiveMQ brokers.
# Before running these tests, start all brokers using Docker Compose:
#
#   docker compose --profile failover up -d
#
# This starts:
#   - activemq1: Failover primary on port 61617
#   - activemq2: Failover secondary on port 61618
#   - activemq3: Independent broker on port 61619
#
# To run specific test suites:
#   ./neoactivemq-integration-benchmark-test -test OpenwireFailoverIntegrationTest
#   ./neoactivemq-integration-benchmark-test -test OpenwireMultiConnectionTest
#   ./neoactivemq-integration-benchmark-test -test OpenwireHighVolumeListenerTest
#
# To stop the brokers after testing:
#   docker compose --profile failover down
#
# NOTE: Integration benchmark tests are designed to run SEPARATELY from unit and
# integration tests because they:
#   - Require multiple broker instances
#   - Test high-volume scenarios (10k+ messages)
#   - May take longer to complete
#   - Test failover/reconnection behavior

# Reuse utility files from test-integration
set(TEST_INTEGRATION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test-integration)

set(INTEGRATION_BENCHMARK_TEST_SRCS
  # Top level launcher
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

  # Reuse utilities from test-integration
  ${TEST_INTEGRATION_DIR}/util/teamcity/TeamCityProgressListener.cpp
  ${TEST_INTEGRATION_DIR}/activemq/util/CMSListener.cpp
  ${TEST_INTEGRATION_DIR}/activemq/util/CMSProvider.cpp
  ${TEST_INTEGRATION_DIR}/activemq/util/IntegrationCommon.cpp

  # Integration benchmark tests - Failover and Multi-Connection
  activemq/test/openwire/OpenwireFailoverIntegrationTest.cpp
  activemq/test/openwire/OpenwireMultiConnectionTest.cpp
  activemq/test/openwire/OpenwireHighVolumeListenerTest.cpp
)

set(INTEGRATION_BENCHMARK_TEST_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Build a single integration benchmark test harness
list(REMOVE_ITEM INTEGRATION_BENCHMARK_TEST_SRCS ${INTEGRATION_BENCHMARK_TEST_MAIN})
add_executable(neoactivemq-integration-benchmark-test ${INTEGRATION_BENCHMARK_TEST_MAIN} ${INTEGRATION_BENCHMARK_TEST_SRCS})
target_include_directories(neoactivemq-integration-benchmark-test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../main
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  # Include test-integration for shared utilities
  ${TEST_INTEGRATION_DIR}
  ${TEST_INTEGRATION_DIR}/..
  # Include test directory for shared test utilities (TestWatchdog.h)
  ${CMAKE_CURRENT_SOURCE_DIR}/../test
)

# Add /bigobj flag for MSVC to handle large object files with many sections
if(MSVC)
  target_compile_options(neoactivemq-integration-benchmark-test PRIVATE /bigobj)
  # Allow duplicate symbols - templates instantiated in both DLL and test exe
  target_link_options(neoactivemq-integration-benchmark-test PRIVATE /FORCE:MULTIPLE)
endif()

# Define DLL import macros when linking against shared library on Windows
if(AMQCPP_SHARED_LIB AND WIN32)
  target_compile_definitions(neoactivemq-integration-benchmark-test PRIVATE
    DECAF_DLL
    CMS_DLL
    AMQCPP_DLL
  )
endif()

# Link to the consolidated library and Google Test
target_link_libraries(neoactivemq-integration-benchmark-test PRIVATE neoactivemq-cpp GTest::gtest)

include(StaticTestDiscovery)
static_discover_tests(neoactivemq-integration-benchmark-test
  TEST_PREFIX "integration-benchmark/"
  SOURCES
    activemq/test/openwire/OpenwireFailoverIntegrationTest.cpp
    activemq/test/openwire/OpenwireHighVolumeListenerTest.cpp
    activemq/test/openwire/OpenwireMultiConnectionTest.cpp
  LABELS integration-benchmark
  PROPERTIES
    TIMEOUT 600
    ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
)
