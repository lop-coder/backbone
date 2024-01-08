#include <stdio.h>

#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
//非常重要
#ifdef DLL_EXPORTS
#undef DLL_EXPORTS
#endif

#include "../extensions/dummy/extension-dummy.h"
#include "../extensions/timemeasurement/extension-timemeasurement.h"
#include "backbone.h"

int main(int argc, char** argv) {
  cxxopts::Options options("backbone", "Load all extensions given an path!");
  // clang-format off
    options.add_options()
      ("p", "extension path", cxxopts::value<std::string>())
      ("h,help", "Show help");

  // clang-format on
  options.parse_positional({"path"});
  auto args = options.parse(argc, argv);

  if (args.count("help")) {
    std::cout << options.help({"", "Group"}) << std::endl;
    exit(0);
  }
  // fetch all arguments
  std::string path = args["p"].as<std::string>();
  std::filesystem::path fsPath(path);
  std::error_code ec;
  if (!std::filesystem::exists(path, ec)) {
    std::cerr << "path [" << path << "] not exist" << std::endl;
    return false;
  }
  std::filesystem::directory_iterator entrys;
  try {
    entrys = std::filesystem::directory_iterator(fsPath);
  } catch (const std::exception& e) {
    std::cerr << "open path [" << path << "] failed:" << e.what() << std::endl;
    return false;
  }
  for (const auto& entry : entrys) {
    if (entry.is_regular_file()) {
      auto entry_path = entry.path();
      if (!entry_path.extension().empty()) {
        if (entry_path.extension() == ".dll" ||
            entry_path.extension() == ".lib") {
          std::string load_path = entry.path().parent_path().string();
          std::string load_module = entry.path().filename().stem().string();
          std::string module_name = entry.path().filename().stem().string();
          try {
            Backbone::Hub::load(load_path, load_module, module_name);
          } catch (...) {
            //std::cerr<<"load :"<<entry.path().filename().stem().string()<<" failed"<<std::endl;
          }
        }
      }
    }
  }
  std::cout << "   " << std::endl;
  std::cout << "extention list:" << std::endl;
  auto list = Backbone::Hub::list();
  for (auto& item : list) {
    std::cout << "    " << item << std::endl;
  }
  std::cout << "   " << std::endl;

  auto extensionDummy = Backbone::Hub::create<ExtensionDummy>("dummy", "");
  if (!extensionDummy) {
    DisruptorParam p;
    extensionDummy->add(p);
    extensionDummy->cfg();
  }

  auto timeMeasurement =
      Backbone::Hub::create<TimeMeasurement>("timemeasurement", "");
  if (timeMeasurement) {
    timeMeasurement->start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cerr << "---------" << timeMeasurement->nanoseconds() << "---------"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cerr << "---------" << timeMeasurement->microseconds() << "---------"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cerr << "---------" << timeMeasurement->milliseconds() << "---------"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cerr << "---------" << timeMeasurement->seconds() << "---------"
              << std::endl;
  }

  return 0;
}
