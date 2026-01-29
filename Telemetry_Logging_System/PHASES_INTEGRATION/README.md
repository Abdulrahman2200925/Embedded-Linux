# 🚀 Multi-Threaded Logging & Telemetry System

A high-performance, asynchronous logging framework with real-time system telemetry monitoring, built from scratch in modern C++17.

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Phase Evolution](#phase-evolution)
- [Quick Start](#quick-start)
- [Usage Examples](#usage-examples)
- [Design Patterns](#design-patterns)
- [Performance](#performance)
- [Project Structure](#project-structure)
- [Building](#building)
- [Future Work](#future-work)

---

## 🎯 Overview

This project implements a **production-grade logging system** that continuously monitors system resources (CPU, RAM, GPU) and logs events asynchronously using multi-threading. Think of it as a mini version of enterprise logging systems like **syslog** or **Logstash**, but built entirely from the ground up to understand the internals.

### What Makes It Special?

- ⚡ **Zero-Blocking Logging**: Producer threads never wait - messages flow through a lock-free ring buffer
- 🔄 **Parallel Processing**: ThreadPool distributes sink writes across multiple worker threads
- 📊 **Real-Time Telemetry**: Continuous background monitoring of system resources
- 🎛️ **Policy-Based Design**: Compile-time configuration using C++ policy classes
- 🛡️ **RAII-Safe**: All resources (files, sockets, threads) are automatically managed

---

## ✨ Features

### Core Logging
- 📝 **Multiple Sink Support**: Console, File, Unix Domain Sockets
- 🎨 **Structured Messages**: Timestamp, severity, context, and application name
- 🏭 **Factory Pattern**: Easy creation of different sink types
- 🔨 **Builder Pattern**: Fluent API for configuring the log manager

### Asynchronous Processing (Phase 4)
- 🔁 **Ring Buffer**: Thread-safe circular buffer with condition variables
- 👷 **Thread Pool**: Reusable worker threads for parallel sink operations
- 🧵 **Consumer Thread**: Dedicated thread for buffer processing
- 🚪 **Graceful Shutdown**: Clean thread termination with buffer draining

### Telemetry System (Phase 3)
- 📈 **System Monitoring**: CPU, RAM, GPU usage tracking
- 🤖 **Auto-Classification**: Automatic severity inference (INFO/WARNING/CRITICAL)
- 📡 **Multiple Sources**: File-based, socket-based, or system-based readers
- ⏱️ **Configurable Intervals**: Customize reading frequency per source

### Safety & Reliability
- 🔒 **Thread-Safe**: Mutex protection on all shared state
- 💾 **RAII Wrappers**: SafeFile and SafeSocket prevent resource leaks
- ⚠️ **Exception Handling**: Robust error handling throughout
- 🧪 **Move Semantics**: Efficient resource transfer, no copies

---

## 🏗️ Architecture

### High-Level Flow
```
┌─────────────────────────────────────────────────────────────┐
│                      Main Application                        │
│  • Creates LogManager with ThreadPool                        │
│  • Spawns TelemetryReaders (CPU, RAM, GPU)                  │
│  • Each reader runs in its own thread                        │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
│ TelemetryReader  │ │ TelemetryReader  │ │ TelemetryReader  │
│   <CpuPolicy>    │ │   <RamPolicy>    │ │   <GpuPolicy>    │
│                  │ │                  │ │                  │
│ • Reads source   │ │ • Reads source   │ │ • Reads source   │
│ • Formats data   │ │ • Formats data   │ │ • Formats data   │
│ • Calls log()    │ │ • Calls log()    │ │ • Calls log()    │
└──────────────────┘ └──────────────────┘ └──────────────────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              ▼
                    ┌──────────────────┐
                    │   LogManager     │
                    │                  │
                    │ • RingBuffer     │ ◄── Multiple producers (non-blocking)
                    │ • Mutex locked   │
                    └──────────────────┘
                              ▼
                    ┌──────────────────┐
                    │ Consumer Thread  │
                    │                  │
                    │ • Pops messages  │
                    │ • Sends to pool  │
                    └──────────────────┘
                              ▼
                    ┌──────────────────┐
                    │   ThreadPool     │
                    │                  │
                    │ • 4-8 workers    │
                    │ • Parallel write │
                    └──────────────────┘
                              ▼
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│ ConsoleSink  │    │  FileSink    │    │ SocketSink   │
└──────────────┘    └──────────────┘    └──────────────┘
```

### Thread Architecture

| Thread Type | Count | Purpose |
|------------|-------|---------|
| **Main Thread** | 1 | Orchestrates startup/shutdown |
| **Telemetry Readers** | 3+ | Continuous data collection (CPU, RAM, GPU) |
| **Consumer Thread** | 1 | Processes ring buffer |
| **Worker Threads** | 4-8 | Parallel sink writing |
| **Generator Thread** | 1 | (Optional) Simulates telemetry data |

**Total: ~10 threads running concurrently!** 🔥

---

## 🔄 Phase Evolution

This project was built incrementally across multiple phases:

### Phase 1: Foundation (Basic Logging)
**Goal**: Build a simple synchronous logging system

**Topics Covered**:
- C++ OOP fundamentals
- Enums and switch statements
- File I/O with `fstream`
- Operator overloading

**Deliverables**:
- ✅ `LogMessage` class with severity levels
- ✅ Console and File sinks
- ✅ `LogManager` for sink management
- ✅ Builder pattern for configuration

### Phase 2: Design Patterns & Safety
**Goal**: Make the system more flexible and safer

**Topics Covered**:
- Factory pattern
- RAII (Resource Acquisition Is Initialization)
- Move semantics
- Unix domain sockets

**Deliverables**:
- ✅ `LogSinkFactory` for sink creation
- ✅ `SafeFile` and `SafeSocket` RAII wrappers
- ✅ Socket-based sink implementation
- ✅ Move constructors/assignment operators

### Phase 3: Telemetry & Policy-Based Design
**Goal**: Add real-time system monitoring with generic programming

**Topics Covered**:
- Template classes and functions
- Policy-based design
- `/proc` filesystem parsing (Linux)
- Compile-time configuration

**Deliverables**:
- ✅ `ITelemetrySource` interface
- ✅ Policy classes (CpuPolicy, RamPolicy, GpuPolicy)
- ✅ `LogFormatter<Policy>` template
- ✅ `SystemTelemetryWriter` for CPU/RAM monitoring

### Phase 4: Asynchronous & Multi-Threading ⭐ **(Current)**
**Goal**: Transform into a high-performance async system

**Topics Covered**:
- `std::thread` fundamentals
- `std::mutex` and `std::lock_guard`
- `std::condition_variable` for producer-consumer
- `std::atomic` for lock-free flags
- Thread pools and worker patterns

**Deliverables**:
- ✅ Thread-safe `RingBuffer<T>` with blocking/non-blocking ops
- ✅ `ThreadPool` for parallel task execution
- ✅ Refactored `LogManager` with consumer thread
- ✅ `TelemetryReader<Policy>` for continuous monitoring
- ✅ Graceful shutdown with buffer draining

---

## 🚀 Quick Start

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# Fedora/RHEL
sudo dnf install gcc-c++ cmake
```

### Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Run
```bash
./app/app
```

### Expected Output
```
=================================================
    Phase 4: Asynchronous Logging System
=================================================

[MAIN] Creating LogManager...
ThreadPool: Created with 4 worker threads
[MAIN] LogManager started

[MAIN] Starting telemetry readers...
TelemetryReader: Started (CPU)
TelemetryReader: Started (RAM)
TelemetryReader: Started (GPU)

[CPU],[2025-01-29 14:23:45],[Telemetry],[INFO],[CPU usage at 45.30%]
[RAM],[2025-01-29 14:23:46],[Telemetry],[WARNING],[RAM usage at 72.50%]
[GPU],[2025-01-29 14:23:47],[Telemetry],[CRITICAL],[GPU usage at 96.20%]
...

Press Ctrl+C to stop
```

---

## 💡 Usage Examples

### Example 1: Basic Logging
```cpp
// Create logger with 100-message buffer and 4 worker threads
LogManager logger(100, 4);

// Add sinks
logger.addSink(LogSinkFactory::create(LogSinkType_enum::Console));
logger.addSink(LogSinkFactory::create(LogSinkType_enum::File, "app.log"));

// Start the consumer thread
logger.start();

// Log from anywhere (thread-safe!)
logger.log(LogMessage("MyApp", "Init", Severity::INFO, "Application started"));
logger.log(LogMessage("MyApp", "Database", Severity::WARNING, "Connection slow"));

// Graceful shutdown (drains buffer)
logger.stop();
```

### Example 2: Custom Telemetry Reader
```cpp
// Create a telemetry source
FileTelemetrySourceImpl cpuSource("/tmp/cpu_data.txt");

// Create reader with CpuPolicy (reads every 500ms)
TelemetryReader<CpuPolicy> cpuReader(
    cpuSource, 
    logger,
    std::chrono::milliseconds(500)
);

// Start background reading
cpuReader.start();

// Reader continuously:
// 1. Reads from source
// 2. Parses value (e.g., "75.5")
// 3. Infers severity using CpuPolicy thresholds
// 4. Logs formatted message

// Stop when done
cpuReader.stop();
```

### Example 3: Builder Pattern
```cpp
auto logger = LogManagerBuilder()
    .setBufferSize(200)
    .setThreadPoolSize(8)
    .addSink(LogSinkFactory::create(LogSinkType_enum::Console))
    .addSink(LogSinkFactory::create(LogSinkType_enum::File, "system.log"))
    .build();

logger->start();
// Use logger...
logger->stop();
```

---

## 🎨 Design Patterns

### Factory Pattern
```cpp
// Abstract factory for creating sinks
auto sink = LogSinkFactory::create(LogSinkType_enum::File, "output.log");
```

### Builder Pattern
```cpp
// Fluent API for configuration
LogManagerBuilder()
    .setBufferSize(100)
    .addSink(consoleSink)
    .build();
```

### Policy-Based Design
```cpp
// Compile-time customization via policy classes
template<typename Policy>
class LogFormatter {
    // Uses Policy::context, Policy::unit, Policy::inferSeverity()
};
```

### Producer-Consumer Pattern
```cpp
// Multiple producers (TelemetryReaders)
// Single consumer (LogManager's consumer thread)
// Synchronized via RingBuffer + condition_variable
```

### Thread Pool Pattern
```cpp
// Reusable worker threads
// Task queue with synchronization
threadPool.enqueue([]() { /* work */ });
```

---

## ⚡ Performance

### Design Decisions for Speed

1. **Non-Blocking Producers**: 
   - `log()` uses `tryPush()` - returns immediately if buffer full
   - No producer thread ever waits

2. **Lock-Free Reading**:
   - Ring buffer uses single mutex (minimal contention)
   - Condition variables avoid busy-waiting

3. **Parallel Sink Writing**:
   - ThreadPool distributes writes across workers
   - File/Socket I/O happens in parallel

4. **Move Semantics**:
   - `LogMessage` moved through buffer (no copies)
   - RAII wrappers use move-only semantics

### Benchmarking Ideas
```cpp
// Throughput test
auto start = std::chrono::high_resolution_clock::now();
for (int i = 0; i < 100000; ++i) {
    logger.log(msg);
}
auto end = std::chrono::high_resolution_clock::now();
// Measure messages/second
```

---

## 📂 Project Structure
```
.
├── app/
│   └── main.cpp                    # Main application entry point
├── include/
│   ├── ConsoleSinkImpl.hpp         # Console output sink
│   ├── FileSinkImpl.hpp            # File output sink
│   ├── SocketSinkImpl.hpp          # Unix socket sink
│   ├── ILogSink.hpp                # Sink interface
│   ├── LogManager.hpp              # Core logging manager (Phase 4)
│   ├── LogManagerBuilder.hpp       # Builder pattern
│   ├── LogMessage.hpp              # Log message structure
│   ├── LogSinkFactory.hpp          # Factory pattern
│   ├── RingBuffer.hpp              # Thread-safe circular buffer (Phase 4)
│   ├── ThreadPool.hpp              # Worker thread pool (Phase 4)
│   ├── TelemetryReader.hpp         # Continuous reader (Phase 4)
│   ├── ITelemetrySource.hpp        # Telemetry source interface
│   ├── FileTelemetrySourceImpl.hpp # File-based source
│   ├── SocketTelemetrySourceImpl.hpp # Socket-based source
│   ├── SystemTelemetryWriter.hpp   # /proc reader
│   ├── LogFormatter.hpp            # Policy-based formatter
│   ├── CpuPolicy.hpp               # CPU thresholds & logic
│   ├── RamPolicy.hpp               # RAM thresholds & logic
│   ├── GpuPolicy.hpp               # GPU thresholds & logic
│   ├── SafeFile.hpp                # RAII file wrapper
│   └── SafeSocket.hpp              # RAII socket wrapper
├── src/
│   ├── ConsoleSinkImpl.cpp
│   ├── FileSinkImpl.cpp
│   ├── SocketSinkImpl.cpp
│   ├── LogManager.cpp              # Multi-threaded implementation
│   ├── LogManagerBuilder.cpp
│   ├── LogMessage.cpp
│   ├── LogSinkFactory.cpp
│   ├── ThreadPool.cpp              # Thread pool implementation
│   ├── FileTelemetrySourceImpl.cpp
│   ├── SocketTelemetrySourceImpl.cpp
│   ├── SystemTelemetryWriter.cpp
│   ├── SafeFile.cpp
│   └── SafeSocket.cpp
├── CMakeLists.txt                  # Build configuration
├── README.md                       # This file
└── class_diagram.puml              # PlantUML architecture diagram
```

---

## 🔧 Building

### CMake Build
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Manual Build (g++)
```bash
g++ -std=c++17 -pthread -o app \
    app/main.cpp \
    src/*.cpp \
    -I./include
```

### Compiler Requirements
- C++17 or later
- GCC 7+ / Clang 5+ / MSVC 2017+
- POSIX threads (`-pthread`)

---

## 🔮 Future Work

### Potential Enhancements

1. **Network Sinks**
   - TCP/UDP remote logging
   - HTTP POST to logging services
   - Kafka/RabbitMQ integration

2. **Advanced Filtering**
   - Severity-based filtering per sink
   - Regex pattern matching
   - Rate limiting

3. **Performance Monitoring**
   - Built-in metrics (messages/sec, buffer usage)
   - Prometheus exporter
   - Grafana dashboard

4. **Configuration File**
   - JSON/YAML configuration
   - Runtime sink addition/removal
   - Hot-reload without restart

5. **Compression & Rotation**
   - Log file rotation (by size/time)
   - Gzip compression
   - Retention policies

6. **Testing**
   - Unit tests with Google Test
   - Integration tests
   - Stress testing framework

---

## 📚 Key Learnings

### C++ Concepts Mastered
- ✅ Multi-threading with `std::thread`
- ✅ Synchronization primitives (`mutex`, `condition_variable`)
- ✅ Lock-free programming with `std::atomic`
- ✅ Template metaprogramming
- ✅ Policy-based design
- ✅ Move semantics and perfect forwarding
- ✅ RAII and smart pointers
- ✅ Design patterns (Factory, Builder, Producer-Consumer)

### System Programming
- ✅ Unix domain sockets
- ✅ `/proc` filesystem parsing
- ✅ File descriptors and system calls
- ✅ Thread lifecycle management
- ✅ Graceful shutdown strategies

---





<div align="center">

### ⭐ Star this repo if you found it helpful!

**Built with ❤️ and lots of ☕**

</div>