# Advanced C++ Logging & Telemetry System

> A production-ready, multi-phase logging and telemetry framework demonstrating modern C++ design patterns, RAII principles, and software architecture best practices.

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
  - [Running the Application](#running-the-application)
- [Phase Breakdown](#phase-breakdown)
  - [Phase 1: Core Logging System](#phase-1-core-logging-system)
  - [Phase 2: Telemetry Integration](#phase-2-telemetry-integration)
  - [Phase 3: Design Patterns](#phase-3-design-patterns)
  - [Bonus: Real System Monitoring](#bonus-real-system-monitoring)
- [Components](#components)
- [Design Patterns Used](#design-patterns-used)
- [Testing](#testing)
- [Usage Examples](#usage-examples)
- [API Reference](#api-reference)
- [Advanced Topics](#advanced-topics)

---

## Overview

This project is a comprehensive C++ logging and telemetry framework built incrementally across multiple phases. It demonstrates enterprise-level software engineering practices including:

- **Extensible architecture** using polymorphism and interfaces
- **RAII resource management** for safe file and socket handling
- **Multiple design patterns** (Strategy, Builder, Factory, Template, Policy-Based)
- **Real-time system monitoring** with /proc filesystem integration
- **Thread-safe data structures** (RingBuffer)
- **Comprehensive unit testing** with Google Test and Google Mock

The system can log messages to multiple destinations simultaneously (console, file, network) and integrate telemetry data from various sources (files, sockets, system metrics).

---

## Key Features

### Core Capabilities
- **Multi-Sink Architecture**: Log to console, files, and network sockets simultaneously
- **Structured Logging**: Type-safe severity levels with timestamp, context, and application metadata
- **Telemetry Integration**: Read sensor data from files, network sources, or system metrics
- **RAII Wrappers**: Safe resource handling with automatic cleanup (SafeFile, SafeSocket)
- **Extensible Design**: Easy to add new sinks, sources, or telemetry policies

### Advanced Features
- **Builder Pattern**: Fluent API for constructing LogManager instances
- **Factory Pattern**: Centralized creation of log sinks without tight coupling
- **Template Programming**: Generic RingBuffer for various data types
- **Policy-Based Design**: Configurable telemetry formatting (CPU, RAM, GPU policies)
- **Real System Monitoring**: Live CPU and memory usage tracking from /proc filesystem

### Quality Assurance
- **Unit Testing**: Comprehensive test coverage with Google Test
- **Mock Objects**: Isolated testing with Google Mock
- **Type Safety**: Strong typing with enum classes
- **Modern C++17**: Smart pointers, RAII, range-based loops

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                        │
│                         (main.cpp)                           │
└────────────────────┬────────────────────────────────────────┘
                     │
        ┌────────────┴─────────────┐
        │                          │
┌───────▼──────────┐      ┌────────▼────────────┐
│   LogManager     │      │ Telemetry Sources   │
│   (Core System)  │      │ (Data Providers)    │
│                  │      │                     │
│  - addSink()     │      │ - FileTelemetry     │
│  - log()         │      │ - SocketTelemetry   │
│  - flush()       │      │ - SystemTelemetry   │
└────────┬─────────┘      └─────────────────────┘
         │
    ┌────┴─────┐
    │  Sinks   │
    │          │
┌───▼───┐ ┌───▼───┐ ┌────▼────┐
│Console│ │ File  │ │ Socket  │
│ Sink  │ │ Sink  │ │  Sink   │
└───────┘ └───────┘ └─────────┘
```

**Data Flow**:
1. Telemetry sources read data (files, sockets, /proc)
2. Data is formatted using policy-based LogFormatter
3. LogManager receives structured LogMessage objects
4. Messages are distributed to all registered sinks
5. Each sink writes to its destination (console/file/network)

---

## Project Structure

```
PHASES_INTEGRATION/
├── CMakeLists.txt              # Root CMake configuration
├── README.md                   # Project documentation
│
├── include/                    # Public headers
│   ├── ILogSink.hpp           # Log sink interface
│   ├── ConsoleSinkImpl.hpp    # Console output sink
│   ├── FileSinkImpl.hpp       # File output sink
│   ├── SocketSinkImpl.hpp     # Network output sink
│   ├── ITelemetrySource.hpp   # Telemetry source interface
│   ├── FileTelemetrySourceImpl.hpp    # File-based telemetry
│   ├── SocketTelemetrySourceImpl.hpp  # Network-based telemetry
│   ├── SystemTelemetryWriter.hpp      # /proc parser
│   ├── LogManager.hpp         # Core logging orchestrator
│   ├── LogManagerBuilder.hpp  # Builder pattern implementation
│   ├── LogSinkFactory.hpp     # Factory pattern implementation
│   ├── LogMessage.hpp         # Log message data structure
│   ├── LogFormatter.hpp       # Policy-based formatter
│   ├── RingBuffer.hpp         # Thread-safe circular buffer
│   ├── SafeFile.hpp           # RAII file wrapper
│   ├── SafeSocket.hpp         # RAII socket wrapper
│   ├── CpuPolicy.hpp          # CPU telemetry policy
│   ├── RamPolicy.hpp          # RAM telemetry policy
│   ├── GpuPolicy.hpp          # GPU telemetry policy
│   ├── SeverityLvl_enum.hpp   # Log severity levels
│   ├── LogSinkType_enum.hpp   # Sink type enumeration
│   └── TelemetrySrc_enum.hpp  # Telemetry source types
│
├── src/                       # Implementation files
│   ├── LogManager.cpp
│   ├── LogManagerBuilder.cpp
│   ├── LogSinkFactory.cpp
│   ├── LogMessage.cpp
│   ├── ConsoleSinkImpl.cpp
│   ├── FileSinkImpl.cpp
│   ├── SocketSinkImpl.cpp
│   ├── FileTelemetrySourceImpl.cpp
│   ├── SocketTelemetrySourceImpl.cpp
│   ├── SystemTelemetryWriter.cpp
│   ├── SafeFile.cpp
│   └── SafeSocket.cpp
│
├── app/                       # Application entry point
│   └── main.cpp              # Integration test & demo
│
├── tests/                     # Unit tests
│   ├── MockSink.hpp          # Mock implementations
│   ├── LogManger_test.cpp    # LogManager tests
│   ├── SafeFile_test.cpp     # SafeFile tests
│   ├── SafeSocket_test.cpp   # SafeSocket tests
│   ├── FileTelemetrySourceImpl_test.cpp
│   ├── SocketTelemetrySourceImpl_test.cpp
│   ├── phase3_tests.cpp      # Design pattern tests
│   └── integration_test.cpp  # End-to-end tests
│
├── docs/                      # Documentation
│   └── class_diagram.puml    # UML diagrams
│
└── build/                     # Build artifacts (generated)
```

---

## Getting Started

### Prerequisites

- **C++ Compiler**: GCC 7.0+ or Clang 5.0+ with C++17 support
- **CMake**: Version 3.16.0 or higher
- **Google Test**: For running unit tests (optional)
- **Google Mock**: For mocking in tests (optional)
- **Linux**: Required for bonus /proc filesystem features

### Building the Project

```bash
# Clone or navigate to the project directory
cd PHASES_INTEGRATION

# Create build directory and generate build files
cmake -B build

# Build the project
cmake --build build

# Alternative: Build with parallel jobs
cmake --build build -j$(nproc)
```

### Running the Application

```bash
# Run the main integration test
./build/app/app
```

This will execute all four test phases and generate log files:
- `phase1_test.log` - Basic logging demonstrations
- `phase2_test.log` - Telemetry integration examples
- `phase3_test.log` - Design pattern demonstrations
- `factory_test.log` - Factory pattern outputs
- `system_telemetry.log` - Real system metrics

### Running Tests

```bash
# Run all tests using CTest
cd build
ctest --output-on-failure

# Or run individual test executables
./tests/LogManager_test
./tests/SafeFile_test
./tests/integration_test
```

---

## Phase Breakdown

### Phase 1: Core Logging System

**Objective**: Build a foundational logging framework with multiple output destinations.

**Components**:
- `LogMessage`: Data structure holding log metadata (timestamp, severity, context, message)
- `ILogSink`: Abstract interface for output destinations
- `ConsoleSinkImpl`: Writes logs to stdout
- `FileSinkImpl`: Writes logs to files with RAII management
- `LogManager`: Orchestrates multiple sinks and buffers messages

**Key Concepts**:
- Polymorphism and virtual functions
- Strategy pattern for interchangeable sinks
- Operator overloading for output streams
- Smart pointers (`std::shared_ptr`)

**Severity Levels**:
```cpp
enum class Severity {
    DEBUG,      // Detailed debugging information
    INFO,       // General informational messages
    WARNING,    // Warning messages for potential issues
    ERROR,      // Error messages for failures
    CRITICAL    // Critical failures requiring immediate attention
};
```

**Example**:
```cpp
LogManager logger;
logger.addSink(std::make_shared<ConsoleSinkImpl>());
logger.addSink(std::make_shared<FileSinkImpl>("app.log"));

logger.log(LogMessage("MyApp", "Network", Severity::INFO, "Connection established"));
logger.flush();
```

---

### Phase 2: Telemetry Integration

**Objective**: Add ability to read telemetry data from external sources and feed into logging system.

**Components**:
- `ITelemetrySource`: Abstract interface for telemetry data sources
- `FileTelemetrySourceImpl`: Reads telemetry from files using SafeFile
- `SocketTelemetrySourceImpl`: Reads telemetry from network sockets using SafeSocket
- `SafeFile`: RAII wrapper for file descriptors
- `SafeSocket`: RAII wrapper for socket descriptors

**Key Concepts**:
- RAII (Resource Acquisition Is Initialization)
- Rule of Five (destructor, copy/move constructors/assignments)
- Exception safety
- File I/O and socket programming

**Example**:
```cpp
FileTelemetrySourceImpl telemetry("/var/log/sensor_data.txt");

if (telemetry.openSource()) {
    std::string data;
    while (telemetry.readSource(data)) {
        logger.log(LogMessage("Telemetry", "Sensor", Severity::INFO, data));
    }
}
```

---

### Phase 3: Design Patterns

**Objective**: Refactor and enhance the codebase using industry-standard design patterns.

#### 3.1 Builder Pattern
**Purpose**: Fluent API for constructing complex LogManager objects.

```cpp
LogManager logger = LogManagerBuilder()
    .addSink(std::make_shared<ConsoleSinkImpl>())
    .addSink(std::make_shared<FileSinkImpl>("output.log"))
    .build();
```

#### 3.2 Factory Pattern
**Purpose**: Centralized sink creation without exposing concrete classes.

```cpp
auto consoleSink = LogSinkFactory::create(LogSinkType_enum::Console);
auto fileSink = LogSinkFactory::create(LogSinkType_enum::File, "app.log");
auto socketSink = LogSinkFactory::create(LogSinkType_enum::Socket, "192.168.1.100:8080");
```

#### 3.3 Template Programming
**Purpose**: Generic, type-safe data structures.

```cpp
// Thread-safe ring buffer for any type
RingBuffer<std::string> messageBuffer(100);
RingBuffer<int> sensorReadings(50);

messageBuffer.tryPush("Temperature: 25.5°C");
std::string data;
messageBuffer.tryPop(data);
```

#### 3.4 Policy-Based Design
**Purpose**: Compile-time configuration of behavior through template policies.

```cpp
// Different policies for different telemetry types
LogFormatter<CpuPolicy> cpuFormatter;
LogFormatter<RamPolicy> ramFormatter;
LogFormatter<GpuPolicy> gpuFormatter;

auto cpuLog = cpuFormatter.formatDataToLogMsg("85.5");  // High CPU usage
auto ramLog = ramFormatter.formatDataToLogMsg("45.2");  // Normal RAM usage
auto gpuLog = gpuFormatter.formatDataToLogMsg("95.8");  // Critical GPU usage
```

**Policies**:
- **CpuPolicy**: Formats CPU usage, WARNING >80%, CRITICAL >95%
- **RamPolicy**: Formats RAM usage, WARNING >75%, CRITICAL >90%
- **GpuPolicy**: Formats GPU usage, WARNING >85%, CRITICAL >95%

---

### Bonus: Real System Monitoring

**Objective**: Parse actual system metrics from Linux /proc filesystem.

**Components**:
- `SystemTelemetryWriter`: Parses `/proc/stat` for CPU metrics and `/proc/meminfo` for memory stats

**Key Features**:
- Real-time CPU usage calculation
- Memory usage (total, free, available)
- Integration with telemetry pipeline

**Data Flow**:
```
/proc/stat → SystemTelemetryWriter → telemetry.dat → FileTelemetrySource → LogManager → Sinks
```

**Example**:
```cpp
SystemTelemetryWriter writer("/tmp/system_telemetry.dat");
writer.writeCpuTelemetry();      // CPU usage from /proc/stat
writer.writeMemoryTelemetry();   // Memory from /proc/meminfo

FileTelemetrySourceImpl source("/tmp/system_telemetry.dat");
// Read and log real system metrics...
```

---

## Components

### Core Components

| Component | Purpose | Key Methods |
|-----------|---------|-------------|
| **LogManager** | Orchestrates logging operations | `addSink()`, `log()`, `flush()` |
| **LogMessage** | Structured log data container | Constructor, getters, `operator<<` |
| **ILogSink** | Interface for output destinations | `write()` (pure virtual) |

### Sink Implementations

| Sink | Destination | Configuration |
|------|-------------|---------------|
| **ConsoleSinkImpl** | Standard output | None |
| **FileSinkImpl** | File system | File path |
| **SocketSinkImpl** | Network socket | Host:Port |

### Telemetry Components

| Component | Purpose | Input Source |
|-----------|---------|--------------|
| **FileTelemetrySourceImpl** | Read from files | File path |
| **SocketTelemetrySourceImpl** | Read from network | Socket connection |
| **SystemTelemetryWriter** | Parse /proc filesystem | `/proc/stat`, `/proc/meminfo` |

### RAII Wrappers

| Wrapper | Resource | Features |
|---------|----------|----------|
| **SafeFile** | File descriptor | Auto-close, move semantics, error handling |
| **SafeSocket** | Socket descriptor | Auto-close, connection management |

### Utilities

| Component | Type | Purpose |
|-----------|------|---------|
| **RingBuffer<T>** | Template class | Thread-safe circular buffer |
| **LogFormatter<Policy>** | Template class | Policy-based telemetry formatting |
| **LogManagerBuilder** | Builder | Fluent API for LogManager construction |
| **LogSinkFactory** | Factory | Centralized sink creation |

---

## Design Patterns Used

### 1. Strategy Pattern
**Where**: `ILogSink` interface with multiple implementations (Console, File, Socket).

**Benefit**: Allows runtime selection and combination of different logging strategies.

### 2. Builder Pattern
**Where**: `LogManagerBuilder` for constructing LogManager instances.

**Benefit**: Provides a fluent, readable API for complex object construction.

### 3. Factory Pattern
**Where**: `LogSinkFactory` for creating sink instances.

**Benefit**: Decouples sink creation from usage, enables configuration-driven instantiation.

### 4. Template Method Pattern
**Where**: `ITelemetrySource` defines the template for reading telemetry data.

**Benefit**: Standardizes the telemetry reading workflow across different sources.

### 5. Policy-Based Design
**Where**: `LogFormatter<Policy>` with different policy classes (CpuPolicy, RamPolicy, GpuPolicy).

**Benefit**: Compile-time customization without runtime overhead.

### 6. RAII Pattern
**Where**: `SafeFile` and `SafeSocket` for resource management.

**Benefit**: Automatic cleanup, exception safety, prevents resource leaks.

---

## Testing

### Test Framework
- **Google Test (gtest)**: Unit testing framework
- **Google Mock (gmock)**: Mocking framework for interfaces

### Test Coverage

| Test Suite | File | Coverage |
|------------|------|----------|
| LogManager Tests | `LogManger_test.cpp` | Sink registration, message logging, flush operations |
| SafeFile Tests | `SafeFile_test.cpp` | File opening, reading, writing, RAII behavior |
| SafeSocket Tests | `SafeSocket_test.cpp` | Socket creation, connection, RAII behavior |
| Telemetry Tests | `FileTelemetrySourceImpl_test.cpp` | File-based telemetry reading |
| Socket Telemetry | `SocketTelemetrySourceImpl_test.cpp` | Network-based telemetry |
| Design Patterns | `phase3_tests.cpp` | Builder, Factory, Template, Policy patterns |
| Integration Tests | `integration_test.cpp` | End-to-end workflows |

### Mock Objects

```cpp
class MockSink : public ILogSink {
public:
    MOCK_METHOD(void, write, (const LogMessage& message), (override));
};

// Usage in tests
TEST(LogManagerTest, FlushCallsWriteOnSink) {
    auto mockSink = std::make_shared<MockSink>();
    EXPECT_CALL(*mockSink, write(_)).Times(1);

    LogManager manager;
    manager.addSink(mockSink);
    manager.log(LogMessage("App", "Test", Severity::INFO, "Message"));
    manager.flush();
}
```

### Running Tests

```bash
# Build and run all tests
cd build
ctest -V

# Run specific test
./tests/LogManager_test

# Run with detailed output
./tests/integration_test --gtest_verbose
```

---

## Usage Examples

### Basic Logging

```cpp
#include "LogManager.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"

int main() {
    LogManager logger;

    // Add output destinations
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    logger.addSink(std::make_shared<FileSinkImpl>("application.log"));

    // Log messages with different severities
    logger.log(LogMessage("MyApp", "Startup", Severity::INFO, "Application started"));
    logger.log(LogMessage("MyApp", "Auth", Severity::WARNING, "Login attempt failed"));
    logger.log(LogMessage("MyApp", "Database", Severity::ERROR, "Connection timeout"));

    // Flush buffers to sinks
    logger.flush();

    return 0;
}
```

### Using Builder Pattern

```cpp
#include "LogManagerBuilder.hpp"

LogManager logger = LogManagerBuilder()
    .addSink(std::make_shared<ConsoleSinkImpl>())
    .addSink(std::make_shared<FileSinkImpl>("app.log"))
    .addSink(std::make_shared<SocketSinkImpl>("logserver.example.com:9000"))
    .build();
```

### Telemetry Integration

```cpp
#include "FileTelemetrySourceImpl.hpp"
#include "LogFormatter.hpp"
#include "CpuPolicy.hpp"

// Read telemetry from file
FileTelemetrySourceImpl cpuSource("/sys/class/thermal/thermal_zone0/temp");

if (cpuSource.openSource()) {
    LogFormatter<CpuPolicy> formatter;

    std::string temperature;
    while (cpuSource.readSource(temperature)) {
        auto logMsg = formatter.formatDataToLogMsg(temperature);
        if (logMsg.has_value()) {
            logger.log(logMsg.value());
        }
    }
}
```

### Policy-Based Formatting

```cpp
#include "LogFormatter.hpp"
#include "RamPolicy.hpp"

// Create formatter with specific policy
LogFormatter<RamPolicy> ramFormatter;

// Format telemetry data (automatically determines severity)
auto logMessage = ramFormatter.formatDataToLogMsg("82.5");  // 82.5% RAM usage

if (logMessage.has_value()) {
    logger.log(logMessage.value());  // Logs with WARNING severity
}
```

### Real System Monitoring

```cpp
#include "SystemTelemetryWriter.hpp"

// Write system metrics to file
SystemTelemetryWriter writer("/tmp/metrics.dat");
writer.writeCpuTelemetry();       // Parse /proc/stat
writer.writeMemoryTelemetry();    // Parse /proc/meminfo

// Read and log the metrics
FileTelemetrySourceImpl source("/tmp/metrics.dat");
source.openSource();

std::string metric;
while (source.readSource(metric)) {
    logger.log(LogMessage("System", "Monitor", Severity::INFO, metric));
}
```

---

## API Reference

### LogManager

```cpp
class LogManager {
public:
    void addSink(std::shared_ptr<ILogSink> sink);
    void log(const LogMessage& message);
    void flush();
};
```

### LogMessage

```cpp
struct LogMessage {
    LogMessage(std::string appName, std::string context,
               Severity severity, std::string text);

    std::string getAppName() const;
    std::string getContext() const;
    Severity getSeverity() const;
    std::string getText() const;
    std::string getTimestamp() const;
};
```

### ILogSink (Interface)

```cpp
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogMessage& message) = 0;
};
```

### ITelemetrySource (Interface)

```cpp
class ITelemetrySource {
public:
    virtual ~ITelemetrySource() = default;
    virtual bool openSource() = 0;
    virtual bool readSource(std::string& data) = 0;
    virtual void closeSource() = 0;
};
```

### LogSinkFactory

```cpp
class LogSinkFactory {
public:
    static std::shared_ptr<ILogSink> create(
        LogSinkType_enum type,
        const std::string& config = ""
    );
};
```

### RingBuffer<T>

```cpp
template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity);

    bool tryPush(const T& item);
    bool tryPop(T& item);
    size_t size() const;
    size_t getCapacity() const;
    bool isEmpty() const;
    bool isFull() const;
};
```

---

## Advanced Topics

### Thread Safety

The `RingBuffer<T>` is designed to be thread-safe for single-producer, single-consumer scenarios. For multi-threaded logging, consider:

```cpp
// Use mutex protection for LogManager in multi-threaded contexts
std::mutex logMutex;

void threadSafeLog(LogManager& logger, const LogMessage& msg) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger.log(msg);
}
```

### Custom Sink Implementation

To create a custom sink:

```cpp
class DatabaseSink : public ILogSink {
public:
    DatabaseSink(const std::string& connectionString)
        : conn_(connectionString) {}

    void write(const LogMessage& message) override {
        // Your database insertion logic
        conn_.execute("INSERT INTO logs VALUES (?, ?, ?, ?)",
            message.getTimestamp(),
            message.getSeverity(),
            message.getContext(),
            message.getText()
        );
    }

private:
    DatabaseConnection conn_;
};
```

### Custom Telemetry Policy

To create a custom policy:

```cpp
struct NetworkPolicy {
    static constexpr const char* getAppName() { return "NetworkMonitor"; }
    static constexpr const char* getContext() { return "Bandwidth"; }

    static Severity determineSeverity(double value) {
        if (value > 900.0) return Severity::CRITICAL;  // >900 Mbps
        if (value > 700.0) return Severity::WARNING;   // >700 Mbps
        return Severity::INFO;
    }

    static std::string formatMessage(double value) {
        return "Network bandwidth: " + std::to_string(value) + " Mbps";
    }
};

// Usage
LogFormatter<NetworkPolicy> netFormatter;
```

### Output Format Customization

Log messages are formatted as:
```
[AppName],[Timestamp],[Context],[Severity],[Text]
```

Example output:
```
[MyApp],[2026-01-23 14:30:45],[Network],[INFO],[Connection established]
[MyApp],[2026-01-23 14:30:50],[Database],[ERROR],[Query timeout after 5000ms]
[SystemMonitor],[2026-01-23 14:31:00],[Telemetry],[WARNING],[CPU_USAGE:85.5%]
```

To customize the format, modify the `operator<<` overload in sink implementations.

---

## C++ Concepts Demonstrated

### Modern C++ Features (C++17)
- **Smart Pointers**: `std::shared_ptr`, `std::unique_ptr`
- **Move Semantics**: Efficient resource transfer
- **RAII**: Automatic resource management
- **std::optional**: Optional return values
- **Structured Bindings**: Decomposing return values
- **constexpr**: Compile-time evaluation

### Object-Oriented Programming
- **Polymorphism**: Virtual functions, interfaces
- **Encapsulation**: Public/private/protected access
- **Inheritance**: Base classes and derived implementations
- **Operator Overloading**: Custom `operator<<`

### Generic Programming
- **Templates**: Class templates (`RingBuffer<T>`)
- **Template Specialization**: Policy-based design
- **Type Traits**: Compile-time type information

### Design Principles
- **SOLID Principles**: Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion
- **DRY**: Don't Repeat Yourself
- **Separation of Concerns**: Clear module boundaries

---

## Dependencies

| Dependency | Version | Purpose | Required |
|------------|---------|---------|----------|
| **C++ Compiler** | GCC 7.0+ or Clang 5.0+ | C++17 compilation | Yes |
| **CMake** | 3.16.0+ | Build system | Yes |
| **Google Test** | Latest | Unit testing | No (for tests) |
| **Google Mock** | Latest | Mocking framework | No (for tests) |
| **Linux** | Any modern distro | /proc filesystem | No (for bonus features) |

---

## Building Without CMake

If you prefer manual compilation:

```bash
# Compile library
g++ -std=c++17 -c src/*.cpp -I include/

# Link executable
g++ -std=c++17 -o app app/main.cpp *.o -I include/

# Run
./app
```

---

## Troubleshooting

### Common Issues

**Issue**: `undefined reference to pthread_create`
```bash
# Solution: Link pthread library
cmake --build build -- -lpthread
```

**Issue**: `/proc/stat: Permission denied`
```bash
# Solution: Ensure read permissions on /proc
ls -la /proc/stat
```

**Issue**: `Could not find package gtest`
```bash
# Solution: Install Google Test
sudo apt-get install libgtest-dev libgmock-dev  # Ubuntu/Debian
brew install googletest                          # macOS
```

---

## License

This project is created for educational purposes as part of a C++ software engineering curriculum.

---

## Author



---

## Acknowledgments

- Design patterns inspired by Gang of Four (GoF) and modern C++ best practices
- RAII concepts from Bjarne Stroustrup's C++ guidelines
- Testing methodology from Google Test documentation

---

**Last Updated**: January 23, 2026
**Project Version**: 1.0.0
**C++ Standard**: C++17
