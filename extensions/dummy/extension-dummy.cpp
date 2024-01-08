#include <stdio.h>

#include <iostream>
#include <memory>

#include "extension-dummy.h"

#ifdef __cplusplus
extern "C" {
#endif

std::string identity(void) {
  return "ExtensionDummy";
};
std::string version(void) {
  return "v1.0.0";
};
std::string description(void) {
  return "ExtensionDummy";
};
void* create(const std::string& cfg) {
  return new ExtensionDummy(cfg);
}
void destroy(void* p) {
  if (p) {
    auto tp = reinterpret_cast<ExtensionDummy*>(p);
    delete tp;
  }
}
#ifdef __cplusplus
}
#endif
