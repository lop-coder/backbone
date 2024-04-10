#include "systeminfo.h"

#include <filesystem>
#include <iostream>
#include <unordered_map>

#include "common.h"
#include "logic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
SystemInfoRouter::SystemInfoRouter() {
  REGISTER_ROUTEREX("GET", "/v1/hardware", SystemInfoRouter::hardware, false);
  REGISTER_ROUTEREX("GET", "/v1/system_memory", SystemInfoRouter::systemMemory,
                    false);
}
SystemInfoRouter::~SystemInfoRouter() {}
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
resMsg* SystemInfoRouter::hardware(std::string method, std::string host,
                                   std::string proto, std::string path,
                                   std::map<std::string, std::string> querys,
                                   std::map<std::string, std::string> headers,
                                   std::map<std::string, std::string> form,
                                   std::string body)
*/
DEFINITION_ROUTER(SystemInfoRouter, hardware) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  try {
    res["data"]["cpu"] = json::object();
    {
      TurboLogic::CPUInfo_t cpuInfo;
      json cpu;

      logic.systemInfo()->CPUInfo(cpuInfo);

      cpu["vendor"] = cpuInfo.vendor;
      cpu["model"] = cpuInfo.model;
      cpu["cores"] = cpuInfo.total_cores;
      cpu["usage"] = cpuInfo.usage;
      res["data"]["cpu"] = cpu;

      if (cpuInfo.error.type != TurboLogic::ErrorType::Success) {
        res["code"] = cpuInfo.error.type;
        res["msg"] = cpuInfo.error.message;
      }
    }
    res["data"]["cuda_version"] = json::object();
    {
      TurboLogic::CudaInfo cudaInfo;
      json cuda;

      logic.systemInfo()->CUDAInfo(cudaInfo);

      cuda["installed"] = cudaInfo.installed;
      cuda["version"] = cudaInfo.version;
      cuda["match"] = cudaInfo.match;
      cuda["requirement"] = cudaInfo.requirement;

      res["data"]["cuda_version"] = cuda;
      if (cudaInfo.error.type != TurboLogic::ErrorType::Success) {
        res["code"] = cudaInfo.error.type;
        res["msg"] = cudaInfo.error.message;
      }
    }
    res["data"]["gpu"] = json::array();
    {
      TurboLogic::GPUsInfo_t gpuInfo;
      logic.systemInfo()->GPUInfo(gpuInfo);

      for (auto iter = gpuInfo.gpus.begin(); iter != gpuInfo.gpus.end();
           iter++) {
        json gpu;
        gpu["name"] = iter->name;
        gpu["major"] = iter->major;
        gpu["minor"] = iter->minor;
        gpu["avail"] = iter->avail;
        gpu["total"] = iter->total;
        res["data"]["gpu"].push_back(gpu);
      }
      if (gpuInfo.error.type != TurboLogic::ErrorType::Success) {
        res["code"] = gpuInfo.error.type;
        res["msg"] = gpuInfo.error.message;
      }
    }
    res["data"]["system"] = json::object();
    {
      TurboLogic::SysVersion sysVersion;
      logic.systemInfo()->SystemVersion(sysVersion);
      json system;
      system["arch"] = sysVersion.arch;
      system["description"] = sysVersion.description;
      if (sysVersion.error.type != TurboLogic::ErrorType::Success) {
        res["code"] = sysVersion.error.type;
        res["msg"] = sysVersion.error.message;
      }
      res["data"]["system"] = system;
    }
  } catch (const nlohmann::detail::parse_error& e) {
    /*
res["msg"] = "parse result fail";
res["code"] = TurboLogic::ErrorType::ErrUnknown;
*/
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};

/*
resMsg* SystemInfoRouter::systemMemory(
    std::string method, std::string host, std::string proto, std::string path,
    std::map<std::string, std::string> querys,
    std::map<std::string, std::string> headers,
    std::map<std::string, std::string> form, std::string body)
    */
DEFINITION_ROUTER(SystemInfoRouter, systemMemory) {
  TurboLogic::Logic& logic =
      TurboLogic::Logic::getInstance(TurboLogic::LogicType::Local);
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = TurboLogic::ErrorType::Success;
  res["msg"] = "";
  try {
    res["data"]["free"] = 0;
    res["data"]["total"] = 0;
    res["data"]["used"] = 0;
    {
      TurboLogic::MemoryInfo memoryInfo;
      logic.systemInfo()->MemInfo(memoryInfo);
      res["data"]["free"] = memoryInfo.free;
      res["data"]["total"] = memoryInfo.total;
      res["data"]["used"] = memoryInfo.used;
    }
  } catch (const nlohmann::detail::parse_error& e) {
    /*
res["msg"] = "parse result fail";
res["code"] = TurboLogic::ErrorType::ErrUnknown;
*/
  }
  std::string dumpData;
  dumpResponse(res, dumpData);
  return std::make_shared<TurboHttp::resMsg>(res.dump());
};
