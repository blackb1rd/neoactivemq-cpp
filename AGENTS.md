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

## Coding Guidelines

### Avoid Static Storage

- **DO NOT** use `static` or `static thread_local` for storing state
- **PREFER** instance members over static variables
- **REASON**: Static variables introduce global state, make testing harder, and can cause initialization order issues

**Why avoid static:**
- Creates hidden global state that's hard to track
- Makes unit testing difficult (state persists between tests)
- Can cause issues in DLLs and across compilation units
- Thread-local static still has lifetime and initialization concerns
- Harder to reason about object lifecycle

**Alternative approaches:**
- Store state as instance members in the appropriate class
- Pass data through function parameters
- Use dependency injection for shared resources

**Example:**
```cpp
// BAD: Static thread-local storage
inline int& getConnectionCounter() {
    static thread_local int counter = 0;
    return counter;
}

// GOOD: Instance member
class Transport {
    int connectionCounter;
public:
    int getConnectionCounter() const {
        return connectionCounter;
    }
    void incrementCounter() {
        connectionCounter++;
    }
};
```

## Safety Rules

- **DO NOT** perform `git checkout` or switch branches without explicit user confirmation
- **DO NOT** delete large files or entire directories without explicit user confirmation
- **DO NOT** run destructive git operations (reset, rebase, force push) without user approval
- **ALWAYS** ask before making changes that could result in data loss
