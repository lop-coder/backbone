#pragma once
#ifndef _EXTENSIONS_HUB_H_
#define _EXTENSIONS_HUB_H_

#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <vector>

#include <sstream>
#include "extension.h"

namespace Backbone {
namespace Hub {
BACKBONE_PLUGIN_API void load(const std::string& path,
                              const std::string& libname,
                              const std::string& name);
BACKBONE_PLUGIN_API void load(const std::string& libname,
                              const std::string& name);
BACKBONE_PLUGIN_API void unload(const std::string& name);
BACKBONE_PLUGIN_API std::vector<std::string> list();

BACKBONE_PLUGIN_API std::string identity(const std::string& name);
BACKBONE_PLUGIN_API std::string version(const std::string& name);
BACKBONE_PLUGIN_API std::string description(const std::string& name);

BACKBONE_PLUGIN_API std::function<void*(const std::string&)> createHander(
    const std::string& name);
BACKBONE_PLUGIN_API std::function<void(void*)> destroyHander(
    const std::string& name);

template <typename T>
std::shared_ptr<T> create(const std::string& name, const std::string& cfg) {
  if (name.empty()) {
    return nullptr;
  }
  auto createFunc = Backbone::Hub::createHander(name);
  auto destroyFunc = Backbone::Hub::destroyHander(name);
  if (createFunc && destroyFunc) {
    auto original = createFunc(cfg.c_str());
    if (original) {
      auto obj = reinterpret_cast<T*>(original);
      if (obj) {
        return std::shared_ptr<T>(obj, destroyFunc);
      }
    }
    return nullptr;
  }
  return nullptr;
}

}  // namespace Hub
}  // namespace Backbone

#endif
