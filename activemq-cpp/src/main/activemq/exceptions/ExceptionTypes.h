#ifndef _ACTIVEMQ_EXCEPTIONS_EXCEPTIONTYPES_H_
#define _ACTIVEMQ_EXCEPTIONS_EXCEPTIONTYPES_H_

#include <activemq/util/Config.h>

#include <new>
#include <stdexcept>
#include <string>

namespace activemq
{
namespace exceptions
{

    inline std::string buildSourceMessage(const char* file,
                                          int         lineNumber,
                                          const char* message)
    {
        return std::string(file) + ":" + std::to_string(lineNumber) + ": " +
               std::string(message);
    }

    inline std::string buildSourceMessage(const char*        file,
                                          int                lineNumber,
                                          const std::string& message)
    {
        return std::string(file) + ":" + std::to_string(lineNumber) + ": " +
               message;
    }

    class AMQCPP_API InterruptedException : public std::runtime_error
    {
    public:
        explicit InterruptedException(const std::string& message)
            : std::runtime_error(message)
        {
        }

        InterruptedException(const char* file,
                             int         lineNumber,
                             const char* message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        InterruptedException(const char*        file,
                             int                lineNumber,
                             const std::string& message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * General unchecked failure (Java RuntimeException analogue).
     */
    class AMQCPP_API RuntimeException : public std::runtime_error
    {
    public:
        RuntimeException()
            : std::runtime_error("RuntimeException")
        {
        }

        explicit RuntimeException(const std::string& message)
            : std::runtime_error(message)
        {
        }

        RuntimeException(const char* file, int lineNumber, const char* message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        RuntimeException(const char*        file,
                         int                lineNumber,
                         const std::string& message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    class AMQCPP_API IllegalStateException : public std::logic_error
    {
    public:
        explicit IllegalStateException(const std::string& message)
            : std::logic_error(message)
        {
        }

        IllegalStateException(const char* file,
                              int         lineNumber,
                              const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        IllegalStateException(const char*        file,
                              int                lineNumber,
                              const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    class AMQCPP_API InvalidStateException : public std::logic_error
    {
    public:
        explicit InvalidStateException(const std::string& message)
            : std::logic_error(message)
        {
        }

        InvalidStateException(const char* file,
                              int         lineNumber,
                              const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        InvalidStateException(const char*        file,
                              int                lineNumber,
                              const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Object does not support cloning (Java CloneNotSupportedException
     * analogue).
     */
    class AMQCPP_API CloneNotSupportedException : public std::logic_error
    {
    public:
        explicit CloneNotSupportedException(const std::string& message)
            : std::logic_error(message)
        {
        }

        CloneNotSupportedException(const char* file,
                                   int         lineNumber,
                                   const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        CloneNotSupportedException(const char*        file,
                                   int                lineNumber,
                                   const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    class AMQCPP_API NullPointerException : public std::logic_error
    {
    public:
        explicit NullPointerException(const std::string& message)
            : std::logic_error(message)
        {
        }

        NullPointerException(const char* file,
                             int         lineNumber,
                             const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        NullPointerException(const char*        file,
                             int                lineNumber,
                             const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    class AMQCPP_API UnsupportedOperationException : public std::logic_error
    {
    public:
        UnsupportedOperationException()
            : std::logic_error("UnsupportedOperationException")
        {
        }

        explicit UnsupportedOperationException(const std::string& message)
            : std::logic_error(message)
        {
        }

        UnsupportedOperationException(const char* file,
                                      int         lineNumber,
                                      const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        UnsupportedOperationException(const char*        file,
                                      int                lineNumber,
                                      const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    class AMQCPP_API IllegalMonitorStateException : public std::logic_error
    {
    public:
        IllegalMonitorStateException()
            : std::logic_error("IllegalMonitorStateException")
        {
        }

        explicit IllegalMonitorStateException(const std::string& message)
            : std::logic_error(message)
        {
        }

        IllegalMonitorStateException(const char* file,
                                     int         lineNumber,
                                     const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        IllegalMonitorStateException(const char*        file,
                                     int                lineNumber,
                                     const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Thrown when a value does not match an expected static type (for example
     * OpenWire copyDataStructure / dynamic_cast failures).
     */
    class AMQCPP_API TypeMismatchException : public std::logic_error
    {
    public:
        explicit TypeMismatchException(const std::string& message)
            : std::logic_error(message)
        {
        }

        TypeMismatchException(const char* file,
                              int         lineNumber,
                              const char* message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        TypeMismatchException(const char*        file,
                              int                lineNumber,
                              const std::string& message)
            : std::logic_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Index or range violation; derives from std::out_of_range so existing
     * handlers can remain unchanged.
     */
    class AMQCPP_API OutOfRangeException : public std::out_of_range
    {
    public:
        explicit OutOfRangeException(const std::string& message)
            : std::out_of_range(message)
        {
        }

        OutOfRangeException(const char* file,
                            int         lineNumber,
                            const char* message)
            : std::out_of_range(buildSourceMessage(file, lineNumber, message))
        {
        }

        OutOfRangeException(const char*        file,
                            int                lineNumber,
                            const std::string& message)
            : std::out_of_range(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Structural modification during iteration or similar detected failures.
     */
    class AMQCPP_API ConcurrentModificationException : public std::runtime_error
    {
    public:
        explicit ConcurrentModificationException(const std::string& message)
            : std::runtime_error(message)
        {
        }

        ConcurrentModificationException(const char* file,
                                        int         lineNumber,
                                        const char* message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        ConcurrentModificationException(const char*        file,
                                        int                lineNumber,
                                        const std::string& message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Iterator-style exhaustion (for example StringTokenizer::nextToken).
     */
    class AMQCPP_API NoSuchElementException : public std::runtime_error
    {
    public:
        NoSuchElementException()
            : std::runtime_error("NoSuchElementException")
        {
        }

        explicit NoSuchElementException(const std::string& message)
            : std::runtime_error(message)
        {
        }

        NoSuchElementException(const char* file,
                               int         lineNumber,
                               const char* message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }

        NoSuchElementException(const char*        file,
                               int                lineNumber,
                               const std::string& message)
            : std::runtime_error(buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Allocation failure; derives from std::bad_alloc so existing handlers and
     * tests catching std::bad_alloc remain valid. Keeps static footprint
     * minimal (no extra allocations for message storage).
     */
    class AMQCPP_API OutOfMemoryError : public std::bad_alloc
    {
    public:
        OutOfMemoryError() noexcept = default;
    };

    /**
     * Invalid parameter or conversion input; derives from
     * std::invalid_argument.
     */
    class AMQCPP_API InvalidArgumentException : public std::invalid_argument
    {
    public:
        explicit InvalidArgumentException(const std::string& message)
            : std::invalid_argument(message)
        {
        }

        InvalidArgumentException(const char* file,
                                 int         lineNumber,
                                 const char* message)
            : std::invalid_argument(
                  buildSourceMessage(file, lineNumber, message))
        {
        }

        InvalidArgumentException(const char*        file,
                                 int                lineNumber,
                                 const std::string& message)
            : std::invalid_argument(
                  buildSourceMessage(file, lineNumber, message))
        {
        }
    };

    /**
     * Legacy Decaf / Java-style names for index and argument failures.
     * These are type aliases to the thin STL-backed exceptions above so
     * throw/catch types can match Apache CMS Decaf semantics without
     * duplicating implementations.
     */
    using IndexOutOfBoundsException       = OutOfRangeException;
    using StringIndexOutOfBoundsException = OutOfRangeException;
    using IllegalArgumentException        = InvalidArgumentException;
    using NumberFormatException           = InvalidArgumentException;

}  // namespace exceptions

}  // namespace activemq

#endif /* _ACTIVEMQ_EXCEPTIONS_EXCEPTIONTYPES_H_ */
