#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads)
    : stopFlag(false)
{
    // Validate input
    if (numThreads == 0) {
        throw std::invalid_argument("ThreadPool: numThreads must be > 0");
    }
    
    // Create worker threads
    workers.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerLoop, this);
    }
    
    std::cout << "ThreadPool: Created with " << numThreads << " worker threads\n";
}

ThreadPool::~ThreadPool() {
    // Signal all threads to stop
    stopFlag.store(true);
    
    // Wake up all waiting threads
    condition.notify_all();
    
    // Wait for all threads to finish
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    std::cout << "ThreadPool: All workers stopped\n";
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        
        // Wait for a task or stop signal
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            
            // Wait until there's a task or we're stopping
            condition.wait(lock, [this]() {
                return stopFlag.load() || !tasks.empty();
            });
            
            // If stopping and no tasks left, exit
            if (stopFlag.load() && tasks.empty()) {
                return;
            }
            
            // If stopping but tasks remain, process them
            if (tasks.empty()) {
                continue;  // Spurious wakeup
            }
            
            // Get task from queue
            task = std::move(tasks.front());
            tasks.pop();
        }
        // Lock released here
        
        // Execute task outside the lock
        try {
            task();
        } catch (const std::exception& e) {
            std::cerr << "ThreadPool: Task threw exception: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "ThreadPool: Task threw unknown exception\n";
        }
    }
}

size_t ThreadPool::size() const {
    return workers.size();
}