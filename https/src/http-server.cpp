#include <hwinfo/PCIMapper.h>
#include <hwinfo/hwinfo.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <thread>
#include "httplib.h"
#include "https.h"
#include "router/systeminfo.h"

#if ISDEBUG
#ifdef LEAK_DETECTOR
#include <vld.h>
#endif
#endif

#ifdef HTTPS_DLL_EXPORTS
#undef HTTPS_DLL_EXPORTS
#endif

class HardwareRouter {
 public:
  HardwareRouter();
  ~HardwareRouter();
  void Install(std::shared_ptr<Https::HttpSrv> srv);

  DECLARE_ROUTER(os);
  DECLARE_ROUTER(cpu);
  DECLARE_ROUTER(gpu);
  DECLARE_ROUTER(disk);
  DECLARE_ROUTER(memory);
  DECLARE_ROUTER(battery);
  DECLARE_ROUTER(hardware);
};

HardwareRouter::HardwareRouter() {}
HardwareRouter::~HardwareRouter() {}

void HardwareRouter::Install(std::shared_ptr<Https::HttpSrv> srv) {
  REGISTER_ROUTEREX(srv, "GET", "/v1/os", HardwareRouter::os, 0);
  REGISTER_ROUTEREX(srv, "GET", "/v1/cpu", HardwareRouter::cpu, 0);
  REGISTER_ROUTEREX(srv, "GET", "/v1/gpu", HardwareRouter::gpu, 0);
  REGISTER_ROUTEREX(srv, "GET", "/v1/disk", HardwareRouter::disk, 0);
  REGISTER_ROUTEREX(srv, "GET", "/v1/memory", HardwareRouter::memory, 0);
  REGISTER_ROUTEREX(srv, "GET", "/v1/battery", HardwareRouter::battery, 0);
  REGISTER_ROUTEREX(srv, "GET", "/v1/hardware", HardwareRouter::hardware, 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINITION_ROUTER(HardwareRouter, os) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    hwinfo::OS os;
    auto osObj = json::object();
    osObj["full name"] = os.fullName();
    osObj["short name"] = os.name();
    osObj["version"] = os.version();
    osObj["kernel"] = os.kernel();
    osObj["architecture"] = (os.is32bit() ? "32 bit" : "64 bit");
    osObj["endianess"] = (os.isLittleEndian() ? "little endian" : "big endian");

    res["data"]["os"] = osObj;
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
DEFINITION_ROUTER(HardwareRouter, cpu) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    res["data"]["cpu"] = json::array();
    auto sockets = hwinfo::getAllSockets();
    for (const auto& s : sockets) {
      const auto& cpu = s.CPU();
      auto cpuObj = json::object();
      cpuObj["id"] = s.id();
      cpuObj["vendor"] = cpu.vendor();
      cpuObj["model"] = cpu.modelName();
      cpuObj["physical cores"] = cpu.numPhysicalCores();
      cpuObj["logical cores"] = cpu.numLogicalCores();
      cpuObj["max frequency"] = cpu.maxClockSpeed_MHz();
      cpuObj["regular frequency"] = cpu.regularClockSpeed_MHz();
      cpuObj["min frequency"] = cpu.minClockSpeed_MHz();
      cpuObj["current frequency"] = cpu.currentClockSpeed_MHz();
      cpuObj["cache size"] = cpu.cacheSize_Bytes();
      cpuObj["CPU Usage Average"] = cpu.currentUtility_Percentage();

      res["data"]["cpu"].push_back(cpuObj);
    }
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
DEFINITION_ROUTER(HardwareRouter, gpu) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    res["data"]["gpu"] = json::array();
    auto gpus = hwinfo::getAllGPUs();
    for (auto& gpu : gpus) {
      auto gpuObj = json::object();
      gpuObj["id"] = gpu.id();
      gpuObj["vendor"] = gpu.vendor();
      gpuObj["model"] = gpu.name();
      gpuObj["driverVersion"] = gpu.driverVersion();
      gpuObj["memory [MiB]"] =
          static_cast<double>(gpu.memory_Bytes()) / 1024.0 / 1024.0;
      gpuObj["frequency"] = gpu.frequency_MHz();

      res["data"]["gpu"].push_back(gpuObj);
    }
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
DEFINITION_ROUTER(HardwareRouter, disk) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    res["data"]["disk"] = json::array();
    std::vector<hwinfo::Disk> disks = hwinfo::getAllDisks();
    if (!disks.empty()) {
      int disk_counter = 0;
      for (const auto& disk : disks) {
        auto diskObj = json::object();
        diskObj["id"] = disk_counter++;
        diskObj["serial-number"] = disk.serialNumber();
        diskObj["size [MiB]"] = disk.size_Bytes() / 1024 / 1024;
        res["data"]["disk"].push_back(diskObj);
      }
    } else {
      std::cout << "No Disks installed or detected" << std::endl;
    }
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
DEFINITION_ROUTER(HardwareRouter, memory) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    auto memObj = json::object();

    hwinfo::RAM ram;
    memObj["vendor"] = ram.vendor();
    memObj["model"] = ram.model();
    memObj["serial-number"] = ram.serialNumber();
    memObj["size [MiB]"] = ram.total_Bytes() / 1024 / 1024;
    memObj["free [MiB]"] = ram.free_Bytes() / 1024 / 1024;
    memObj["available [MiB]"] = ram.available_Bytes() / 1024 / 1024;

    res["data"]["memory"] = memObj;
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
DEFINITION_ROUTER(HardwareRouter, battery) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    res["data"]["battery"] = json::array();
    std::vector<hwinfo::Battery> batteries = hwinfo::getAllBatteries();
    if (!batteries.empty()) {
      int battery_counter = 0;
      for (auto& battery : batteries) {
        auto batteryObj = json::object();
        batteryObj["id"] = battery_counter++;
        batteryObj["vendor"] = battery.vendor();
        batteryObj["model"] = battery.model();
        batteryObj["serial-number"] = battery.serialNumber();
        batteryObj["charging"] = battery.charging();
        batteryObj["capacity"] = battery.capacity();

        res["data"]["battery"].push_back(batteryObj);
      }
    }
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
DEFINITION_ROUTER(HardwareRouter, hardware) {
  json req;
  json res;
  res["data"] = json::object();
  res["code"] = 0;
  res["msg"] = "";
  try {
    //os
    hwinfo::OS os;
    auto osObj = json::object();
    osObj["full name"] = os.fullName();
    osObj["short name"] = os.name();
    osObj["version"] = os.version();
    osObj["kernel"] = os.kernel();
    osObj["architecture"] = (os.is32bit() ? "32 bit" : "64 bit");
    osObj["endianess"] = (os.isLittleEndian() ? "little endian" : "big endian");

    res["data"]["os"] = osObj;
    //cpu
    res["data"]["cpu"] = json::array();
    auto sockets = hwinfo::getAllSockets();
    for (const auto& s : sockets) {
      const auto& cpu = s.CPU();
      auto cpuObj = json::object();
      cpuObj["id"] = s.id();
      cpuObj["vendor"] = cpu.vendor();
      cpuObj["model"] = cpu.modelName();
      cpuObj["physical cores"] = cpu.numPhysicalCores();
      cpuObj["logical cores"] = cpu.numLogicalCores();
      cpuObj["max frequency"] = cpu.maxClockSpeed_MHz();
      cpuObj["regular frequency"] = cpu.regularClockSpeed_MHz();
      cpuObj["min frequency"] = cpu.minClockSpeed_MHz();
      cpuObj["current frequency"] = cpu.currentClockSpeed_MHz();
      cpuObj["cache size"] = cpu.cacheSize_Bytes();
      cpuObj["CPU Usage Average"] = cpu.currentUtility_Percentage();

      res["data"]["cpu"].push_back(cpuObj);
    }

    //gpu
    res["data"]["gpu"] = json::array();
    auto gpus = hwinfo::getAllGPUs();
    for (auto& gpu : gpus) {
      auto gpuObj = json::object();
      gpuObj["id"] = gpu.id();
      gpuObj["vendor"] = gpu.vendor();
      gpuObj["model"] = gpu.name();
      gpuObj["driverVersion"] = gpu.driverVersion();
      gpuObj["memory [MiB]"] =
          static_cast<double>(gpu.memory_Bytes()) / 1024.0 / 1024.0;
      gpuObj["frequency"] = gpu.frequency_MHz();

      res["data"]["gpu"].push_back(gpuObj);
    }
    //disk
    res["data"]["disk"] = json::array();
    std::vector<hwinfo::Disk> disks = hwinfo::getAllDisks();
    if (!disks.empty()) {
      int disk_counter = 0;
      for (const auto& disk : disks) {
        auto diskObj = json::object();
        diskObj["id"] = disk_counter++;
        diskObj["serial-number"] = disk.serialNumber();
        diskObj["size [MiB]"] = disk.size_Bytes() / 1024 / 1024;
        res["data"]["disk"].push_back(diskObj);
      }

    } else {
      std::cout << "No Disks installed or detected" << std::endl;
    }

    //memory
    auto memObj = json::object();
    hwinfo::RAM ram;
    memObj["vendor"] = ram.vendor();
    memObj["model"] = ram.model();
    //memObj["name"]=ram.name();
    memObj["serial-number"] = ram.serialNumber();
    memObj["size [MiB]"] = ram.total_Bytes() / 1024 / 1024;
    memObj["free [MiB]"] = ram.free_Bytes() / 1024 / 1024;
    memObj["available [MiB]"] = ram.available_Bytes() / 1024 / 1024;

    res["data"]["memory"] = memObj;

    //battery
    res["data"]["battery"] = json::array();
    std::vector<hwinfo::Battery> batteries = hwinfo::getAllBatteries();
    if (!batteries.empty()) {
      int battery_counter = 0;
      for (auto& battery : batteries) {
        auto batteryObj = json::object();
        batteryObj["id"] = battery_counter++;
        batteryObj["vendor"] = battery.vendor();
        batteryObj["model"] = battery.model();
        batteryObj["serial-number"] = battery.serialNumber();
        batteryObj["charging"] = battery.charging();
        batteryObj["capacity"] = battery.capacity();

        res["data"]["battery"].push_back(batteryObj);
      }
    }
  } catch (std::exception& e) {
    res["code"] = -1;
    res["msg"] = e.what();
  }
  return std::make_shared<Https::resMsg>(res.dump());
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "config.h"
int main(int argc, char* argv[]) {
  bool status = true;
  int port = 8888;

  auto httpSrv = Https::newHttpServer();

  std::shared_ptr<HardwareRouter> hardwareRouterRouter =
      std::make_shared<HardwareRouter>();
  hardwareRouterRouter->Install(httpSrv);

  httpSrv->authentication(3);

  std::thread([&] {
    int tryCount = 6;
    while (tryCount >= 0) {
      if (httpSrv->start("127.0.0.1", port)) {
        break;
      }
      std::cerr << "http server [" << std::to_string(port) << "] start error!!"
                << std::endl;
      port++;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      tryCount--;
    }
  }).detach();

  while (status) {
    if (httpSrv->status()) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  status = false;

  // HTTP
  httplib::Client cli("127.0.0.1", port);
  auto res = cli.Get("/v1/hardware");
  std::cout << "    status:" << res->status << std::endl;
  std::cout << "    body:" << std::endl;
  std::cout << res->body << std::endl;
}
