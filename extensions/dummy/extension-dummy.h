#pragma once
#include <stdio.h>

#include <iostream>
#include <memory>
#include "extension.h"

struct BACKBONE_PLUGIN_API DisruptorParam {
  int a;
  int b;
};
class BACKBONE_PLUGIN_API ExtensionDummy {
 public:
  ExtensionDummy(std::string cfg) : _cfg(cfg) {}
  virtual ~ExtensionDummy() {}
  virtual std::string name() { return "ExtensionDummy"; };
  virtual int add(struct DisruptorParam& a) { return a.a + a.b; }
  virtual std::string cfg() { return _cfg; }

 private:
  std::string _cfg;
};
