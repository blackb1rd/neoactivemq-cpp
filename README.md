# NeoActiveMQ CPP Library

NeoActiveMQ CPP is a modernized C++17 messaging library that can use multiple protocols to talk to a MOM (e.g. ActiveMQ). This is a fork of Apache ActiveMQ CPP with updated build system and modern C++ standards.

## 1. Prerequisites

### 1.1 Required Tools

| Tool | Recommended Version |
|------|-------------------|
| CMake | >= 3.27 (>= 3.31 on Windows) |
| vcpkg | latest |
| C++ Compiler | C++17 support required |
| MSVC | >= 2019 (Windows) |
| Clang-CL | >= 14 (Windows, for test builds) |
| GCC | >= 7.0 (Linux, non-test builds) |
| Clang | >= 14 (Linux/macOS) |

### 1.2 Automatic Dependency Management

This project uses vcpkg for dependency management. All dependencies are automatically downloaded and built during the CMake configuration phase. No manual installation is required.

Dependencies managed by vcpkg:
- **OpenSSL** - cryptography
- **asio** - async I/O
- **Google Test** - testing
- **zlib** - compression

## 2. Building with CMake Presets

This project uses CMake presets for simplified configuration and building. The build system automatically manages all dependencies through vcpkg.

### 2.1 Available Presets

To see all available presets:

```bash
cmake --list-presets
```

Presets follow the naming pattern `<arch>-<os>-<compiler>-<build_type>[-<variant>]`:

**Windows (MSVC cl.exe — standard builds):**

| Preset | Description |
|--------|-------------|
| `x86-windows-cl-debug` | Windows 32-bit Debug |
| `x86-windows-cl-release` | Windows 32-bit Release |
| `x64-windows-cl-debug` | Windows 64-bit Debug |
| `x64-windows-cl-release` | Windows 64-bit Release |
| `x64-windows-cl-debug-static` | Windows 64-bit Debug, static runtime |
| `x64-windows-cl-release-static` | Windows 64-bit Release, static runtime |
| `x64-windows-cl-debug-no-ssl` | Windows 64-bit Debug, no SSL |
| `x64-windows-cl-release-no-ssl` | Windows 64-bit Release, no SSL |

**Windows (MSVC cl.exe — test builds):**

| Preset | Description |
|--------|-------------|
| `x86-windows-cl-debug-test` | Windows 32-bit Debug with tests |
| `x86-windows-cl-release-test` | Windows 32-bit Release with tests |
| `x64-windows-cl-debug-test` | Windows 64-bit Debug with tests |
| `x64-windows-cl-release-test` | Windows 64-bit Release with tests |
| `x64-windows-cl-debug-no-ssl-test` | Windows 64-bit Debug with tests, no SSL |
| `x64-windows-cl-release-no-ssl-test` | Windows 64-bit Release with tests, no SSL |

**Windows (clang-cl — test builds, used in CI):**

| Preset | Description |
|--------|-------------|
| `x64-windows-clang-debug-test` | Windows 64-bit Debug with tests |
| `x64-windows-clang-release-test` | Windows 64-bit Release with tests |
| `x64-windows-clang-debug-no-ssl-test` | Windows 64-bit Debug with tests, no SSL |
| `x64-windows-clang-release-no-ssl-test` | Windows 64-bit Release with tests, no SSL |

**Linux (GCC — standard builds):**

| Preset | Description |
|--------|-------------|
| `x64-linux-gcc-debug` | Linux 64-bit Debug |
| `x64-linux-gcc-release` | Linux 64-bit Release |
| `x64-linux-gcc-debug-static` | Linux 64-bit Debug, static |
| `x64-linux-gcc-release-static` | Linux 64-bit Release, static |
| `x64-linux-gcc-debug-no-ssl` | Linux 64-bit Debug, no SSL |
| `x64-linux-gcc-release-no-ssl` | Linux 64-bit Release, no SSL |

**Linux (GCC — test builds):**

| Preset | Description |
|--------|-------------|
| `x64-linux-gcc-debug-test` | Linux 64-bit Debug with tests |
| `x64-linux-gcc-release-test` | Linux 64-bit Release with tests |
| `x64-linux-gcc-debug-no-ssl-test` | Linux 64-bit Debug with tests, no SSL |
| `x64-linux-gcc-release-no-ssl-test` | Linux 64-bit Release with tests, no SSL |

**Linux (Clang — test builds, used in CI):**

| Preset | Description |
|--------|-------------|
| `x64-linux-clang-debug-test` | Linux 64-bit Debug with tests |
| `x64-linux-clang-release-test` | Linux 64-bit Release with tests |
| `x64-linux-clang-debug-no-ssl-test` | Linux 64-bit Debug with tests, no SSL |
| `x64-linux-clang-release-no-ssl-test` | Linux 64-bit Release with tests, no SSL |

**macOS (Clang):**

| Preset | Description |
|--------|-------------|
| `x64-osx-debug` | macOS Intel Debug |
| `x64-osx-release` | macOS Intel Release |
| `x64-osx-debug-test` | macOS Intel Debug with tests |
| `x64-osx-release-test` | macOS Intel Release with tests |
| `arm64-osx-debug` | macOS Apple Silicon Debug |
| `arm64-osx-release` | macOS Apple Silicon Release |
| `arm64-osx-debug-test` | macOS Apple Silicon Debug with tests |
| `arm64-osx-release-test` | macOS Apple Silicon Release with tests |

### 2.2 Quick Start - Windows

1. Configure the project:

   ```bash
   cmake --preset x64-windows-cl-debug-test
   ```

   This will:
   - Download and build all dependencies via vcpkg
   - Configure the build with tests enabled
   - Generate build files in `output/build/x64-windows-cl-debug-test/`

2. Build the project:

   ```bash
   cmake --build --preset x64-windows-cl-debug-test
   ```

   The build output will be in:
   - Libraries: `output/build/<preset-name>/lib/`
   - Executables: `output/build/<preset-name>/bin/`

### 2.3 Quick Start - Linux

1. Configure the project:

   ```bash
   cmake --preset x64-linux-gcc-debug-test
   ```

2. Build the project:

   ```bash
   cmake --build --preset x64-linux-gcc-debug-test
   ```

### 2.4 Quick Start - macOS

1. Configure the project (Apple Silicon):

   ```bash
   cmake --preset arm64-osx-debug-test
   ```

   Or for Intel Macs:

   ```bash
   cmake --preset x64-osx-debug-test
   ```

2. Build the project:

   ```bash
   cmake --build --preset arm64-osx-debug-test
   ```

### 2.5 Installation

To install the library:

```bash
cmake --install output/build/<preset-name> --prefix /usr/local
```

Or on Windows:

```bash
cmake --install output/build/<preset-name> --prefix "C:/Program Files/neoactivemq-cpp"
```

This installs:
- Headers to `<prefix>/include/`
- Libraries to `<prefix>/lib/`
- CMake config files to `<prefix>/lib/cmake/neoactivemq-cpp/`

## 3. Running Tests

### 3.1 Unit Tests

Test executables are built automatically when using a preset with `-test` in the name. Test builds use Clang on Linux/macOS and clang-cl on Windows for consistent diagnostics across platforms.

To run unit tests via CTest:

```bash
# Linux (GCC)
ctest --preset x64-linux-gcc-debug-test -L unit

# Windows (MSVC)
ctest --preset x64-windows-cl-debug-test -L unit
```

### 3.2 Integration Tests

Integration tests run against a real ActiveMQ broker. Running without a broker will result in failed tests.

The tests connect to:
- **Stomp:** `tcp://localhost:61613`
- **OpenWire:** `tcp://localhost:61616`

To run integration tests:

```bash
# Start an ActiveMQ broker
docker compose up -d

# Run all integration tests (OpenWire + STOMP)
ctest --preset <preset> -L integration --output-on-failure

# Or run specific protocols:
# Run only OpenWire tests (non-SSL)
ctest --preset <preset> -L integration-openwire --output-on-failure

# Run only STOMP tests
ctest --preset <preset> -L integration-stomp --output-on-failure

# Stop the broker
docker compose down
```

Or use the CMake helper target:

```bash
cmake --build --preset <preset> --target integration-full
```

### 3.3 SSL Integration Tests (Linux only)

SSL integration tests provide **comprehensive validation** of the SSL/TLS transport layer against an SSL-enabled ActiveMQ broker. The SSL test suite mirrors the complete OpenWire test suite (29 tests) to ensure SSL transport works for all features.

```bash
# Start the SSL-enabled broker (certificates are generated automatically)
docker compose --profile ssl up -d

# Run all OpenWire SSL integration tests (29 comprehensive tests)
SSL_CERT_FILE=docker/ssl/certs/ca.pem ctest --preset <preset> -L integration-openwire-ssl --output-on-failure

# Stop the broker
docker compose --profile ssl down
```

**SSL Test Coverage:**
- All acknowledgment modes (client, individual, optimized)
- Advisory messages
- Async sending and callbacks
- Message selectors and groups
- Durable subscriptions
- Transactions (local and XA)
- Redelivery policies and session recovery
- Temporary destinations
- Message compression and priority
- Queue browsing and virtual topics
- Slow consumers and expiration
- Enhanced connection features

### 3.4 Integration Benchmark Tests

Integration benchmark tests exercise failover, multi-connection, and high-volume scenarios. They require multiple broker instances.

```bash
# Start all brokers (failover profile)
docker compose --profile failover up -d

# Run integration benchmark tests
ctest --preset <preset> -L integration-benchmark --output-on-failure --timeout 1800

# Stop all brokers
docker compose --profile failover down
```

Or use the CMake helper target:

```bash
cmake --build --preset <preset> --target integration-benchmark-full
```

## 4. Examples

Example applications are located in `activemq-cpp/src/examples/` and are built automatically.

After building, examples are located in:

```
output/build/<preset-name>/bin/examples/
```

Example executables follow the naming pattern `example_<directory>_<MainFile>`:

- `example_advisories_AdvisoryConsumerMain`
- `example_cmstemplate_CMSTemplateReceiverMain`
- `example_producers_SimpleProducerMain`

## 5. Using in Your Project

### 5.1 With CMake

After installation, use `find_package` in your `CMakeLists.txt`:

```cmake
find_package(neoactivemq-cpp REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE neoactivemq-cpp::activemq-cpp)
```

### 5.2 Build Options

The following CMake options can be configured:

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON for `-test` presets | Build test executables |
| `BUILD_EXAMPLES` | ON | Build example applications |
| `AMQCPP_SHARED_LIB` | OFF | Build as shared library (default: static) |

To customize a preset, pass options during configuration:

```bash
cmake --preset x64-windows-cl-debug-test -DBUILD_EXAMPLES=OFF
```

## 6. Project Structure

```
activemq-cpp/src/main/                  Main library source code
  activemq/                             ActiveMQ protocol implementation
  cms/                                  CMS (Common Messaging System) API
  decaf/                                Foundation library (I/O, threading, etc.)
activemq-cpp/src/test/                  Unit tests
activemq-cpp/src/test-integration/      Integration tests
  activemq/test/openwire/               OpenWire protocol tests
  activemq/test/openwire_ssl/           OpenWire SSL protocol tests
  activemq/test/stomp/                  STOMP protocol tests
activemq-cpp/src/test-integration-benchmarks/  Failover & high-volume benchmark tests
activemq-cpp/src/examples/             Example applications
cmake/                                 CMake configuration files
  presets/                             Platform-specific preset files
docker/ssl/                            SSL certificate generation and broker config
output/build/                          Build output directory (created by CMake)
```

## 7. Compiler Strategy

This project supports multiple compilers. CI uses Clang/clang-cl for test builds (similar to the Chromium project), but all compilers can be used locally:

| Platform | Standard Builds | Test Builds (local) | Test Builds (CI) |
|----------|----------------|---------------------|------------------|
| Windows | MSVC (`cl.exe`) | MSVC (`cl.exe`) | clang-cl |
| Linux | GCC | GCC | Clang |
| macOS | Clang | Clang | Clang |

CI uses Clang/clang-cl to benefit from better diagnostics, improved sanitizer support, and consistent cross-platform behavior.

## 8. Notes for Windows Users

- Visual Studio 2019 or later is required for C++17 support
- No need to manually install dependencies — vcpkg handles everything
- The Platform SDK is included with Visual Studio 2019+
- clang-cl is included with Visual Studio 2019+ (LLVM toolset component)
- When linking applications:
  - **Static library** (default): No special considerations
  - **Shared library**: Ensure runtime library matches (MD vs MT flags)

## 9. Notes for Linux/macOS Users

- Clang 14+ recommended for test builds; GCC 7+ supported for standard builds
- vcpkg automatically downloads and builds all dependencies
- No need for manual `apt-get` or `yum` package installations for library dependencies
- For system-wide installation, use `sudo` with `cmake --install`

## 10. Troubleshooting

### 10.1 CMake Configuration Fails

If CMake can't find the compiler:
- Ensure Visual Studio is installed (Windows)
- Ensure GCC/Clang is in PATH (Linux/macOS)
- Try running from Visual Studio Developer Command Prompt (Windows)

### 10.2 Build Fails

If build fails with missing dependencies:
- Delete `output/build/<preset-name>` and reconfigure
- vcpkg will re-download dependencies

### 10.3 Preset Not Found

If preset is not recognized:
- Ensure you're in the project root directory
- Check `CMakePresets.json` exists
- Update CMake to version 3.27 or later (3.31+ on Windows)
