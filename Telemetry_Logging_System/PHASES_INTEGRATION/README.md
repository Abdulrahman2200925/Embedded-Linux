# Phase 1: Basic Logging System

A foundational logging system implementing core logging functionality with multiple output sinks.

## Features

- Structured log messages with timestamp, severity, and context
- Multiple output destinations (Console, File)
- Extensible sink interface for custom outputs
- Type-safe severity levels
- Unit tested with Google Test and Google Mock

## Project Structure

```
PHASE_1/
├── CMakeLists.txt
├── README.md
├── app/
│   ├── CMakeLists.txt
│   └── main.cpp
├── include/
│   ├── ILogSink.hpp
│   ├── LogManager.hpp
│   ├── LogMessage.hpp
│   ├── ConsoleSinkImpl.hpp
│   └── FileSinkImpl.hpp
├── src/
│   ├── CMakeLists.txt
│   ├── LogManager.cpp
│   ├── LogMessage.cpp
│   ├── ConsoleSinkImpl.cpp
│   └── FileSinkImpl.cpp
├── tests/
│   ├── LogManager_test.cpp
│   └── MockSink.hpp
└── docs/
    └── class_diagram.puml
```

## Classes

| Class | Description |
|-------|-------------|
| `LogMessage` | Holds log data: app name, context, timestamp, severity, text |
| `ILogSink` | Abstract interface for output destinations |
| `ConsoleSinkImpl` | Writes logs to standard output |
| `FileSinkImpl` | Writes logs to a file |
| `LogManager` | Manages sinks and buffers messages |

## Severity Levels

```cpp
enum class Severity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};
```

## Build

```bash
cmake -B build && cmake --build build
```

## Run

```bash
./build/app/app
```

## Testing

### Test Structure

| File | Purpose |
|------|---------|
| `MockSink.hpp` | Mock implementation of `ILogSink` for testing |
| `LogManager_test.cpp` | Unit tests for `LogManager` class |

### Run Tests

Using CMake (if configured):
```bash
cd build && ctest
```

Using g++ directly:
```bash
g++ tests/LogManager_test.cpp src/LogManager.cpp src/LogMessage.cpp \
    -I include -lgtest -lgtest_main -lgmock -pthread -o test
./test
```

### Test Cases

| Test | Description |
|------|-------------|
| `FlushCallsWriteOnSink` | Verifies `flush()` calls `write()` on registered sinks |

### Mock Class

```cpp
class MockSink : public ILogSink {
public:
    MOCK_METHOD(void, write, (const LogMessage& message), (override));
};
```

## Usage

```cpp
#include "LogManager.hpp"
#include "LogMessage.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"

int main() {
    LogManager manager;
    
    manager.addSink(std::make_shared<ConsoleSinkImpl>());
    manager.addSink(std::make_shared<FileSinkImpl>("log.txt"));
    
    manager.log({"MyApp", "Network", Severity::INFO, "Connected"});
    manager.log({"MyApp", "Network", Severity::ERROR, "Connection lost"});
    
    manager.flush();
    
    return 0;
}
```

## Output Format

```
[AppName],[Timestamp],[Context],[Severity],[Text]
```

Example:
```
[MyApp],[2026-01-02 04:30:00],[Network],[INFO],[Connected]
[MyApp],[2026-01-02 04:30:00],[Network],[ERROR],[Connection lost]
```

## C++ Concepts Used

- Virtual functions and polymorphism
- Operator overloading (`operator<<`)
- Smart pointers (`std::shared_ptr`)
- Range-based for loops
- `enum class` for type safety
- `std::chrono` for timestamps

## Design Patterns

- **Strategy Pattern**: `ILogSink` interface allows interchangeable sink implementations

## Dependencies

- C++17
- CMake 3.16+
- Google Test (for testing)
- Google Mock (for testing)

## Topics Covered

### C++
- `virtual`, `default`, `friend`, `public`, `protected`, `private` keywords
- Classes, interfaces, and inheritance
- Virtual functions and virtual destructors
- Operator overloading
- Namespaces
- Smart pointers

### Design Patterns
- Behavioural → Strategy Pattern

### Testing
- Unit testing with Google Test
- Mocking with Google Mock
- Test-driven verification of class behavior