# NeoActiveMQ CPP Library

NeoActiveMQ CPP is a modernized C++17 messaging library that can use multiple protocols to talk to a MOM (e.g. ActiveMQ). This is a fork of Apache ActiveMQ CPP with updated build system and modern C++ standards.

## 1. Prerequisites

### 1.1 Required Tools

| Tool | Recommended Version |
|------|-------------------|
| CMake | >= 3.15 |
| vcpkg | latest |
| C++ Compiler | C++17 support required |
| MSVC | >= 2019 (Windows) |
| GCC | >= 7.0 (Linux) |
| Clang | >= 5.0 (macOS/Linux) |

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

Common presets:

| Preset | Description |
|--------|-------------|
| `x86-windows-debug-test` | Windows 32-bit Debug with tests |
| `x86-windows-release` | Windows 32-bit Release |
| `x64-windows-debug-test` | Windows 64-bit Debug with tests |
| `x64-windows-release` | Windows 64-bit Release |
| `x64-linux-debug-test` | Linux 64-bit Debug with tests |
| `x64-linux-release` | Linux 64-bit Release |

### 2.2 Quick Start - Windows

1. Configure the project:

   ```bash
   cmake --preset x86-windows-debug-test
   ```

   This will:
   - Download and build all dependencies via vcpkg
   - Configure the build with tests enabled
   - Generate build files in `output/build/x86-windows-debug-test/`

2. Build the project:

   ```bash
   cmake --build --preset x86-windows-debug-test
   ```

   The build output will be in:
   - Libraries: `output/build/<preset-name>/lib/`
   - Executables: `output/build/<preset-name>/bin/`

### 2.3 Quick Start - Linux/macOS

1. Configure the project:

   ```bash
   cmake --preset x64-linux-debug-test
   ```

2. Build the project:

   ```bash
   cmake --build --preset x64-linux-debug-test
   ```

### 2.4 Installation

To install the library to the system:

```bash
cmake --install output/build/<preset-name> --prefix /usr/local
```

Or on Windows with administrator privileges:

```bash
cmake --install output/build/<preset-name> --prefix "C:/Program Files/neoactivemq-cpp"
```

This installs:
- Headers to `<prefix>/include/`
- Libraries to `<prefix>/lib/`
- CMake config files to `<prefix>/lib/cmake/neoactivemq-cpp/`

## 3. Running Tests

### 3.1 Unit Tests

The test executables are built automatically when using a preset with `-test` in the name (e.g., `x86-windows-debug-test`).

To run unit tests via CTest:

```bash
ctest --preset x86-windows-debug-test -L unit
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

SSL integration tests validate the SSL/TLS transport layer against an SSL-enabled ActiveMQ broker.

```bash
# Start the SSL-enabled broker (certificates are generated automatically)
docker compose --profile ssl up -d

# Run OpenWire SSL integration tests
SSL_CERT_FILE=docker/ssl/certs/ca.pem ctest --preset <preset> -L integration-openwire-ssl --output-on-failure

# Stop the broker
docker compose --profile ssl down
```

The SSL tests cover: basic send/receive, connection lifecycle, transactions, durable subscriptions, message compression, queue browsing, virtual topics, and more -- all over SSL transport.

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
cmake --preset x86-windows-debug-test -DBUILD_EXAMPLES=OFF
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
docker/ssl/                            SSL certificate generation and broker config
output/build/                          Build output directory (created by CMake)
```

## 7. Notes for Windows Users

- Visual Studio 2019 or later is required for C++17 support
- No need to manually install dependencies -- vcpkg handles everything
- The Platform SDK is included with Visual Studio 2019+
- When linking applications:
  - **Static library** (default): No special considerations
  - **Shared library**: Ensure runtime library matches (MD vs MT flags)

## 8. Notes for Linux/macOS Users

- GCC 7+ or Clang 5+ required for C++17 support
- vcpkg automatically downloads and builds all dependencies
- No need for manual `apt-get` or `yum` package installations
- For system-wide installation, use `sudo` with `cmake --install`

## 9. Troubleshooting

### 9.1 CMake Configuration Fails

If CMake can't find the compiler:
- Ensure Visual Studio is installed (Windows)
- Ensure GCC/Clang is in PATH (Linux/macOS)
- Try running from Visual Studio Developer Command Prompt (Windows)

### 9.2 Build Fails

If build fails with missing dependencies:
- Delete `output/build/<preset-name>` and reconfigure
- vcpkg will re-download dependencies

### 9.3 Preset Not Found

If preset is not recognized:
- Ensure you're in the project root directory
- Check `CMakePresets.json` exists
- Update CMake to version 3.15 or later
