//
// Created by hhhxiao on 4/22/20.
//

#ifndef ANET_THREADPOOL_H
#define ANET_THREADPOOL_H

#include <cstddef>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <future>
#include <type_traits>


class ThreadPool {
public:


    explicit ThreadPool(size_t num);

    ThreadPool();

    template<typename Function, typename ...Args>
    std::future<typename std::result_of<Function(Args...)>::type>
    enqueue(Function &&f, Args &&...args);

    ~ThreadPool();

private:
    std::vector<std::thread> work_threads;
    std::queue<std::function<void()>> task_queue;

    std::mutex queue_mutex;
    std::condition_variable cond_val;
    bool stop;
};


ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

ThreadPool::ThreadPool(size_t num)
        : stop(false) {
    for (size_t i = 0; i < num; ++i) {
        work_threads.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->cond_val.wait(lock, [this] {
                        return this->stop || !this->task_queue.empty();
                    });
                    if (this->stop && this->task_queue.empty())return;
                    task = std::move(this->task_queue.front());
                    this->task_queue.pop();
                }
                task();

            }
        });
    }
}

template<typename Function, typename... Args>

std::future<typename std::result_of<Function(Args...)>::type>
ThreadPool::enqueue(Function &&f, Args &&... args) {
    using return_type = typename std::result_of<Function(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<Function>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped thread pool");
        task_queue.emplace([task]() { (*task)(); });
    }
    cond_val.notify_all();
    return res;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cond_val.notify_all();
    for (auto &t:work_threads)
        t.join();

}

#endif //ANET_THREADPOOL_H
