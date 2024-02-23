#pragma once
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "common.h"
namespace Backbone {

template <class B>
class BuilderFactory {
 public:
  ~BuilderFactory() { clear(); };
  static BuilderFactory<B>& instance() {
    static BuilderFactory<B> instance;
    return instance;
  }

  template <typename S>
  static int enroll(const std::string& key) {
    static_assert(std::is_base_of_v<B, S>);
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    auto item = instance()._book.find(key);
    if (item != instance()._book.end()) {
      return 0;
    }
    instance()._book[key] = [](std::string config) -> std::shared_ptr<B> {
      return std::make_shared<S>(config);
    };
    return 1;
  };

  static std::shared_ptr<B> build(const std::string& key,
                                  const std::string& config) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    auto item = instance()._book.find(key);
    if (item != instance()._book.end()) {
      return item->second(config);
    }
    std::stringstream ss;
    ss << "the `" << key << "` does not exist in registry `"
       << "!";
    std::cerr << ss.str() << std::endl;
    return nullptr;
  }
  std::vector<std::string> list() {
    std::vector<std::string> result;
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    for (auto const& [key, val] : instance()._book) {
      result.push_back(key);
    }
    return result;
  }
  void clear() {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    instance()._book.clear();
    instance()._property.clear();
  }
  void remove(const std::string key) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    auto item = instance()._book.find(key);
    if (item != instance()._book.end()) {
      instance()._book.erase(item);
      auto propertyItem = instance()._property.find(key);
      if (propertyItem != instance()._property.end()) {
        instance()._property.erase(propertyItem);
      }
    }
  }
  static int setProperty(const std::string& key,
                         const std::string& property_key,
                         const std::string& property_value) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    if (instance()._book.count(key) == 0) {
      return 0;
    }
    auto propertyItem = instance()._property.find(key);
    if (propertyItem == instance()._property.end()) {
      instance()._property[key] = std::map<std::string, std::string>{};
    }
    instance()._property[key][property_key] = property_value;
    return 1;
  }

  static std::string getProperty(const std::string& key,
                                 const std::string& property_key) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    if (instance()._book.count(key) == 0) {
      return "";
    }
    auto item = instance()._property.find(key);
    if (item != instance()._property.end()) {
      if (instance()._property[key].count(property_key) > 0) {
        return instance()._property[key][property_key];
      }
      return "";
    }
    return "";
  }
  static int eraseProperty(const std::string& key,
                           const std::string& property_key) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    if (instance()._book.count(key) == 0) {
      return 0;
    }
    auto item = instance()._property.find(key);
    if (item != instance()._property.end()) {
      auto propertyItem = instance()._property[key].find(property_key);
      if (propertyItem != instance()._property[key].end()) {
        instance()._property[key].erase(property_key);
        return 1;
      }
    }
    return 0;
  }

 private:
  std::map<std::string, std::function<std::shared_ptr<B>(std::string)>> _book;
  std::map<std::string, std::map<std::string, std::string>> _property;
  std::mutex _mtx;
};

template <class B>
class HolderFactory {
 public:
  ~HolderFactory() { clear(); };
  static HolderFactory<B>& instance() {
    static HolderFactory<B> instance;
    return instance;
  }

  template <typename... P>
  static int enroll(const std::string& key, P&&... param) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    auto item = instance()._book.find(key);
    if (item != instance()._book.end()) {
      return 0;
    }
    auto newItem = std::make_shared<B>(std::forward<P>(param)...);
    instance()._book[key] = newItem;
    return 1;
  };

  static int enroll(const std::string& key, std::shared_ptr<B> newItem) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    auto item = instance()._book.find(key);
    if (item != instance()._book.end()) {
      return 0;
    }
    instance()._book[key] = newItem;
    return 1;
  };
  std::shared_ptr<B> get(const std::string key) {
    std::scoped_lock<std::mutex> lck(_mtx);
    auto item = _book.find(key);
    if (item != _book.end()) {
      return item->second;
    }
    std::stringstream ss;
    ss << "the `" << key << "` does not exist in registry `"
       << "!";
    std::cerr << ss.str() << std::endl;
    return nullptr;
  }
  std::vector<std::string> list() {
    std::vector<std::string> result;
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    for (auto const& [key, val] : instance()._book) {
      result.push_back(key);
    }
    return result;
  }
  void clear() {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    instance()._book.clear();
    instance()._property.clear();
  }
  void remove(const std::string key) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    auto item = instance()._book.find(key);
    if (item != instance()._book.end()) {
      instance()._book.erase(item);
      auto propertyItem = instance()._property.find(key);
      if (propertyItem != instance()._property.end()) {
        instance()._property.erase(propertyItem);
      }
    }
  }
  static int setProperty(const std::string& key,
                         const std::string& property_key,
                         const std::string& property_value) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    if (instance()._book.count(key) == 0) {
      return 0;
    }
    auto item = instance()._property.find(key);
    if (item == instance()._property.end()) {
      instance()._property[key] = std::map<std::string, std::string>{};
    }
    instance()._property[key][property_key] = property_value;
    return 1;
  }

  static std::string getProperty(const std::string& key,
                                 const std::string& property_key) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    if (instance()._book.count(key) == 0) {
      return "";
    }
    auto item = instance()._property.find(key);
    if (item != instance()._property.end()) {
      if (instance()._property[key].count(property_key) > 0) {
        return instance()._property[key][property_key];
      }
    }
    return "";
  }
  static int eraseProperty(const std::string& key,
                           const std::string& property_key) {
    std::scoped_lock<std::mutex> lck(instance()._mtx);
    if (instance()._book.count(key) == 0) {
      return 0;
    }
    auto item = instance()._property.find(key);
    if (item != instance()._property.end()) {
      auto propertyItem = instance()._property[key].find(property_key);
      if (propertyItem != instance()._property[key].end()) {
        instance()._property[key].erase(property_key);
        return 1;
      }
    }
    return 0;
  }

 private:
  std::map<std::string, std::shared_ptr<B>> _book;
  std::map<std::string, std::map<std::string, std::string>> _property;
  std::mutex _mtx;
};

template <class B, class S>
class BuilderFactoryAssistant {
 public:
  explicit BuilderFactoryAssistant(std::string key) {
    static_assert(std::is_base_of_v<B, S>);

    //BuilderFactory<B>::instance().enroll<S>(key); //ok for windows
    BuilderFactory<B>::instance().template enroll<S>(key);  //for g++
  }
};

template <class B>
class HolderFactoryAssistant {
 public:
  template <typename... P>
  HolderFactoryAssistant(std::string key, P&&... param) {
    if (!HolderFactory<B>::instance().enroll(key, std::forward<P>(param)...)) {
      std::stringstream ss;
      ss << "the `" << key << "` enroll failed `"
         << "!";
      throw std::invalid_argument(ss.str());
    }
  }
  HolderFactoryAssistant(std::string key, std::shared_ptr<B> newItem) {
    if (!HolderFactory<B>::instance().enroll(key, newItem)) {
      std::stringstream ss;
      ss << "the `" << key << "` enroll failed `"
         << "!";
      throw std::invalid_argument(ss.str());
    }
  }
};
}  // namespace Backbone
