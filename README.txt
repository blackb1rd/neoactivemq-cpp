--------------------------------------------------------------------------
NeoActiveMQ CPP Library
--------------------------------------------------------------------------

NeoActiveMQ CPP is a modernized C++17 messaging library that can use
multiple protocols to talk to a MOM (e.g. ActiveMQ). This is a fork of
Apache ActiveMQ CPP with updated build system and modern C++ standards.

1 Prerequisites
--------------------------------------------------------------------------

1.1 Required Tools
--------------------------------------------------------------------------

Tool           Recommended Version
-------------------------------
CMake          >= 3.15
vcpkg          latest
C++ Compiler   C++17 support required
  - MSVC       >= 2019 (Windows)
  - GCC        >= 7.0 (Linux)
  - Clang      >= 5.0 (macOS/Linux)

1.2 Automatic Dependency Management
--------------------------------------------------------------------------

This project uses vcpkg for dependency management. All dependencies are
automatically downloaded and built during the CMake configuration phase.
No manual installation is required.

Dependencies managed by vcpkg:
  - OpenSSL     (cryptography)
  - asio        (async I/O)
  - CppUnit     (testing)
  - zlib        (compression)

2 Building with CMake Presets
--------------------------------------------------------------------------

This project uses CMake presets for simplified configuration and building.
The build system automatically manages all dependencies through vcpkg.

2.1 Available Presets
--------------------------------------------------------------------------

To see all available presets:

  cmake --list-presets

Common presets:
  - x86-windows-debug-test      (Windows 32-bit Debug with tests)
  - x86-windows-release         (Windows 32-bit Release)
  - x64-windows-debug-test      (Windows 64-bit Debug with tests)
  - x64-windows-release         (Windows 64-bit Release)
  - x64-linux-debug-test        (Linux 64-bit Debug with tests)
  - x64-linux-release           (Linux 64-bit Release)

2.2 Quick Start - Windows
--------------------------------------------------------------------------

1. Configure the project:

     cmake --preset x86-windows-debug-test

   This will:
   - Download and build all dependencies via vcpkg
   - Configure the build with tests enabled
   - Generate build files in output/build/x86-windows-debug-test/

2. Build the project:

     cmake --build --preset x86-windows-debug-test

   The build output will be in:
   - Libraries: output/build/<preset-name>/lib/
   - Executables: output/build/<preset-name>/bin/
   - Tests: output/build/<preset-name>/bin/

2.3 Quick Start - Linux/macOS
--------------------------------------------------------------------------

1. Configure the project:

     cmake --preset x64-linux-debug-test

2. Build the project:

     cmake --build --preset x64-linux-debug-test

2.4 Installation
--------------------------------------------------------------------------

To install the library to the system:

  cmake --install output/build/<preset-name> --prefix /usr/local

Or on Windows with administrator privileges:

  cmake --install output/build/<preset-name> --prefix "C:/Program Files/neoactivemq-cpp"

This installs:
  - Headers to <prefix>/include/
  - Libraries to <prefix>/lib/
  - CMake config files to <prefix>/lib/cmake/neoactivemq-cpp/

3 Running Tests
--------------------------------------------------------------------------

3.1 Unit Tests
--------------------------------------------------------------------------

The test executable is built automatically when using a preset with "-test"
in the name (e.g., x86-windows-debug-test).

To run the unit tests:

  Windows:
    .\output\build\x86-windows-debug-test\bin\neoactivemq-test.exe

  Linux/macOS:
    ./output/build/x64-linux-debug-test/bin/neoactivemq-test

This will verify that the library is functioning correctly on the target
platform.

3.2 Integration Tests
--------------------------------------------------------------------------

The library also contains tests that run against a real ActiveMQ broker.
These validate the distribution against your broker. Running without a
broker will result in failed tests.

The tests currently connect to:
  - Stomp:    tcp://localhost:61613
  - Openwire: tcp://localhost:61616

To run integration tests:

1. Start an ActiveMQ broker using Docker:

   docker-compose up -d

   Or manually (adjust version to match .env if needed):
     docker run -d --rm --name activemq-test \
       -p 61616:61616 -p 61613:61613 -p 8161:8161 \
       -e ACTIVEMQ_MIN_MEMORY=512 -e ACTIVEMQ_MAX_MEMORY=2048 \
       rmohr/activemq:5.15.9

2. Run the integration test executable:

   Windows:
     .\output\build\x86-windows-debug-test\bin\neoactivemq-integration-test.exe

   Linux/macOS:
     ./output/build/x64-linux-debug-test/bin/neoactivemq-integration-test

3. Stop the broker when done:

   docker-compose down
   # OR: docker stop activemq-test

Note: This takes considerable time. It's recommended to restart the broker
between successive test runs.

4 Examples
--------------------------------------------------------------------------

Example applications are located in activemq-cpp/src/examples/ and are
built automatically.

After building, examples are located in:
  output/build/<preset-name>/bin/examples/

Example executables follow the naming pattern:
  example_<directory>_<MainFile>

For instance:
  - example_advisories_AdvisoryConsumerMain
  - example_cmstemplate_CMSTemplateReceiverMain
  - example_producers_SimpleProducerMain

5 Using in Your Project
--------------------------------------------------------------------------

5.1 With CMake
--------------------------------------------------------------------------

After installation, use find_package in your CMakeLists.txt:

  find_package(neoactivemq-cpp REQUIRED)

  add_executable(myapp main.cpp)
  target_link_libraries(myapp PRIVATE neoactivemq-cpp::activemq-cpp)

5.2 Build Options
--------------------------------------------------------------------------

The following CMake options can be configured:

  -DBUILD_TESTS=ON/OFF          Build test executables (default: ON for -test presets)
  -DBUILD_EXAMPLES=ON/OFF       Build example applications (default: ON)
  -DAMQCPP_SHARED_LIB=ON/OFF    Build as shared library (default: OFF, builds static)

To customize a preset, pass options during configuration:

  cmake --preset x86-windows-debug-test -DBUILD_EXAMPLES=OFF

6 Project Structure
--------------------------------------------------------------------------

Key directories:
  activemq-cpp/src/main/        Main library source code
    activemq/                   ActiveMQ protocol implementation
    cms/                        CMS (Common Messaging System) API
    decaf/                      Foundation library (I/O, threading, etc.)
  activemq-cpp/src/test/        Unit tests
  activemq-cpp/src/test-integration/  Integration tests
  activemq-cpp/src/examples/    Example applications
  cmake/                        CMake configuration files
  output/build/                 Build output directory (created by CMake)

7 Notes for Windows Users
--------------------------------------------------------------------------

* Visual Studio 2019 or later is required for C++17 support
* No need to manually install dependencies - vcpkg handles everything
* The Platform SDK is included with Visual Studio 2019+
* When linking applications:
  - Static library (default): No special considerations
  - Shared library: Ensure runtime library matches (MD vs MT flags)

8 Notes for Linux/macOS Users
--------------------------------------------------------------------------

* GCC 7+ or Clang 5+ required for C++17 support
* vcpkg automatically downloads and builds all dependencies
* No need for manual apt-get or yum package installations
* For system-wide installation, use sudo with cmake --install

9 Troubleshooting
--------------------------------------------------------------------------

9.1 CMake Configuration Fails
--------------------------------------------------------------------------

If CMake can't find the compiler:
  - Ensure Visual Studio is installed (Windows)
  - Ensure GCC/Clang is in PATH (Linux/macOS)
  - Try running from Visual Studio Developer Command Prompt (Windows)

9.2 Build Fails
--------------------------------------------------------------------------

If build fails with missing dependencies:
  - Delete output/build/<preset-name> and reconfigure
  - vcpkg will re-download dependencies

9.3 Preset Not Found
--------------------------------------------------------------------------

If preset is not recognized:
  - Ensure you're in the project root directory
  - Check CMakePresets.json exists
  - Update CMake to version 3.15 or later

10 Legacy Build System
--------------------------------------------------------------------------

The original autotools-based build system has been replaced with CMake.
Legacy files are preserved in activemq-cpp/legacy-autotools/ for reference
but are no longer maintained or supported.

For the modern build system, always use CMake with presets as documented above.
