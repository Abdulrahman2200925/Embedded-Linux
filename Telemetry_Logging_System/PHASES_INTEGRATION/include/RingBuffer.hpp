#ifndef RingBuffer_HPP
#define RingBuffer_HPP

#include <vector>
#include <optional>
#include <cstddef>  // for size_t

template<typename T>
class RingBuffer {
private:
    std::vector<std::optional<T>> buffer;  // Storage
    size_t head;        // Index where we read (pop)
    size_t tail;        // Index where we write (push)
    size_t count;       // Number of items currently in buffer
    size_t capacity;    // Maximum capacity
    
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
    
    // Try to push an item (returns false if full)
    bool tryPush(T&& item);
    
    // Try to pop an item (returns false if empty)
    bool tryPop(T& item);
    
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
{
}

template<typename T>
bool RingBuffer<T>::tryPush(T&& item) {
    // Check if buffer is full
    if (isFull()) {
        return false;  // Cannot push, buffer full
    }
    
    // Place item at tail position
    buffer[tail] = std::move(item);  // Move item into optional
    
    // Advance tail with wraparound
    tail = (tail + 1) % capacity;
    
    // Increment count
    count++;
    
    return true;  // Success
}

template<typename T>
bool RingBuffer<T>::tryPop(T& item) {
    // Check if buffer is empty
    if (isEmpty()) {
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
    
    return true;  // Success
}

template<typename T>
bool RingBuffer<T>::isFull() const {
    return count == capacity;
}

template<typename T>
bool RingBuffer<T>::isEmpty() const {
    return count == 0;
}

template<typename T>
size_t RingBuffer<T>::size() const {
    return count;
}

template<typename T>
size_t RingBuffer<T>::getCapacity() const {
    return capacity;
}

#endif