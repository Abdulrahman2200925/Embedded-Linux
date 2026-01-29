#ifndef ThreadPool_HPP
#define ThreadPool_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <stdexcept>

class ThreadPool {
private:
    // Worker threads
    std::vector<std::thread> workers;
    
    // Task queue
    std::queue<std::function<void()>> tasks;
    
    // Synchronization
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
    
    // Worker thread function
    void workerLoop();
    
public:
    // Constructor - creates pool with specified number of threads
    explicit ThreadPool(size_t numThreads);
    
    // Destructor - stops all threads and waits for completion
    ~ThreadPool();
    
    // Delete copy and move (threads are not copyable/movable easily)
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    
    // Enqueue a task (callable) to be executed by worker threads
    template<typename F>
    void enqueue(F&& task);
    
    // Get number of worker threads
    size_t size() const;
};

// ============================================================================
// TEMPLATE IMPLEMENTATION (must be in header)
// ============================================================================

template<typename F>
void ThreadPool::enqueue(F&& task) {
    // Check if pool is stopped
    if (stopFlag.load()) {
        throw std::runtime_error("ThreadPool: Cannot enqueue on stopped pool");
    }
    
    // Add task to queue
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.emplace(std::forward<F>(task));
    }
    
    // Notify one waiting worker
    condition.notify_one();
}

#endif