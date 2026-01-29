#ifndef RingBuffer_HPP
#define RingBuffer_HPP

#include <vector>
#include <optional>
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<typename T>
class RingBuffer {
private:
    std::vector<std::optional<T>> buffer;  // Storage
    size_t head;        // Index where we read (pop)
    size_t tail;        // Index where we write (push)
    size_t count;       // Number of items currently in buffer
    size_t capacity;    // Maximum capacity
    
    // Thread safety primitives
    mutable std::mutex bufferMutex;           // Protects all shared data
    std::condition_variable notEmpty;          // Signals when data available
    std::condition_variable notFull;           // Signals when space available
    std::atomic<bool> shutdownFlag;            // Signals shutdown
    
public:
    // Constructor - creates buffer with given capacity
    explicit RingBuffer(size_t capacity);
    
    // Destructor 
    ~RingBuffer() = default;
    
    // Delete copy operations (copy not supported)
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    
    // Move operations (move supported)
    RingBuffer(RingBuffer&&) noexcept = default;
    RingBuffer& operator=(RingBuffer&&) noexcept = default;
    
    // Non-blocking operations (original - now thread-safe)
    bool tryPush(T&& item);
    bool tryPop(T& item);
    
    // Blocking operations (NEW - for producer-consumer pattern)
    void push(T&& item);           // Blocks if full
    bool pop(T& item);              // Blocks if empty, returns false on shutdown
    
    // Shutdown
    void shutdown();                // Signal all waiting threads to stop
    
    // Utility methods
    bool isFull() const;
    bool isEmpty() const;
    size_t size() const;
    size_t getCapacity() const;
};

// ============================================================================
// IMPLEMENTATION (Templates must be in header!)
// ============================================================================

template<typename T>
RingBuffer<T>::RingBuffer(size_t cap)
    : buffer(cap)      // Create vector with 'cap' empty optionals
    , head(0)
    , tail(0)
    , count(0)
    , capacity(cap)
    , shutdownFlag(false)
{
}

// NON-BLOCKING: Try to push (returns false if full)
template<typename T>
bool RingBuffer<T>::tryPush(T&& item) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    
    // Check if buffer is full
    if (count >= capacity) {
        return false;  // Cannot push, buffer full
    }
    
    // Place item at tail position
    buffer[tail] = std::move(item);
    
    // Advance tail with wraparound
    tail = (tail + 1) % capacity;
    
    // Increment count
    count++;
    
    // Notify one waiting consumer
    notEmpty.notify_one();
    
    return true;  // Success
}

// NON-BLOCKING: Try to pop (returns false if empty)
template<typename T>
bool RingBuffer<T>::tryPop(T& item) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    
    // Check if buffer is empty
    if (count == 0) {
        return false;  // Cannot pop, buffer empty
    }
    
    // Check if slot actually has value (safety check)
    if (!buffer[head].has_value()) {
        return false;  // Shouldn't happen, but be safe
    }
    
    // Move item out of buffer
    item = std::move(buffer[head].value());
    
    // Clear the slot
    buffer[head].reset();
    
    // Advance head with wraparound
    head = (head + 1) % capacity;
    
    // Decrement count
    count--;
    
    // Notify one waiting producer
    notFull.notify_one();
    
    return true;  // Success
}

// BLOCKING: Push (waits if full)
template<typename T>
void RingBuffer<T>::push(T&& item) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    
    // Wait until buffer is not full (or shutdown)
    notFull.wait(lock, [this]() { 
        return count < capacity || shutdownFlag.load(); 
    });
    
    // If shutdown, don't push
    if (shutdownFlag.load()) {
        return;
    }
    
    // Place item at tail position
    buffer[tail] = std::move(item);
    
    // Advance tail with wraparound
    tail = (tail + 1) % capacity;
    
    // Increment count
    count++;
    
    // Notify one waiting consumer
    notEmpty.notify_one();
}

// BLOCKING: Pop (waits if empty, returns false on shutdown)
template<typename T>
bool RingBuffer<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(bufferMutex);
    
    // Wait until buffer has data (or shutdown)
    notEmpty.wait(lock, [this]() { 
        return count > 0 || shutdownFlag.load(); 
    });
    
    // If shutdown and buffer empty, return false
    if (shutdownFlag.load() && count == 0) {
        return false;
    }
    
    // Check if slot actually has value (safety check)
    if (!buffer[head].has_value()) {
        return false;
    }
    
    // Move item out of buffer
    item = std::move(buffer[head].value());
    
    // Clear the slot
    buffer[head].reset();
    
    // Advance head with wraparound
    head = (head + 1) % capacity;
    
    // Decrement count
    count--;
    
    // Notify one waiting producer
    notFull.notify_one();
    
    return true;  // Success
}

// Signal shutdown to all waiting threads
template<typename T>
void RingBuffer<T>::shutdown() {
    shutdownFlag.store(true);
    notEmpty.notify_all();   // Wake all consumers
    notFull.notify_all();    // Wake all producers
}

template<typename T>
bool RingBuffer<T>::isFull() const {
    std::lock_guard<std::mutex> lock(bufferMutex);
    return count == capacity;
}

template<typename T>
bool RingBuffer<T>::isEmpty() const {
    std::lock_guard<std::mutex> lock(bufferMutex);
    return count == 0;
}

template<typename T>
size_t RingBuffer<T>::size() const {
    std::lock_guard<std::mutex> lock(bufferMutex);
    return count;
}

template<typename T>
size_t RingBuffer<T>::getCapacity() const {
    // Capacity is const, no lock needed
    return capacity;
}

#endif