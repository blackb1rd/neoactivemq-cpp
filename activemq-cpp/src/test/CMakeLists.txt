# Build unit tests under src/test -- split into module-based executables
#
# Each module is a separate executable containing tests from one logical area.
# The add_unit_test_module() helper (from cmake/AddUnitTestModule.cmake)
# handles shared infrastructure (main.cpp, TeamCity listener), include paths,
# compiler/linker flags, and CTest registration via static_discover_tests().

include(AddUnitTestModule)

# ─── Module 1: activemq-cmsutil (5 tests) ────────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-cmsutil
  SOURCES
    activemq/cmsutil/CmsAccessorTest.cpp
    activemq/cmsutil/CmsDestinationAccessorTest.cpp
    activemq/cmsutil/CmsTemplateTest.cpp
    activemq/cmsutil/DynamicDestinationResolverTest.cpp
    activemq/cmsutil/SessionPoolTest.cpp
)

# ─── Module 2: activemq-commands (13 tests) ──────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-commands
  SOURCES
    activemq/commands/ActiveMQBytesMessageTest.cpp
    activemq/commands/ActiveMQDestinationTest2.cpp
    activemq/commands/ActiveMQMapMessageTest.cpp
    activemq/commands/ActiveMQMessageTest.cpp
    activemq/commands/ActiveMQQueueTest.cpp
    activemq/commands/ActiveMQStreamMessageTest.cpp
    activemq/commands/ActiveMQTempQueueTest.cpp
    activemq/commands/ActiveMQTempTopicTest.cpp
    activemq/commands/ActiveMQTextMessageTest.cpp
    activemq/commands/ActiveMQTopicTest.cpp
    activemq/commands/BrokerIdTest.cpp
    activemq/commands/BrokerInfoTest.cpp
    activemq/commands/XATransactionIdTest.cpp
)

# ─── Module 3: activemq-core (9 tests, includes exceptions) ──────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-core
  SOURCES
    activemq/core/ActiveMQConnectionFactoryTest.cpp
    activemq/core/ActiveMQConnectionTest.cpp
    activemq/core/ActiveMQMessageAuditTest.cpp
    activemq/core/ActiveMQSessionTest.cpp
    activemq/core/ConnectionAuditTest.cpp
    activemq/core/FifoMessageDispatchChannelTest.cpp
    activemq/core/LazyPropertyUnmarshalTest.cpp
    activemq/core/SimplePriorityMessageDispatchChannelTest.cpp
    activemq/exceptions/ActiveMQExceptionTest.cpp
)

# ─── Module 4: activemq-state (6 tests) ──────────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-state
  SOURCES
    activemq/state/ConnectionStateTest.cpp
    activemq/state/ConnectionStateTrackerTest.cpp
    activemq/state/ConsumerStateTest.cpp
    activemq/state/ProducerStateTest.cpp
    activemq/state/SessionStateTest.cpp
    activemq/state/TransactionStateTest.cpp
)

# ─── Module 5: activemq-threads (3 tests) ────────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-threads
  SOURCES
    activemq/threads/CompositeTaskRunnerTest.cpp
    activemq/threads/DedicatedTaskRunnerTest.cpp
    activemq/threads/SchedulerTest.cpp
)

# ─── Module 6: activemq-transport (14 tests) ─────────────────────────────────
# MockBrokerService.cpp is needed by FailoverTransportTest and
# SingleBrokerReconnectTest but contains no TEST_F macros.
add_unit_test_module(
  NAME neoactivemq-unit-activemq-transport
  SOURCES
    activemq/transport/CorruptedMessageTest.cpp
    activemq/transport/IOTransportTest.cpp
    activemq/transport/TransportRegistryTest.cpp
    activemq/transport/correlator/ResponseCorrelatorTest.cpp
    activemq/transport/discovery/AbstractDiscoveryAgentFactoryTest.cpp
    activemq/transport/discovery/AbstractDiscoveryAgentTest.cpp
    activemq/transport/discovery/DiscoveryAgentRegistryTest.cpp
    activemq/transport/discovery/DiscoveryTransportFactoryTest.cpp
    activemq/transport/failover/FailoverTransportTest.cpp
    activemq/transport/inactivity/InactivityMonitorTest.cpp
    activemq/transport/mock/MockTransportFactoryTest.cpp
    activemq/transport/tcp/TcpTransportTest.cpp
    activemq/transport/tcp/SingleBrokerReconnectTest.cpp
    activemq/transport/tcp/SslTransportTest.cpp
  EXTRA_SOURCES
    activemq/mock/MockBrokerService.cpp
)

# ─── Module 7: activemq-util (12 tests) ──────────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-util
  SOURCES
    activemq/util/ActiveMQMessageTransformationTest.cpp
    activemq/util/AdvisorySupportTest.cpp
    activemq/util/AMQLogTest.cpp
    activemq/util/IdGeneratorTest.cpp
    activemq/util/LongSequenceGeneratorTest.cpp
    activemq/util/MarshallingSupportTest.cpp
    activemq/util/MemoryUsageTest.cpp
    activemq/util/PrimitiveListTest.cpp
    activemq/util/PrimitiveMapTest.cpp
    activemq/util/PrimitiveValueConverterTest.cpp
    activemq/util/PrimitiveValueNodeTest.cpp
    activemq/util/URISupportTest.cpp
)

# ─── Module 8: activemq-wireformat (66 tests) ────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-activemq-wireformat
  SOURCES
    activemq/wireformat/WireFormatRegistryTest.cpp
    activemq/wireformat/openwire/OpenWireFormatTest.cpp
    activemq/wireformat/openwire/marshal/BaseDataStreamMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/PrimitiveTypesMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQBlobMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQBytesMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQMapMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQObjectMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQQueueMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQStreamMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQTempQueueMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQTempTopicMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQTextMessageMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ActiveMQTopicMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/BrokerIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/BrokerInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConnectionControlMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConnectionErrorMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConnectionIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConnectionInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConsumerControlMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConsumerIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ConsumerInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ControlCommandMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/DataArrayResponseMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/DataResponseMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/DestinationInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/DiscoveryEventMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ExceptionResponseMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/FlushCommandMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/IntegerResponseMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/JournalQueueAckMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/JournalTopicAckMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/JournalTraceMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/JournalTransactionMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/KeepAliveInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/LastPartialCommandMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/LocalTransactionIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/MessageAckMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/MessageDispatchMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/MessageDispatchNotificationMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/MessageIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/MessagePullMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/NetworkBridgeFilterMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/PartialCommandMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ProducerAckMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ProducerIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ProducerInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/RemoveInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/RemoveSubscriptionInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ReplayCommandMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ResponseMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/SessionIdMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/SessionInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/ShutdownInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/SubscriptionInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/TransactionInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/WireFormatInfoMarshallerTest.cpp
    activemq/wireformat/openwire/marshal/generated/XATransactionIdMarshallerTest.cpp
    activemq/wireformat/openwire/utils/BooleanStreamTest.cpp
    activemq/wireformat/openwire/utils/HexTableTest.cpp
    activemq/wireformat/openwire/utils/MessagePropertyInterceptorTest.cpp
    activemq/wireformat/stomp/StompHelperTest.cpp
    activemq/wireformat/stomp/StompWireFormatFactoryTest.cpp
    activemq/wireformat/stomp/StompWireFormatTest.cpp
)

# ─── Module 9: decaf-internal (16 tests) ─────────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-decaf-internal
  SOURCES
    decaf/internal/net/URIEncoderDecoderTest.cpp
    decaf/internal/net/URIHelperTest.cpp
    decaf/internal/net/ssl/DefaultSSLSocketFactoryTest.cpp
    decaf/internal/net/ssl/openssl/OpenSSLSocketTest.cpp
    decaf/internal/nio/BufferFactoryTest.cpp
    decaf/internal/nio/ByteArrayBufferTest.cpp
    decaf/internal/nio/CharArrayBufferTest.cpp
    decaf/internal/nio/DoubleArrayBufferTest.cpp
    decaf/internal/nio/FloatArrayBufferTest.cpp
    decaf/internal/nio/IntArrayBufferTest.cpp
    decaf/internal/nio/LongArrayBufferTest.cpp
    decaf/internal/nio/ShortArrayBufferTest.cpp
    decaf/internal/util/ByteArrayAdapterTest.cpp
    decaf/internal/util/TimerTaskHeapTest.cpp
    decaf/internal/util/concurrent/TransferQueueTest.cpp
    decaf/internal/util/concurrent/TransferStackTest.cpp
)

# ─── Module 10: decaf-io (15 tests) ──────────────────────────────────────────
# Endian.cpp is a test utility needed by DataInput/OutputStreamTest.
add_unit_test_module(
  NAME neoactivemq-unit-decaf-io
  SOURCES
    decaf/io/BufferedInputStreamTest.cpp
    decaf/io/BufferedOutputStreamTest.cpp
    decaf/io/ByteArrayInputStreamTest.cpp
    decaf/io/ByteArrayOutputStreamTest.cpp
    decaf/io/DataInputStreamTest.cpp
    decaf/io/DataOutputStreamTest.cpp
    decaf/io/FilterInputStreamTest.cpp
    decaf/io/FilterOutputStreamTest.cpp
    decaf/io/InputStreamReaderTest.cpp
    decaf/io/InputStreamTest.cpp
    decaf/io/OutputStreamTest.cpp
    decaf/io/OutputStreamWriterTest.cpp
    decaf/io/PushbackInputStreamTest.cpp
    decaf/io/ReaderTest.cpp
    decaf/io/WriterTest.cpp
  EXTRA_SOURCES
    decaf/util/Endian.cpp
)

# ─── Module 11: decaf-lang (18 tests) ────────────────────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-decaf-lang
  SOURCES
    decaf/lang/ArrayPointerTest.cpp
    decaf/lang/BooleanTest.cpp
    decaf/lang/ByteTest.cpp
    decaf/lang/CharacterTest.cpp
    decaf/lang/DoubleTest.cpp
    decaf/lang/ExceptionTest.cpp
    decaf/lang/FloatTest.cpp
    decaf/lang/IntegerTest.cpp
    decaf/lang/LongTest.cpp
    decaf/lang/MathTest.cpp
    decaf/lang/PointerTest.cpp
    decaf/lang/ShortTest.cpp
    decaf/lang/StringBufferTest.cpp
    decaf/lang/StringBuilderTest.cpp
    decaf/lang/StringTest.cpp
    decaf/lang/SystemTest.cpp
    decaf/lang/ThreadLocalTest.cpp
    decaf/lang/ThreadTest.cpp
)

# ─── Module 12: decaf-net (12 tests, includes ssl) ───────────────────────────
add_unit_test_module(
  NAME neoactivemq-unit-decaf-net
  SOURCES
    decaf/net/Inet4AddressTest.cpp
    decaf/net/Inet6AddressTest.cpp
    decaf/net/InetAddressTest.cpp
    decaf/net/ServerSocketTest.cpp
    decaf/net/SocketFactoryTest.cpp
    decaf/net/SocketTest.cpp
    decaf/net/URISyntaxExceptionTest.cpp
    decaf/net/URITest.cpp
    decaf/net/URLDecoderTest.cpp
    decaf/net/URLEncoderTest.cpp
    decaf/net/URLTest.cpp
    decaf/net/ssl/SSLSocketFactoryTest.cpp
)

# ─── Module 13: decaf-util (36 tests, includes zip/nio/security) ─────────────
add_unit_test_module(
  NAME neoactivemq-unit-decaf-util
  SOURCES
    decaf/util/AbstractCollectionTest.cpp
    decaf/util/AbstractListTest.cpp
    decaf/util/AbstractSequentialListTest.cpp
    decaf/util/ArrayListTest.cpp
    decaf/util/ArraysTest.cpp
    decaf/util/BitSetTest.cpp
    decaf/util/CollectionsTest.cpp
    decaf/util/DateTest.cpp
    decaf/util/HashCodeTest.cpp
    decaf/util/HashMapTest.cpp
    decaf/util/HashSetTest.cpp
    decaf/util/LRUCacheTest.cpp
    decaf/util/LinkedHashMapTest.cpp
    decaf/util/LinkedHashSetTest.cpp
    decaf/util/LinkedListTest.cpp
    decaf/util/ListTest.cpp
    decaf/util/PriorityQueueTest.cpp
    decaf/util/PropertiesTest.cpp
    decaf/util/QueueTest.cpp
    decaf/util/RandomTest.cpp
    decaf/util/SetTest.cpp
    decaf/util/StlMapTest.cpp
    decaf/util/StringTokenizerTest.cpp
    decaf/util/TimerTest.cpp
    decaf/util/UUIDTest.cpp
    decaf/util/zip/Adler32Test.cpp
    decaf/util/zip/CheckedInputStreamTest.cpp
    decaf/util/zip/CheckedOutputStreamTest.cpp
    decaf/util/zip/CRC32Test.cpp
    decaf/util/zip/DeflaterOutputStreamTest.cpp
    decaf/util/zip/DeflaterTest.cpp
    decaf/util/zip/InflaterInputStreamTest.cpp
    decaf/util/zip/InflaterTest.cpp
    decaf/nio/BufferTest.cpp
    decaf/security/MessageDigestTest.cpp
    decaf/security/SecureRandomTest.cpp
  EXTRA_SOURCES
    decaf/util/Endian.cpp
)

# ─── Module 14: decaf-concurrent (21 tests) ──────────────────────────────────
# ExecutorsTestSupport.cpp is a shared test fixture base class.
add_unit_test_module(
  NAME neoactivemq-unit-decaf-concurrent
  SOURCES
    decaf/util/concurrent/ExecutorsTest.cpp
    decaf/util/concurrent/CountDownLatchTest.cpp
    decaf/util/concurrent/CopyOnWriteArrayListTest.cpp
    decaf/util/concurrent/CopyOnWriteArraySetTest.cpp
    decaf/util/concurrent/ConcurrentStlMapTest.cpp
    decaf/util/concurrent/ConcurrentHashMapTest.cpp
    decaf/util/concurrent/LinkedBlockingQueueTest.cpp
    decaf/util/concurrent/FutureTaskTest.cpp
    decaf/util/concurrent/ThreadPoolExecutorTest.cpp
    decaf/util/concurrent/AbstractExecutorServiceTest.cpp
    decaf/util/concurrent/atomic/AtomicIntegerTest.cpp
    decaf/util/concurrent/atomic/AtomicReferenceTest.cpp
    decaf/util/concurrent/atomic/AtomicBooleanTest.cpp
    decaf/util/concurrent/MutexTest.cpp
    decaf/util/concurrent/SemaphoreTest.cpp
    decaf/util/concurrent/SynchronousQueueTest.cpp
    decaf/util/concurrent/TimeUnitTest.cpp
    decaf/util/concurrent/locks/ReentrantReadWriteLockTest.cpp
    decaf/util/concurrent/locks/ReentrantLockTest.cpp
    decaf/util/concurrent/locks/LockSupportTest.cpp
    decaf/util/concurrent/locks/AbstractQueuedSynchronizerTest.cpp
  EXTRA_SOURCES
    decaf/util/concurrent/ExecutorsTestSupport.cpp
)
