#pragma once
#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#include <mutex>
namespace Backbone {
template <typename T>
struct Singleton {
  static T* getInstance() {
    static void* _p = nullptr;
    if (nullptr == _p) {
      static std::mutex m_mtx;
      std::lock_guard<std::mutex> lock{m_mtx};
      if (nullptr == _p) {
        _p = new T;
      }
    }
    return _p;
  }
  T* operator->() const { return getInstance(); }
};
}  // namespace Backbone

#endif
