#pragma once

#ifndef _WIN32
#define BACKBONE_PLUGIN_API
#elif defined DLL_EXPORTS
#define BACKBONE_PLUGIN_API __declspec(dllexport)
#else
#define BACKBONE_PLUGIN_API __declspec(dllimport)
#endif

// export namespaces
namespace backbone {}  // namespace backbone
