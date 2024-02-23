#pragma once
#include <functional>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif
BACKBONE_PLUGIN_API std::string identity(void);
BACKBONE_PLUGIN_API std::string version(void);
BACKBONE_PLUGIN_API std::string description(void);
BACKBONE_PLUGIN_API void* create(const std::string& cfg);
BACKBONE_PLUGIN_API void destroy(void* p);
#ifdef __cplusplus
}
#endif
