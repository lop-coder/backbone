#include <stdio.h>

#include <iostream>
#include <memory>

#include "extension-disruptor.h"

#ifdef __cplusplus
extern "C" {
#endif

std::string identity(void) {
  return "ExtensionDisruptor";
};
std::string version(void) {
  return "v1.0.0";
};
std::string description(void) {
  return "ExtensionDisruptor";
};
void* create(const std::string& cfg) {
  return new ExtensionDisruptor(cfg);
}
void destroy(void* p) {
  if (p) {
    auto tp = reinterpret_cast<ExtensionDisruptor*>(p);
    delete tp;
  }
}
#ifdef __cplusplus
}
#endif
