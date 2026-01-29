# Build integration tests under src/test-integration
#
# IMPORTANT: Integration tests require a running ActiveMQ broker.
# Before running these tests, start the broker using Docker Compose:
#
#   docker-compose up -d
#
# Or manually with docker run (adjust version to match .env if needed):
#   docker run -d --rm --name activemq-test \
#     -p 61616:61616 -p 61613:61613 -p 8161:8161 \
#     -e ACTIVEMQ_MIN_MEMORY=512 -e ACTIVEMQ_MAX_MEMORY=2048 \
#     rmohr/activemq:5.15.9
#
# To stop the broker after testing:
#   docker-compose down
#   # OR: docker stop activemq-test

set(INTEGRATION_TEST_SRCS
  # Top level launcher / registry
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestRegistry.cpp

  # Utilities and support
  util/teamcity/TeamCityProgressListener.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../util/StackTrace.cpp
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

# Link to the consolidated library and CppUnit
target_link_libraries(neoactivemq-integration-test PRIVATE neoactivemq-cpp CppUnit)

# Note: Integration tests are NOT registered with CTest by default because they
# require a running ActiveMQ broker. To run integration tests manually:
#
# 1. Start ActiveMQ broker (see Docker commands above)
# 2. Run: ./activemq-integration-test
# 3. Stop the broker when done
#
# To register with CTest (advanced users only), uncomment the following:
# add_test(NAME activemq-integration-tests COMMAND activemq-integration-test)
# set_tests_properties(activemq-integration-tests PROPERTIES
#   TIMEOUT 600
#   ENVIRONMENT "ACTIVEMQ_BROKER_URI=tcp://localhost:61616"
# )
