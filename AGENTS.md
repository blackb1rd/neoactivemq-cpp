# Agent Instructions for activemq-cpp

## Build System

This project uses **CMake with presets**. Always use CMake presets for configuration and building.

### Configure

```powershell
# Configure with a preset (e.g., x86-windows-debug-test)
cmake --preset x86-windows-debug-test
```

### Build

```powershell
# Build using the configured preset
cmake --build --preset x86-windows-debug-test
```

### Available Presets

Check available presets:
```powershell
cmake --list-presets
```

### Test

```powershell
# Run tests after building
.\output\build\x86-windows-debug-test\bin\neoactivemq-test.exe
```

## Important Notes

- **DO NOT** run `ninja` directly in the build directory
- **DO NOT** use manual cmake configuration commands
- **ALWAYS** use `cmake --preset <preset-name>` for configuration
- **ALWAYS** use `cmake --build --preset <preset-name>` for building
- The build output is in `output/build/<preset-name>/`
