#include "extension-threadpool.h"
#include <iostream>
class ThreadPoolImpl : public ThreadPool {
 public:
  ThreadPoolImpl();
  ~ThreadPoolImpl();
  virtual bool start(size_t minThreads, size_t maxThreads) final override;
  virtual bool stop() final override;
  virtual void pushTask(std::function<void()> task) final override;
  virtual size_t threadNum() final override;
  virtual size_t taskQueueSize() final override;

 private:
  void worker();
  void joinFinishedThreads();

  std::atomic<bool> _isRun;
  std::atomic<bool> _quit;
  std::atomic<size_t> _currentNum;
  std::atomic<size_t> _idleCount;
  std::atomic<size_t> _maxThreads;
  std::atomic<size_t> _minhreads;

  mutable std::mutex _mutex;
  std::condition_variable _conditionv;
  std::queue<std::function<void()>> _tasks;
  std::queue<std::thread::id> _threadIDs;
  std::unordered_map<std::thread::id, std::thread> _threads;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define WAIT_SECONDS 2

ThreadPoolImpl::ThreadPoolImpl()
    : _quit(false),
      _isRun(false),
      _currentNum(0),
      _idleCount(0),
      _maxThreads(std::thread::hardware_concurrency()),
      _minhreads(1) {
  _threads.clear();
  while (!_threadIDs.empty()) {
    _threadIDs.pop();
  }
  while (!_tasks.empty()) {
    _tasks.pop();
  }
}

ThreadPoolImpl::~ThreadPoolImpl() {
  stop();
}

size_t ThreadPoolImpl::threadNum() {
  std::lock_guard<std::mutex> guard(_mutex);
  return _currentNum;
}
size_t ThreadPoolImpl::taskQueueSize() {
  std::lock_guard<std::mutex> guard(_mutex);
  return _tasks.size();
}
void ThreadPoolImpl::pushTask(std::function<void()> task) {
  std::lock_guard<std::mutex> guard(_mutex);
  assert(!_quit);
  if (_quit) {
    throw std::runtime_error("submit on stopped ThreadPool");
  }
  _tasks.emplace(task);
  if (!_isRun) {
    return;
  }
  if (_idleCount > 0) {
    _conditionv.notify_one();
  } else if (_currentNum < _maxThreads) {
    std::thread t(&ThreadPoolImpl::worker, this);
    assert(_threads.find(t.get_id()) == _threads.end());
    _threads[t.get_id()] = std::move(t);
    ++_currentNum;
  }
}
bool ThreadPoolImpl::start(size_t minThreads, size_t maxThreads) {
  stop();
  if (!_isRun) {
    if (_currentNum == 0) {
      _isRun = true;
      if (_minhreads <= 0) {
        _minhreads = 1;
      }
      if (_maxThreads < _minhreads) {
        _maxThreads.store(_minhreads.load());
      }
      int max = _maxThreads;
      int min = _minhreads;
      int threadNum = _tasks.size() > max ? max : _tasks.size();
      threadNum = threadNum < min ? min : threadNum;
      while (threadNum > 0) {
        threadNum--;
        std::thread t(&ThreadPoolImpl::worker, this);
        assert(_threads.find(t.get_id()) == _threads.end());
        _threads[t.get_id()] = std::move(t);
        ++_currentNum;
      }
      if (!_tasks.empty()) {
        _conditionv.notify_all();
      }
      return true;
    }
  }
  return false;
}
bool ThreadPoolImpl::stop() {
  if (!_quit) {
    {
      std::lock_guard<std::mutex> guard(_mutex);
      _quit = true;
    }
    _conditionv.notify_all();

    for (auto& elem : _threads) {
      assert(elem.second.joinable());
      elem.second.join();
    }
    _currentNum = 0;
    _idleCount = 0;
    _threads.clear();
    while (!_threadIDs.empty()) {
      _threadIDs.pop();
    }
    {
      std::lock_guard<std::mutex> guard(_mutex);
      _quit = false;
    }
    _isRun = false;
    return true;
  }
  return false;
}
void ThreadPoolImpl::worker() {
  while (true) {
    std::function<void()> task;
    bool hasTimeout = false;
    {
      std::unique_lock<std::mutex> uniqueLock(_mutex);
      ++_idleCount;
      if (_currentNum > _minhreads) {
        hasTimeout = !_conditionv.wait_for(
            uniqueLock, std::chrono::seconds(WAIT_SECONDS),
            [this]() { return _quit || !_tasks.empty(); });
      } else {
        _conditionv.wait(uniqueLock,
                         [this]() { return _quit || !_tasks.empty(); });
      }

      --_idleCount;
      if (_tasks.empty()) {
        if (_quit) {
          --_currentNum;
          return;
        }
        if (hasTimeout && (_currentNum > _minhreads)) {
          --_currentNum;
          joinFinishedThreads();
          _threadIDs.emplace(std::this_thread::get_id());
          return;
        }
      } else {
        task = std::move(_tasks.front());
        _tasks.pop();
        task();
      }
    }
  }
}

void ThreadPoolImpl::joinFinishedThreads() {
  while (!_threadIDs.empty()) {
    auto id = std::move(_threadIDs.front());
    _threadIDs.pop();
    auto iter = _threads.find(id);

    assert(iter != _threads.end());
    assert(iter->second.joinable());

    iter->second.join();
    _threads.erase(iter);
  }
}
#ifdef __cplusplus
extern "C" {
#endif
std::string identity(void) {
  return THREADPOOL_IDENTITY;
}
std::string version(void) {
  return THREADPOOL_VERSION;
}
std::string description(void) {
  return THREADPOOL_DESCRIPTION;
}
void* create(const std::string& cfg) {
  return new ThreadPoolImpl();
}
void destroy(void* p) {
  if (p) {
    auto tp = reinterpret_cast<ThreadPoolImpl*>(p);
    delete tp;
  }
}
#ifdef __cplusplus
}
#endif
