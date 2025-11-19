# CMS layer library

file(GLOB_RECURSE CMS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

list(FILTER CMS_SOURCES EXCLUDE REGEX ".*/test/.*|.*/tests?/.*")

# These two exception implementations are needed by activemq_core at link
# time. Build them into activemq_core instead of activemq_cms to avoid a
# circular dependency when linking shared libraries. Remove them from the
# CMS sources list so they are not compiled into activemq_cms.
list(REMOVE_ITEM CMS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/main/cms/CMSSecurityException.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/MessageEOFException.cpp
)

# Also remove other CMS exception implementation files that are needed
# by activemq_core so their symbols are available where BrokerError is
# constructed.
list(REMOVE_ITEM CMS_SOURCES
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

# Also remove additional CMS implementations that are referenced by
# activemq_core (so activemq_core can link without depending on
# activemq_cms which would create a circular dependency when using
# shared libraries).
list(REMOVE_ITEM CMS_SOURCES
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

# Remove additional CMS implementation files that should be compiled
# into activemq_core so the core gets the concrete definitions (vtables,
# destructors, typeinfo) and avoids static archive ordering problems.
list(REMOVE_ITEM CMS_SOURCES
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
  ${CMAKE_SOURCE_DIR}/src/main/cms/ConnectionFactory.cpp
  ${CMAKE_SOURCE_DIR}/src/main/cms/DestinationSource.cpp
)

if(AMQCPP_SHARED_LIB)
  add_library(activemq_cms SHARED ${CMS_SOURCES})
else()
  add_library(activemq_cms STATIC ${CMS_SOURCES})
endif()

target_include_directories(activemq_cms
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/main
)

target_link_libraries(activemq_cms PUBLIC activemq_core decaf)

set_target_properties(activemq_cms PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)

# Windows-specific settings
if(WIN32)
  if(AMQCPP_SHARED_LIB)
    set_target_properties(activemq_cms PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
  endif()
  target_link_libraries(activemq_cms PRIVATE ws2_32)
  target_compile_definitions(activemq_cms PRIVATE _WIN32_WINNT=0x0601)
endif()
