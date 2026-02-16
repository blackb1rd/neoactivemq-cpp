# Build integration tests under src/test-integration
#
# IMPORTANT: Integration tests require a running ActiveMQ broker.
# Before running these tests, start the broker using Docker Compose:
#
#   docker-compose up -d
#
# Or using the helper script:
#   ./start-activemq-docker.sh
#
# Or manually with docker run:
#   docker run -d --rm --name activemq-test \
#     -p 61616:61616 -p 61613:61613 -p 8161:8161 \
#     apache/activemq-classic:5.18.7
#
# NOTE: Failover, multi-connection, and high-volume tests have been moved to
# test-system directory. Run those with:
#   docker compose --profile failover up -d
#   ./neoactivemq-system-test
#
# To stop the broker after testing:
#   docker-compose down
#   # OR: docker stop activemq-test

set(INTEGRATION_TEST_SRCS
  # Top level launcher
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

  # Utilities and support
  util/teamcity/TeamCityProgressListener.cpp
  activemq/util/CMSListener.cpp
  activemq/util/CMSProvider.cpp
  activemq/util/IntegrationCommon.cpp

  # Base integration tests
  activemq/test/AdvisoryTest.cpp
  activemq/test/AsyncSenderTest.cpp
  activemq/test/BulkMessageTest.cpp
  activemq/test/CmsConnectionStartStopTest.cpp
  activemq/test/CmsSendWithAsyncCallbackTest.cpp
  activemq/test/CmsTemplateTest.cpp
  activemq/test/DurableTest.cpp
  activemq/test/ExpirationTest.cpp
  activemq/test/JmsMessageGroupsTest.cpp
  activemq/test/MapMessageTest.cpp
  activemq/test/MessageCompressionTest.cpp
  activemq/test/MessagePriorityTest.cpp
  activemq/test/QueueBrowserTest.cpp
  activemq/test/SimpleRollbackTest.cpp
  activemq/test/SimpleTest.cpp
  activemq/test/SlowListenerTest.cpp
  activemq/test/TransactionTest.cpp
  activemq/test/VirtualTopicTest.cpp

  # OpenWire protocol tests
  activemq/test/openwire/OpenwireAdvisoryTest.cpp
  activemq/test/openwire/OpenwireAsyncSenderTest.cpp
  activemq/test/openwire/OpenwireClientAckTest.cpp
  activemq/test/openwire/OpenwireCmsConnectionStartStopTest.cpp
  activemq/test/openwire/OpenWireCmsSendWithAsyncCallbackTest.cpp
  activemq/test/openwire/OpenwireCmsTemplateTest.cpp
  activemq/test/openwire/OpenwireDurableTest.cpp
  activemq/test/openwire/OpenwireEnhancedConnectionTest.cpp
  activemq/test/openwire/OpenwireExpirationTest.cpp
  activemq/test/openwire/OpenwireIndividualAckTest.cpp
  activemq/test/openwire/OpenwireJmsMessageGroupsTest.cpp
  activemq/test/openwire/OpenwireJmsRecoverTest.cpp
  activemq/test/openwire/OpenwireMapMessageTest.cpp
  activemq/test/openwire/OpenwireMessageCompressionTest.cpp
  activemq/test/openwire/OpenWireMessageListenerRedeliveryTest.cpp
  activemq/test/openwire/OpenwireMessagePriorityTest.cpp
  activemq/test/openwire/OpenwireNonBlockingRedeliveryTest.cpp
  activemq/test/openwire/OpenwireOptimizedAckTest.cpp
  activemq/test/openwire/OpenwireQueueBrowserTest.cpp
  activemq/test/openwire/OpenWireRedeliveryPolicyTest.cpp
  activemq/test/openwire/OpenwireSimpleRollbackTest.cpp
  activemq/test/openwire/OpenwireSimpleTest.cpp
  activemq/test/openwire/OpenwireSlowListenerTest.cpp
  activemq/test/openwire/OpenwireTempDestinationTest.cpp
  activemq/test/openwire/OpenwireTransactionTest.cpp
  activemq/test/openwire/OpenwireVirtualTopicTest.cpp
  activemq/test/openwire/OpenwireXATransactionsTest.cpp
  # Additional OpenWire protocol specification tests
  activemq/test/openwire/OpenwireMessageSelectorTest.cpp

  # STOMP protocol tests
  activemq/test/stomp/StompAdvisoryTest.cpp
  activemq/test/stomp/StompAsyncSenderTest.cpp
  activemq/test/stomp/StompBulkMessageTest.cpp
  activemq/test/stomp/StompCmsConnectionStartStopTest.cpp
  activemq/test/stomp/StompCmsTemplateTest.cpp
  activemq/test/stomp/StompDurableTest.cpp
  activemq/test/stomp/StompExpirationTest.cpp
  activemq/test/stomp/StompJmsMessageGroupsTest.cpp
  activemq/test/stomp/StompSimpleRollbackTest.cpp
  activemq/test/stomp/StompSimpleTest.cpp
  activemq/test/stomp/StompSlowListenerTest.cpp
  activemq/test/stomp/StompTransactionTest.cpp
)

set(INTEGRATION_TEST_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Build a single integration test harness driven by the top-level main.cpp
list(REMOVE_ITEM INTEGRATION_TEST_SRCS ${INTEGRATION_TEST_MAIN})
add_executable(neoactivemq-integration-test ${INTEGRATION_TEST_MAIN} ${INTEGRATION_TEST_SRCS})
target_include_directories(neoactivemq-integration-test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../main
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  # Include test directory for shared test utilities (TestWatchdog.h)
  ${CMAKE_CURRENT_SOURCE_DIR}/../test
)

# Add /bigobj flag for MSVC to handle large object files with many sections
if(MSVC)
  target_compile_options(neoactivemq-integration-test PRIVATE /bigobj)
  # Allow duplicate symbols - templates instantiated in both DLL and test exe
  target_link_options(neoactivemq-integration-test PRIVATE /FORCE:MULTIPLE)
endif()

# Define DLL import macros when linking against shared library on Windows
# This is required to import static const members from the DLL
# On Linux/Unix, symbol visibility is handled differently (no __declspec needed)
if(AMQCPP_SHARED_LIB AND WIN32)
  target_compile_definitions(neoactivemq-integration-test PRIVATE
    DECAF_DLL
    CMS_DLL
    AMQCPP_DLL
  )
endif()

# Link to the consolidated library and Google Test
target_link_libraries(neoactivemq-integration-test PRIVATE neoactivemq-cpp GTest::gtest)

include(StaticTestDiscovery)

# Collect openwire + stomp test sources for static test discovery
# (these files contain the TEST_F macros after header merge)
set(INTEGRATION_TEST_DISCOVERY_SRCS
  activemq/test/openwire/OpenwireAdvisoryTest.cpp
  activemq/test/openwire/OpenwireAsyncSenderTest.cpp
  activemq/test/openwire/OpenwireClientAckTest.cpp
  activemq/test/openwire/OpenwireCmsConnectionStartStopTest.cpp
  activemq/test/openwire/OpenWireCmsSendWithAsyncCallbackTest.cpp
  activemq/test/openwire/OpenwireCmsTemplateTest.cpp
  activemq/test/openwire/OpenwireDurableTest.cpp
  activemq/test/openwire/OpenwireEnhancedConnectionTest.cpp
  activemq/test/openwire/OpenwireExpirationTest.cpp
  activemq/test/openwire/OpenwireIndividualAckTest.cpp
  activemq/test/openwire/OpenwireJmsMessageGroupsTest.cpp
  activemq/test/openwire/OpenwireJmsRecoverTest.cpp
  activemq/test/openwire/OpenwireMapMessageTest.cpp
  activemq/test/openwire/OpenwireMessageCompressionTest.cpp
  activemq/test/openwire/OpenWireMessageListenerRedeliveryTest.cpp
  activemq/test/openwire/OpenwireMessagePriorityTest.cpp
  activemq/test/openwire/OpenwireNonBlockingRedeliveryTest.cpp
  activemq/test/openwire/OpenwireOptimizedAckTest.cpp
  activemq/test/openwire/OpenwireQueueBrowserTest.cpp
  activemq/test/openwire/OpenWireRedeliveryPolicyTest.cpp
  activemq/test/openwire/OpenwireSimpleRollbackTest.cpp
  activemq/test/openwire/OpenwireSimpleTest.cpp
  activemq/test/openwire/OpenwireSlowListenerTest.cpp
  activemq/test/openwire/OpenwireTempDestinationTest.cpp
  activemq/test/openwire/OpenwireTransactionTest.cpp
  activemq/test/openwire/OpenwireVirtualTopicTest.cpp
  activemq/test/openwire/OpenwireXATransactionsTest.cpp
  activemq/test/openwire/OpenwireMessageSelectorTest.cpp
  activemq/test/stomp/StompAdvisoryTest.cpp
  activemq/test/stomp/StompAsyncSenderTest.cpp
  activemq/test/stomp/StompBulkMessageTest.cpp
  activemq/test/stomp/StompCmsConnectionStartStopTest.cpp
  activemq/test/stomp/StompCmsTemplateTest.cpp
  activemq/test/stomp/StompDurableTest.cpp
  activemq/test/stomp/StompExpirationTest.cpp
  activemq/test/stomp/StompJmsMessageGroupsTest.cpp
  activemq/test/stomp/StompSimpleRollbackTest.cpp
  activemq/test/stomp/StompSimpleTest.cpp
  activemq/test/stomp/StompSlowListenerTest.cpp
  activemq/test/stomp/StompTransactionTest.cpp
)

static_discover_tests(neoactivemq-integration-test
  TEST_PREFIX "integration/"
  SOURCES ${INTEGRATION_TEST_DISCOVERY_SRCS}
  LABELS integration
  PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
)
