/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "StackTrace.h"

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

#ifdef __linux__
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cxxabi.h>
#include <dirent.h>
#include <cstring>
#include <sys/syscall.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#pragma comment(lib, "dbghelp.lib")
#endif

namespace test {
namespace util {

#ifdef __linux__
// Thread-local storage to identify threads
static thread_local int threadNumber = 0;
static std::atomic<int> nextThreadNumber{0};

// Print stack trace with demangled symbols
void printStackTrace(int threadId) {
    const int maxFrames = 2048;
    void* buffer[maxFrames];

    int numFrames = backtrace(buffer, maxFrames);
    char** symbols = backtrace_symbols(buffer, numFrames);

    if (threadId > 0) {
        std::cerr << "\n=== Stack Trace for Thread " << threadId << " ===" << std::endl;
    } else {
        std::cerr << "\n=== Stack Trace ===" << std::endl;
    }

    if (symbols == nullptr) {
        std::cerr << "Failed to get stack trace symbols" << std::endl;
        return;
    }

    for (int i = 0; i < numFrames; i++) {
        std::string frame(symbols[i]);
        std::cerr << "#" << i << " ";

        // Try to demangle C++ symbols
        size_t start = frame.find('(');
        size_t end = frame.find('+', start);

        if (start != std::string::npos && end != std::string::npos) {
            std::string mangled = frame.substr(start + 1, end - start - 1);
            int status;
            char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);

            if (status == 0 && demangled) {
                std::cerr << frame.substr(0, start + 1) << demangled
                         << frame.substr(end) << std::endl;
                free(demangled);
            } else {
                std::cerr << frame << std::endl;
            }
        } else {
            std::cerr << frame << std::endl;
        }
    }

    free(symbols);
    std::cerr << "===================" << std::endl;
}

// Signal handler for stack trace dumping
static void stackTraceSignalHandler(int sig) {
    if (sig == SIGUSR1) {
        printStackTrace(threadNumber);
    }
}

// Setup signal handler for stack trace
void initializeStackTrace() {
    struct sigaction sa;
    sa.sa_handler = stackTraceSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, nullptr);
}

// Enumerate all threads in the process
static std::vector<pid_t> getAllThreadIds() {
    std::vector<pid_t> threadIds;
    pid_t pid = getpid();

    char taskPath[256];
    snprintf(taskPath, sizeof(taskPath), "/proc/%d/task", pid);

    DIR* dir = opendir(taskPath);
    if (!dir) {
        std::cerr << "Failed to open /proc/" << pid << "/task" << std::endl;
        return threadIds;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            // Check if the name is a number (thread ID)
            char* endptr;
            long tid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0' && tid > 0) {
                threadIds.push_back(static_cast<pid_t>(tid));
            }
        }
    }

    closedir(dir);
    return threadIds;
}

// Dump stack traces of all threads
void dumpAllThreadStackTraces() {
    std::cerr << "\n========================================" << std::endl;
    std::cerr << "Dumping stack traces of all threads..." << std::endl;
    std::cerr << "========================================" << std::endl;

    std::vector<pid_t> threadIds = getAllThreadIds();

    if (threadIds.empty()) {
        std::cerr << "No threads found" << std::endl;
        return;
    }

    std::cerr << "Found " << threadIds.size() << " thread(s)" << std::endl;

    for (size_t i = 0; i < threadIds.size(); i++) {
        pid_t tid = threadIds[i];
        std::cerr << "\nThread " << (i + 1) << "/" << threadIds.size()
                  << " (TID: " << tid << ")" << std::endl;

        // Send signal to thread to dump its stack
        if (syscall(SYS_tgkill, getpid(), tid, SIGUSR1) == 0) {
            // Give thread time to print stack trace
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            std::cerr << "Failed to send signal to thread " << tid << std::endl;
        }
    }

    std::cerr << "\n========================================" << std::endl;
    std::cerr << "End of stack traces" << std::endl;
    std::cerr << "========================================" << std::endl;
}

// Helper for thread identification in tests
int getThreadNumber() {
    return threadNumber;
}

void assignThreadNumber() {
    threadNumber = ++nextThreadNumber;
}
#endif

#ifdef _WIN32
// Initialize symbol handler (call once at startup)
static std::once_flag symbolInitFlag;

static void initializeSymbolHandler() {
    std::call_once(symbolInitFlag, []() {
        HANDLE process = GetCurrentProcess();
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
        if (!SymInitialize(process, NULL, TRUE)) {
            std::cerr << "SymInitialize failed with error: " << GetLastError() << std::endl;
        }
    });
}

// Print stack trace on Windows using StackWalk64
static void printStackTraceForThread(DWORD threadId, HANDLE threadHandle, CONTEXT* context = nullptr) {
    initializeSymbolHandler();

    HANDLE process = GetCurrentProcess();
    HANDLE thread = threadHandle;

    if (threadId > 0) {
        std::cerr << "\n=== Stack Trace for Thread " << threadId << " ===" << std::endl;
    } else {
        std::cerr << "\n=== Stack Trace ===" << std::endl;
    }

    // If no thread handle provided, use current thread
    if (!thread) {
        thread = GetCurrentThread();
    }

    CONTEXT ctx;
    if (!context) {
        memset(&ctx, 0, sizeof(CONTEXT));
        ctx.ContextFlags = CONTEXT_FULL;

        if (thread == GetCurrentThread()) {
            RtlCaptureContext(&ctx);
        } else {
            if (!GetThreadContext(thread, &ctx)) {
                std::cerr << "Failed to get thread context: " << GetLastError() << std::endl;
                std::cerr << "===================" << std::endl;
                return;
            }
        }
        context = &ctx;
    }

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

#ifdef _M_X64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IX86
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context->Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
    std::cerr << "Unsupported architecture for stack walking" << std::endl;
    std::cerr << "===================" << std::endl;
    return;
#endif

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    if (!symbol) {
        std::cerr << "Failed to allocate memory for symbol info" << std::endl;
        std::cerr << "===================" << std::endl;
        return;
    }

    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    int frameNum = 0;
    const int maxFrames = 2048;

    while (frameNum < maxFrames) {
        if (!StackWalk64(
                machineType,
                process,
                thread,
                &stackFrame,
                context,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL)) {
            break;
        }

        if (stackFrame.AddrPC.Offset == 0) {
            break;
        }

        DWORD64 address = stackFrame.AddrPC.Offset;
        DWORD displacement = 0;

        std::cerr << "#" << frameNum << " 0x" << std::hex << address << std::dec << " ";

        if (SymFromAddr(process, address, 0, symbol)) {
            std::cerr << symbol->Name;

            // Try to get file and line number
            if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
                std::cerr << " at " << line.FileName << ":" << line.LineNumber;
            }
        } else {
            std::cerr << "???";
        }

        std::cerr << std::endl;
        frameNum++;
    }

    free(symbol);
    std::cerr << "===================" << std::endl;
}

// Convenience wrapper for current thread
void printStackTrace(int threadId) {
    printStackTraceForThread(static_cast<DWORD>(threadId), NULL, nullptr);
}

void initializeStackTrace() {
    initializeSymbolHandler();
}

// Thread info for stack trace enumeration
struct ThreadStackTraceInfo {
    DWORD threadId;
    int threadNumber;
};

// Enumerate all threads in the process
static std::vector<ThreadStackTraceInfo> getAllThreads() {
    std::vector<ThreadStackTraceInfo> threads;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create thread snapshot: " << GetLastError() << std::endl;
        return threads;
    }

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    DWORD currentPid = GetCurrentProcessId();
    int threadNum = 0;

    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == currentPid) {
                ThreadStackTraceInfo info;
                info.threadId = threadEntry.th32ThreadID;
                info.threadNumber = ++threadNum;
                threads.push_back(info);
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }

    CloseHandle(snapshot);
    return threads;
}

// Dump stack traces of all threads
void dumpAllThreadStackTraces() {
    std::cerr << "\n========================================" << std::endl;
    std::cerr << "Dumping stack traces of all threads..." << std::endl;
    std::cerr << "========================================" << std::endl;

    std::vector<ThreadStackTraceInfo> threads = getAllThreads();

    if (threads.empty()) {
        std::cerr << "No threads found" << std::endl;
        return;
    }

    std::cerr << "Found " << threads.size() << " thread(s)" << std::endl;

    for (size_t i = 0; i < threads.size(); i++) {
        std::cerr << "\nThread " << (i + 1) << "/" << threads.size()
                  << " (TID: " << threads[i].threadId << ")" << std::endl;

        // For the current thread, we can capture directly
        if (threads[i].threadId == GetCurrentThreadId()) {
            printStackTrace(threads[i].threadId);
        } else {
            // For other threads, we need to suspend them and get their context
            HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                                      FALSE, threads[i].threadId);
            if (thread) {
                if (SuspendThread(thread) != (DWORD)-1) {
                    CONTEXT context;
                    memset(&context, 0, sizeof(CONTEXT));
                    context.ContextFlags = CONTEXT_FULL;

                    if (GetThreadContext(thread, &context)) {
                        printStackTraceForThread(threads[i].threadId, thread, &context);
                    } else {
                        std::cerr << "Failed to get thread context: " << GetLastError() << std::endl;
                    }

                    ResumeThread(thread);
                } else {
                    std::cerr << "Failed to suspend thread: " << GetLastError() << std::endl;
                }
                CloseHandle(thread);
            } else {
                std::cerr << "Failed to open thread " << threads[i].threadId << ": " << GetLastError() << std::endl;
            }
        }
    }

    std::cerr << "\n========================================" << std::endl;
    std::cerr << "End of stack traces" << std::endl;
    std::cerr << "========================================" << std::endl;
}
#endif

#if !defined(__linux__) && !defined(_WIN32)
// Stub implementations for unsupported platforms
void printStackTrace(int threadId) {
    std::cerr << "\nStack trace not supported on this platform" << std::endl;
}

void initializeStackTrace() {
    // No-op on unsupported platforms
}

void dumpAllThreadStackTraces() {
    std::cerr << "\nStack trace dumping not supported on this platform" << std::endl;
}
#endif

}}
