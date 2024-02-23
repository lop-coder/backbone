#pragma once
#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "extension.h"
class BACKBONE_PLUGIN_API ThreadPool {
 public:
  ThreadPool(){};
  virtual ~ThreadPool(){};

  template <typename Func, typename... Ts>
  auto addTask(Func&& func, Ts&&... params)
      -> std::future<typename std::result_of<Func(Ts...)>::type> {
    auto execute =
        std::bind(std::forward<Func>(func), std::forward<Ts>(params)...);
    using ReturnType = typename std::result_of<Func(Ts...)>::type;
    using PackagedTask = std::packaged_task<ReturnType()>;

    auto task = std::make_shared<PackagedTask>(std::move(execute));
    auto result = task->get_future();
    pushTask([task]() { (*task)(); });
    return result;
  }
  virtual bool start(size_t minThreads = 1, size_t maxThreads = 1) = 0;
  virtual bool stop() = 0;
  virtual void pushTask(std::function<void()> task) = 0;
  virtual size_t threadNum() = 0;
  virtual size_t taskQueueSize() = 0;
};

#define THREADPOOL_IDENTITY "ThreadPool"
#define THREADPOOL_VERSION "v1.0.0"
#define THREADPOOL_DESCRIPTION "ThreadPool"
