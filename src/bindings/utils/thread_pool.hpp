#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <iostream>
#include <functional>
#include "common/thread_safe_queue.h"
#include "common/common_objs.h"

#include "common/utils.h"

class ThreadPool
{

    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while (!done)
        {
            function_wrapper task;
            if (work_queue.try_pop(task))
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
        work_queue.push(std::move(task));
        return res;
    }
};