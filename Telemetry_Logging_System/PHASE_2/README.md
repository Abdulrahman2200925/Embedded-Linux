# Phase 2: Data Sources & Smart Resource Management

A robust telemetry system implementing RAII (Resource Acquisition Is Initialization) patterns for safe resource management with support for multiple data source types.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Classes](#classes)
- [Design Patterns](#design-patterns)
- [Build Instructions](#build-instructions)
- [Usage Examples](#usage-examples)
- [Testing](#testing)
- [C++ Concepts Covered](#c-concepts-covered)
- [Dependencies](#dependencies)

---

## 🎯 Overview

Phase 2 extends the logging system from Phase 1 by adding telemetry data sources with proper resource management. The implementation demonstrates advanced C++ concepts including RAII, move semantics, and the Rule of 3/5/0.

### Key Objectives

1. Create actual data sources that continuously receive data from multiple source types
2. Implement RAII-compliant resource wrappers (SafeFile & SafeSocket)
3. Parse real system data from `/proc/stat` and `/proc/meminfo`
4. Demonstrate proper resource ownership and transfer semantics

---

## ✨ Features

- **RAII Resource Management**: Automatic resource cleanup with no memory leaks
- **Move-Only Semantics**: Safe transfer of resource ownership
- **Multiple Data Sources**: File-based and socket-based telemetry
- **System Monitoring**: Real CPU and memory usage parsing
- **Type-Safe Interfaces**: Abstract interfaces for extensibility
- **Rule of Zero Compliance**: Minimal manual resource management

---

## 📁 Project Structure
```
PHASE_2/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── ITelemetrySource.hpp              # Telemetry source interface
│   ├── SafeFile.hpp                       # RAII file descriptor wrapper
│   ├── SafeSocket.hpp                     # RAII socket descriptor wrapper
│   ├── FileTelemetrySourceImpl.hpp        # File-based telemetry source
│   ├── SocketTelemetrySourceImpl.hpp      # Socket-based telemetry source
│   └── SystemTelemetryWriter.hpp          # System metrics parser
├── src/
│   ├── SafeFile.cpp
│   ├── SafeSocket.cpp
│   ├── FileTelemetrySourceImpl.cpp
│   ├── SocketTelemetrySourceImpl.cpp
│   └── SystemTelemetryWriter.cpp
├── app/
│   └── main.cpp
├── docs/
│   └── phase2_class_diagram.puml
└── tests/
    ├── test_safefile.cpp
    ├── test_safesocket.cpp
    └── test_telemetry.cpp
```

---

## 🏗️ Classes

### Core Interface

#### `ITelemetrySource`
Abstract interface for telemetry data sources.
```cpp
class ITelemetrySource {
public:
    virtual bool openSource() = 0;
    virtual bool readSource(std::string& out) = 0;
    virtual ~ITelemetrySource() = default;
};
```

**Methods:**
- `openSource()`: Establish connection/open resource, returns success status
- `readSource(out)`: Read latest data into output parameter, returns success status

---

### RAII Classes (Rule of 5)

#### `SafeFile`
RAII wrapper for POSIX file descriptors.

**Key Features:**
- Opens file in constructor
- Closes file in destructor
- Move-only (copy operations deleted)
- Wraps `open()`, `read()`, `write()`, `close()`

**Special Members:**
```cpp
SafeFile(const std::string& path, int flags);     // Constructor
~SafeFile();                                       // Destructor
SafeFile(const SafeFile&) = delete;               // Copy constructor (deleted)
SafeFile& operator=(const SafeFile&) = delete;    // Copy assignment (deleted)
SafeFile(SafeFile&&) noexcept;                    // Move constructor
SafeFile& operator=(SafeFile&&) noexcept;         // Move assignment
```

**Usage:**
```cpp
SafeFile file("/tmp/data.txt", O_RDONLY);
if (file.isValid()) {
    char buffer[1024];
    ssize_t bytesRead = file.read(buffer, sizeof(buffer));
}
// File automatically closed when file goes out of scope
```

---

#### `SafeSocket`
RAII wrapper for Unix Domain Socket descriptors.

**Key Features:**
- Creates socket in constructor
- Closes socket in destructor
- Move-only (copy operations deleted)
- Supports both server and client modes
- Automatic socket file cleanup for servers

**Server Usage:**
```cpp
SafeSocket server;
server.createSocket();
server.bind("/tmp/my.sock");
server.listen();
SafeSocket client = server.accept();  // Move semantics
```

**Client Usage:**
```cpp
SafeSocket client;
client.createSocket();
client.connect("/tmp/my.sock");
client.write(data, size);
```

---

### Rule of Zero Classes

#### `FileTelemetrySourceImpl`
File-based telemetry source implementation.

**Key Features:**
- Implements `ITelemetrySource` interface
- Uses `SafeFile` for resource management
- No custom destructor/copy/move needed (Rule of Zero)
- Reads from files written by external processes

**Usage:**
```cpp
FileTelemetrySourceImpl telemetry("/tmp/sensor_data.txt");

if (telemetry.openSource()) {
    std::string data;
    while (telemetry.readSource(data)) {
        // Process telemetry data
        std::cout << "Data: " << data << std::endl;
    }
}
```

---

#### `SocketTelemetrySourceImpl`
Socket-based telemetry source implementation.

**Key Features:**
- Implements `ITelemetrySource` interface
- Uses `SafeSocket` for resource management
- No custom destructor/copy/move needed (Rule of Zero)
- Receives data over Unix Domain Sockets

**Usage:**
```cpp
SocketTelemetrySourceImpl telemetry("/tmp/telemetry.sock");

if (telemetry.openSource()) {
    std::string data;
    if (telemetry.readSource(data)) {
        // Process received data
    }
}
```

---

### Bonus: System Telemetry

#### `SystemTelemetryWriter`
Parses `/proc/stat` and `/proc/meminfo` for real system metrics.

**Key Features:**
- Parses CPU usage from `/proc/stat`
- Parses memory usage from `/proc/meminfo`
- Writes parsed data to file using `SafeFile`
- Provides meaningful, formatted telemetry

**Usage:**
```cpp
SystemTelemetryWriter writer("/tmp/system_metrics.txt");

if (writer.isOpen()) {
    writer.writeCpuTelemetry();      // Parses /proc/stat
    writer.writeMemoryTelemetry();   // Parses /proc/meminfo
}
```

**Output Format:**
```
CPU_USAGE:15.34%
MEMORY_USAGE:Total=15628.52MB,Used=8432.12MB(53.94%)
```

---

## 🎨 Design Patterns

### RAII (Resource Acquisition Is Initialization)

**Pattern:** Resource lifetime is tied to object lifetime.

**Implementation:**
- Constructor acquires resource (opens file/socket)
- Destructor releases resource (closes file/socket)
- No manual cleanup required

**Example:**
```cpp
{
    SafeFile file("data.txt", O_RDONLY);  // Opens file
    // Use file...
}  // Destructor automatically closes file
```

---

### Strategy Pattern

**Pattern:** Define a family of algorithms (telemetry sources), encapsulate each one, make them interchangeable.

**Implementation:**
- `ITelemetrySource`: Strategy interface
- `FileTelemetrySourceImpl`: Concrete strategy (file-based)
- `SocketTelemetrySourceImpl`: Concrete strategy (socket-based)

**Benefit:** Can switch between data sources without changing client code.

---

### Rule of 3/5/0

#### Rule of 5 (`SafeFile`, `SafeSocket`)
When managing resources manually, implement all 5 special member functions:
1. Destructor
2. Copy constructor (deleted)
3. Copy assignment (deleted)
4. Move constructor
5. Move assignment

#### Rule of 0 (`FileTelemetrySourceImpl`, `SocketTelemetrySourceImpl`)
When using only RAII types, default all special member functions:
- Let compiler generate everything
- No manual resource management

---

## 🔨 Build Instructions

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# Minimum versions
cmake --version  # >= 3.16
g++ --version    # >= 7.0 (C++17 support)
```

### Build Steps
```bash
# Clone repository
git clone <repository-url>
cd PHASE_2

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./app/app
```

---

## 💡 Usage Examples

### Example 1: File-Based Telemetry
```cpp
#include "FileTelemetrySourceImpl.hpp"
#include "LogManager.hpp"
#include "LogMessage.hpp"

int main() {
    // Create logging system (from Phase 1)
    LogManager logger;
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    
    // Create file telemetry source
    FileTelemetrySourceImpl telemetry("/tmp/sensor_data.txt");
    
    if (!telemetry.openSource()) {
        std::cerr << "Failed to open telemetry source" << std::endl;
        return 1;
    }
    
    // Read and log telemetry data
    std::string data;
    while (telemetry.readSource(data)) {
        logger.log(LogMessage("SensorApp", "Telemetry", 
                             Severity::INFO, data));
    }
    
    logger.flush();
    return 0;
}
```

---

### Example 2: Socket-Based Telemetry (Server)
```cpp
#include "SocketTelemetrySourceImpl.hpp"
#include "LogManager.hpp"

int main() {
    LogManager logger;
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    
    // Create socket server
    SocketTelemetrySourceImpl telemetry("/tmp/telemetry.sock");
    
    if (!telemetry.openSource()) {
        std::cerr << "Failed to start socket server" << std::endl;
        return 1;
    }
    
    std::cout << "Socket server listening..." << std::endl;
    
    // Receive data from client
    std::string data;
    while (telemetry.readSource(data)) {
        logger.log(LogMessage("SocketApp", "Network", 
                             Severity::INFO, data));
        logger.flush();
    }
    
    return 0;
}
```

---

### Example 3: System Monitoring with Real Data
```cpp
#include "SystemTelemetryWriter.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "LogManager.hpp"
#include <thread>
#include <chrono>

int main() {
    LogManager logger;
    logger.addSink(std::make_shared<FileSinkImpl>("system_monitor.log"));
    
    const std::string telemetryFile = "/tmp/system_telemetry.dat";
    
    // Continuous monitoring loop
    for (int i = 0; i < 60; i++) {  // Monitor for 1 minute
        // Parse /proc and write to file
        SystemTelemetryWriter writer(telemetryFile);
        writer.writeCpuTelemetry();
        writer.writeMemoryTelemetry();
        
        // Read the telemetry file
        FileTelemetrySourceImpl source(telemetryFile);
        if (source.openSource()) {
            std::string data;
            while (source.readSource(data)) {
                logger.log(LogMessage("SystemMonitor", "Metrics", 
                                     Severity::INFO, data));
            }
        }
        
        logger.flush();
        
        // Wait 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
```

---

### Example 4: Integration with Shell Script

**Shell Script (generates telemetry data):**
```bash
#!/bin/bash
# telemetry_generator.sh

LOG_FILE="/tmp/shell_logs"
DURATION=60
INTERVAL=1

: > "$LOG_FILE"

START_TIME=$(date +%s)

while (( $(date +%s) - START_TIME < DURATION )); do
    VALUE=$(awk -v r=$RANDOM 'BEGIN { printf "%.2f", r/32767*100 }')
    echo "$VALUE" >> "$LOG_FILE"
    sleep "$INTERVAL"
done
```

**C++ Application:**
```cpp
int main() {
    std::cout << "Start shell script: ./telemetry_generator.sh &" << std::endl;
    std::cout << "Press Enter when ready..." << std::endl;
    std::cin.get();
    
    LogManager logger;
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    
    FileTelemetrySourceImpl telemetry("/tmp/shell_logs");
    
    if (!telemetry.openSource()) {
        std::cerr << "Failed to open telemetry source" << std::endl;
        return 1;
    }
    
    std::string value;
    int count = 0;
    
    while (count < 100 && telemetry.readSource(value)) {
        count++;
        logger.log(LogMessage("ShellMonitor", "Sensor", 
                             Severity::INFO, 
                             "Sample #" + std::to_string(count) + ": " + value));
        
        if (count % 10 == 0) {
            logger.flush();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    logger.flush();
    return 0;
}
```

---

## 🧪 Testing

### Test SafeFile
```cpp
// test_safefile.cpp
#include "SafeFile.hpp"
#include <iostream>
#include <cstring>

int main() {
    // Test 1: Write
    {
        SafeFile file("test.txt", O_WRONLY | O_CREAT | O_TRUNC);
        const char* data = "Hello RAII!\n";
        file.write(data, strlen(data));
    }  // File automatically closed
    
    // Test 2: Read
    {
        SafeFile file("test.txt", O_RDONLY);
        char buffer[100];
        ssize_t bytesRead = file.read(buffer, sizeof(buffer));
        buffer[bytesRead] = '\0';
        std::cout << "Read: " << buffer;
    }  // File automatically closed
    
    // Test 3: Move semantics
    SafeFile file1("test.txt", O_RDONLY);
    std::cout << "file1 valid: " << file1.isValid() << std::endl;
    
    SafeFile file2 = std::move(file1);
    std::cout << "After move:" << std::endl;
    std::cout << "  file1 valid: " << file1.isValid() << std::endl;  // 0
    std::cout << "  file2 valid: " << file2.isValid() << std::endl;  // 1
    
    return 0;
}
```

**Compile and run:**
```bash
g++ -std=c++17 test_safefile.cpp SafeFile.cpp -o test_safefile
./test_safefile
```

---

### Test SafeSocket
```cpp
// test_safesocket.cpp
#include "SafeSocket.hpp"
#include <iostream>
#include <thread>
#include <cstring>

void server() {
    SafeSocket serverSocket;
    serverSocket.createSocket();
    serverSocket.bind("/tmp/test.sock");
    serverSocket.listen();
    
    std::cout << "[Server] Waiting for connection..." << std::endl;
    SafeSocket client = serverSocket.accept();
    
    char buffer[1024];
    ssize_t bytesRead = client.read(buffer, sizeof(buffer));
    buffer[bytesRead] = '\0';
    
    std::cout << "[Server] Received: " << buffer << std::endl;
}

void client() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    SafeSocket clientSocket;
    clientSocket.createSocket();
    clientSocket.connect("/tmp/test.sock");
    
    const char* msg = "Hello from client!";
    clientSocket.write(msg, strlen(msg));
    
    std::cout << "[Client] Sent message" << std::endl;
}

int main() {
    std::thread serverThread(server);
    std::thread clientThread(client);
    
    serverThread.join();
    clientThread.join();
    
    return 0;
}
```

**Compile and run:**
```bash
g++ -std=c++17 -pthread test_safesocket.cpp SafeSocket.cpp -o test_safesocket
./test_safesocket
```

---

## 📚 C++ Concepts Covered

### Advanced Topics

| Concept | Implementation | Classes |
|---------|---------------|---------|
| **RAII** | Resource tied to object lifetime | SafeFile, SafeSocket |
| **Move Semantics** | Transfer ownership without copying | SafeFile, SafeSocket |
| **Rule of 5** | Manage all special members | SafeFile, SafeSocket |
| **Rule of 0** | Default all special members | FileTelemetrySourceImpl, SocketTelemetrySourceImpl |
| **deleted functions** | `= delete` prevents copying | SafeFile, SafeSocket |
| **noexcept** | Guarantee no exceptions in moves | Move constructors/assignments |
| **References** | Output parameters `string&` | readSource() |
| **Abstract Interfaces** | Pure virtual functions | ITelemetrySource |

### Special Member Functions
```cpp
// Rule of 5 (SafeFile, SafeSocket)
~SafeFile();                                    // Destructor
SafeFile(const SafeFile&) = delete;            // Copy constructor (deleted)
SafeFile& operator=(const SafeFile&) = delete; // Copy assignment (deleted)
SafeFile(SafeFile&&) noexcept;                 // Move constructor
SafeFile& operator=(SafeFile&&) noexcept;      // Move assignment

// Rule of 0 (FileTelemetrySourceImpl)
~FileTelemetrySourceImpl() = default;                                    // Destructor
FileTelemetrySourceImpl(const FileTelemetrySourceImpl&) = default;      // Copy constructor
FileTelemetrySourceImpl& operator=(const FileTelemetrySourceImpl&) = default; // Copy assignment
FileTelemetrySourceImpl(FileTelemetrySourceImpl&&) = default;           // Move constructor
FileTelemetrySourceImpl& operator=(FileTelemetrySourceImpl&&) = default; // Move assignment
```

### POSIX APIs

| API | Usage | Header |
|-----|-------|--------|
| `open()` | Open file descriptor | `<fcntl.h>` |
| `read()` | Read from descriptor | `<unistd.h>` |
| `write()` | Write to descriptor | `<unistd.h>` |
| `close()` | Close descriptor | `<unistd.h>` |
| `socket()` | Create socket | `<sys/socket.h>` |
| `bind()` | Bind socket to address | `<sys/socket.h>` |
| `listen()` | Listen for connections | `<sys/socket.h>` |
| `accept()` | Accept connection | `<sys/socket.h>` |
| `connect()` | Connect to server | `<sys/socket.h>` |

---

## 📦 Dependencies

### Required

- **C++17 or later**
- **CMake 3.16+**
- **POSIX-compliant OS** (Linux, macOS, *BSD)
- **Standard C++ Library**

### System Files (for bonus features)

- `/proc/stat` - CPU statistics (Linux)
- `/proc/meminfo` - Memory information (Linux)

### Phase 1 Dependencies

Phase 2 integrates with Phase 1 components:
- `LogManager`
- `LogMessage`
- `ILogSink` interface
- Sink implementations (ConsoleSinkImpl, FileSinkImpl)

---

## 📊 Performance Considerations

### Resource Management

- **Zero-copy moves**: Move semantics avoid unnecessary data copying
- **Automatic cleanup**: RAII ensures resources freed even on exceptions
- **No memory leaks**: All resources managed by object lifetime

### File I/O

- Direct POSIX calls (lower overhead than C++ streams)
- Single system call per read/write (no buffering layer)
- Efficient for small, frequent reads

### Socket I/O

- Unix Domain Sockets (faster than TCP for local IPC)
- Blocking I/O (simple, suitable for single-threaded telemetry)

---

## 🐛 Common Issues

### Issue 1: Permission Denied

**Error:**
```
Failed to open /proc/stat: Permission denied
```

**Solution:** Run with appropriate permissions or check file paths.

---

### Issue 2: Socket Already in Use

**Error:**
```
bind: Address already in use
```

**Solution:**
```bash
# Remove old socket file
rm /tmp/telemetry.sock

# Or use a different path
SocketTelemetrySourceImpl telemetry("/tmp/my_unique.sock");
```

---

### Issue 3: File Descriptor Leak

**Symptom:** "Too many open files" error

**Solution:** Ensure RAII classes are working correctly. Check that:
- Destructors are being called
- Move semantics properly invalidate source objects
- No circular references preventing destruction

---

## 🎓 Learning Outcomes

After completing Phase 2, you will understand:

✅ **RAII Pattern** - Automatic resource management  
✅ **Move Semantics** - Efficient resource transfer  
✅ **Rule of 3/5/0** - When and how to manage special members  
✅ **Value Categories** - Lvalues vs Rvalues  
✅ **Resource Ownership** - Clear ownership semantics  
✅ **POSIX APIs** - Low-level system programming  
✅ **Unix Domain Sockets** - Inter-process communication  
✅ **System Monitoring** - Parsing /proc filesystem  
✅ **Design Patterns** - Strategy pattern, Interface segregation  

---

## 📖 Additional Resources

### C++ Resources

- [C++ Core Guidelines - Resource Management](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r-resource-management)
- [cppreference - Rule of three/five/zero](https://en.cppreference.com/w/cpp/language/rule_of_three)
- [Move Semantics Explained](https://www.youtube.com/watch?v=St0MNEU5b0o)

### POSIX Resources

- [Linux Programmer's Manual](https://man7.org/linux/man-pages/)
- [Unix Socket Programming](https://beej.us/guide/bgipc/html/single/bgipc.html)
- [/proc filesystem documentation](https://www.kernel.org/doc/Documentation/filesystems/proc.txt)

---

## 🤝 Integration with Phase 1

Phase 2 seamlessly integrates with Phase 1:
```cpp
// Phase 1: Logging System
LogManager logger;
logger.addSink(std::make_shared<ConsoleSinkImpl>());
logger.addSink(std::make_shared<FileSinkImpl>("application.log"));

// Phase 2: Telemetry Source
FileTelemetrySourceImpl telemetry("/tmp/sensor_data.txt");

// Integration: Read telemetry and log it
if (telemetry.openSource()) {
    std::string data;
    while (telemetry.readSource(data)) {
        // Create LogMessage from telemetry data
        logger.log(LogMessage("TelemetryApp", "Sensor", 
                             Severity::INFO, data));
    }
    logger.flush();
}
```

---



**Phase 2 Complete!** ✅ RAII, Move Semantics, and System Integration mastered! 🎉
