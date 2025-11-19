# activemq core library

file(GLOB_RECURSE ACTIVEMQ_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

list(FILTER ACTIVEMQ_SOURCES EXCLUDE REGEX ".*/test/.*|.*/tests?/.*")

# Include specific CMS exception implementations in activemq_core so the
# exception symbols are available without creating a circular link
# dependency between activemq_core and activemq_cms.
list(APPEND ACTIVEMQ_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main/cms/CMSSecurityException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageEOFException.cpp
)

# Also include other CMS exception implementations that BrokerError and
# other activemq_core sources construct, so their symbols are available
# when linking activemq_core.
list(APPEND ACTIVEMQ_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main/cms/CMSException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageFormatException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageNotReadableException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageNotWriteableException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/InvalidClientIdException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/InvalidDestinationException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/InvalidSelectorException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/IllegalStateException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/ResourceAllocationException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/TransactionInProgressException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/TransactionRolledBackException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/UnsupportedOperationException.cpp
)

# Also compile in additional CMS implementations that activemq_core
# needs directly so those symbols are available at link-time.
list(APPEND ACTIVEMQ_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main/cms/XAException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageConsumer.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageProducer.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Destination.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/DestinationEvent.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageEnumeration.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/QueueBrowser.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/XAResource.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Xid.cpp
)

# Also compile additional CMS implementation files into activemq_core so
# activemq_core object files have the concrete implementations they rely on
# (destructors, vtables, typeinfo). This prevents unresolved-symbol and
# static-archive ordering problems when building/linking on Linux.
list(APPEND ACTIVEMQ_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main/cms/Connection.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/ConnectionFactory.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/ConnectionMetaData.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/EnhancedConnection.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Startable.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Stoppable.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Closeable.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Session.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/CMSProperties.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/BytesMessage.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MapMessage.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/ObjectMessage.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/StreamMessage.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Queue.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/TemporaryQueue.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/TemporaryTopic.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Topic.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/Message.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageTransformer.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/DestinationSource.cpp
)

if(AMQCPP_SHARED_LIB)
  add_library(activemq_core SHARED ${ACTIVEMQ_SOURCES})
else()
  add_library(activemq_core STATIC ${ACTIVEMQ_SOURCES})
endif()

target_include_directories(activemq_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/main
)

target_link_libraries(activemq_core PUBLIC decaf)

set_target_properties(activemq_core PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

# Windows-specific settings
if(WIN32)
  if(AMQCPP_SHARED_LIB)
    set_target_properties(activemq_core PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
  endif()
  target_link_libraries(activemq_core PRIVATE ws2_32)
  target_compile_definitions(activemq_core PRIVATE _WIN32_WINNT=0x0601)
endif()
