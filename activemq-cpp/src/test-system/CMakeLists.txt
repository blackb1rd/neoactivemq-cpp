# Build system tests under src/test-system
#
# IMPORTANT: System tests require multiple running ActiveMQ brokers.
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
#   ./neoactivemq-system-test -test OpenwireFailoverIntegrationTest
#   ./neoactivemq-system-test -test OpenwireMultiConnectionTest
#   ./neoactivemq-system-test -test OpenwireHighVolumeListenerTest
#
# To stop the brokers after testing:
#   docker compose --profile failover down
#
# NOTE: System tests are designed to run SEPARATELY from unit and integration
# tests because they:
#   - Require multiple broker instances
#   - Test high-volume scenarios (10k+ messages)
#   - May take longer to complete
#   - Test failover/reconnection behavior

# Reuse utility files from test-integration
set(TEST_INTEGRATION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test-integration)

set(SYSTEM_TEST_SRCS
  # Top level launcher
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

  # Reuse utilities from test-integration
  ${TEST_INTEGRATION_DIR}/util/teamcity/TeamCityProgressListener.cpp
  ${TEST_INTEGRATION_DIR}/activemq/util/CMSListener.cpp
  ${TEST_INTEGRATION_DIR}/activemq/util/CMSProvider.cpp
  ${TEST_INTEGRATION_DIR}/activemq/util/IntegrationCommon.cpp

  # System tests - Failover and Multi-Connection
  activemq/test/openwire/OpenwireFailoverIntegrationTest.cpp
  activemq/test/openwire/OpenwireMultiConnectionTest.cpp
  activemq/test/openwire/OpenwireHighVolumeListenerTest.cpp
)

set(SYSTEM_TEST_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Build a single system test harness
list(REMOVE_ITEM SYSTEM_TEST_SRCS ${SYSTEM_TEST_MAIN})
add_executable(neoactivemq-system-test ${SYSTEM_TEST_MAIN} ${SYSTEM_TEST_SRCS})
target_include_directories(neoactivemq-system-test PRIVATE
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
  target_compile_options(neoactivemq-system-test PRIVATE /bigobj)
  # Allow duplicate symbols - templates instantiated in both DLL and test exe
  target_link_options(neoactivemq-system-test PRIVATE /FORCE:MULTIPLE)
endif()

# Define DLL import macros when linking against shared library on Windows
if(AMQCPP_SHARED_LIB AND WIN32)
  target_compile_definitions(neoactivemq-system-test PRIVATE
    DECAF_DLL
    CMS_DLL
    AMQCPP_DLL
  )
endif()

# Link to the consolidated library and CppUnit
target_link_libraries(neoactivemq-system-test PRIVATE neoactivemq-cpp CppUnit)

# Note: System tests are NOT registered with CTest by default because they
# require multiple running ActiveMQ brokers. To run system tests manually:
#
# 1. Start all brokers: docker compose --profile failover up -d
# 2. Run: ./neoactivemq-system-test
# 3. Stop the brokers when done: docker compose --profile failover down
#
# To register with CTest (for CI), uncomment the following:
# add_test(NAME activemq-system-tests COMMAND neoactivemq-system-test)
# set_tests_properties(activemq-system-tests PROPERTIES
#   TIMEOUT 1800  # 30 minutes for system tests
#   LABELS "system"
# )
