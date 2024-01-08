#pragma once
#include <stdio.h>

#include <iostream>
#include <memory>
#include "extension.h"

class BACKBONE_PLUGIN_API ExtensionDisruptor {
 public:
  ExtensionDisruptor(const std::string& cfg) {}
  virtual ~ExtensionDisruptor() {}
  /** @brief Get name. */
  virtual std::string name() { return "ExtensionDisruptor"; };
};
