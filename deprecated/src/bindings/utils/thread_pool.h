#pragma once

#include <thread>
#include <future>
#include <atomic>
#include <vector>
#include <iostream>
#include <functional>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

class ThreadGuard
{
public:
    explicit ThreadGuard(std::thread &_t) : t(_t)
    {
    }
    ~ThreadGuard()
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    ThreadGuard(ThreadGuard &) = delete;
    ThreadGuard &operator=(ThreadGuard &) = delete;

private:
    std::thread &t;
};

class JoinThreads
{
public:
    explicit JoinThreads(std::vector<std::thread> &_threads) : threads(_threads)
    {
    }
    ~JoinThreads()
    {
        for (size_t i = 0; i < threads.size(); i++)
        {
            if (threads[i].joinable())
                threads[i].join();
        }
    }

private:
    std::vector<std::thread> &threads;
};

class FunctionWrapper
{
    struct impl_base
    {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };

    template <typename F>
    struct impl_type : impl_base
    {
        F f;
        impl_type(F &&f_) : f(std::move(f_)) {}
        void call() { f(); }
    };

    std::unique_ptr<impl_base> impl;

public:
    template <typename F>
    FunctionWrapper(F &&f) : impl(new impl_type<F>(std::move(f)))
    {
    }

    void operator()() { impl->call(); }

    FunctionWrapper()
    {
    }

    FunctionWrapper(FunctionWrapper &&other) : impl(std::move(other.impl))
    {
    }

    FunctionWrapper &operator=(FunctionWrapper &&other)
    {
        impl = std::move(other.impl);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper &) = delete;
    FunctionWrapper(FunctionWrapper &) = delete;
};

template <typename T>
class ThreadsafeQueue
{
private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable data_cond;

public:
    ThreadsafeQueue()
    {
    }

    void WaitAndPop(T &value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]
                       { return !data_queue.empty(); });
        value = std::move(*data_queue.front());
        data_queue.pop();
    }

    bool TryPop(T &value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        value = std::move(*data_queue.front());
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> WaitAndPop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]
                       { return !data_queue.empty(); });
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    std::shared_ptr<T> TryPop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }

    void Push(T new_value)
    {
        std::shared_ptr<T> data(
            std::make_shared<T>(std::move(new_value)));
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
};

class ThreadPool
{

    std::atomic_bool done;
    ThreadsafeQueue<FunctionWrapper> work_queue;
    std::vector<std::thread> threads;
    JoinThreads joiner;

    void worker_thread()
    {
        while (!done)
        {
            FunctionWrapper task;
            if (work_queue.TryPop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    ThreadPool(size_t thread_count) : done(false), joiner(threads)
    {
        try
        {
            for (size_t i = 0; i < thread_count; ++i)
            {
                threads.push_back(
                    std::thread(&ThreadPool::worker_thread, this));
            }
        }
        catch (...)
        {
            done = true;
            throw;
        }
    }

    ~ThreadPool()
    {
        done = true;
    }

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.Push(std::move(task));
        return res;
    }
};