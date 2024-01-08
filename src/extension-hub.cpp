

#include <extension-hub.h>
#include <factory.h>
#include <dllloader.hpp>

namespace Backbone {
namespace Hub {

static void InstallationProperties(const std::string& name,
                                   const std::string& libname) {
  auto val = HolderFactory<dylib>::instance().get(name);
  if (val) {
    if (val->has_symbol("identity")) {
      auto identityFunc = val->get_function<std::string(void)>("identity");
      if (val->has_symbol("create")) {
        auto createFunc =
            val->get_function<void*(const std::string& cfg)>("create");
        if (val->has_symbol("destroy")) {
          auto destroyFunc = val->get_function<void(void*)>("destroy");
          if (val->has_symbol("version")) {
            auto versionFunc = val->get_function<std::string()>("version");
            if (val->has_symbol("description")) {
              auto descriptionFunc =
                  val->get_function<std::string()>("description");
            } else {
              std::stringstream ss;
              ss << "the `" << libname
                 << "` extended dynamic library does not comply with the "
                    "specification:Missing 'description' function `"
                 << "!";
              throw std::invalid_argument(ss.str());
            }
          } else {
            std::stringstream ss;
            ss << "the `" << libname
               << "` extended dynamic library does not comply with the "
                  "specification:Missing 'version' function `"
               << "!";
            throw std::invalid_argument(ss.str());
          }
        } else {
          std::stringstream ss;
          ss << "the `" << libname
             << "` extended dynamic library does not comply with the "
                "specification:Missing 'destroy' function `"
             << "!";
          throw std::invalid_argument(ss.str());
        }
      } else {
        std::stringstream ss;
        ss << "the `" << libname
           << "` extended dynamic library does not comply with the "
              "specification:Missing 'create' function `"
           << "!";
        throw std::invalid_argument(ss.str());
      }
    } else {
      std::stringstream ss;
      ss << "the `" << libname
         << "` extended dynamic library does not comply with the "
            "specification:Missing 'identity' function `"
         << "!";
      throw std::invalid_argument(ss.str());
    }

  } else {
    std::stringstream ss;
    ss << "the `" << libname << "` dynamic library loading failed `"
       << "!";
    throw std::invalid_argument(ss.str());
  }
}
void load(const std::string& path, const std::string& libname,
          const std::string& name) {
  std::string extemsionName = (name == "" ? libname : name);
  if (extemsionName.empty()) {
    std::stringstream ss;
    ss << "The libname and name cannot all be empty"
       << "!";
    throw std::invalid_argument(ss.str());
  }
  if (HolderFactory<dylib>::instance().enroll(extemsionName, path, libname) ==
      1) {
    try {
      InstallationProperties(extemsionName, libname);
    } catch (const std::exception& e) {
      HolderFactory<dylib>::instance().remove(extemsionName);
      throw std::invalid_argument(e.what());
    }
  }
}
void load(const std::string& libname, const std::string& name) {
  std::string extemsionName = name == "" ? libname : name;
  if (extemsionName.empty()) {
    std::stringstream ss;
    ss << "The libname and name cannot all be empty"
       << "!";
    throw std::invalid_argument(ss.str());
  }
  HolderFactory<dylib>::instance().enroll(extemsionName, libname);
  try {
    InstallationProperties(extemsionName, libname);
  } catch (const std::exception& e) {
    HolderFactory<dylib>::instance().remove(extemsionName);
    throw std::invalid_argument(e.what());
  }
}
void unload(const std::string& name) {
  HolderFactory<dylib>::instance().remove(name);
}

std::vector<std::string> list() {
  return HolderFactory<dylib>::instance().list();
}

std::function<void*(const std::string&)> createHander(const std::string& name) {
  if (name.empty()) {
    return nullptr;
  }
  auto val = HolderFactory<dylib>::instance().get(name);
  if (val) {
    if (val->has_symbol("create")) {
      return val->get_function<void*(const std::string&)>("create");
    }
  }
  return nullptr;
}

std::function<void(void*)> destroyHander(const std::string& name) {
  if (name.empty()) {
    return nullptr;
  }
  auto val = HolderFactory<dylib>::instance().get(name);
  if (val) {
    if (val->has_symbol("destroy")) {
      return val->get_function<void(void*)>("destroy");
    }
  }
  return nullptr;
}

std::string identity(const std::string& name) {
  if (name.empty()) {
    return "";
  }
  auto val = HolderFactory<dylib>::instance().get(name);
  if (val) {
    if (val->has_symbol("identity")) {
      auto identityFunc = val->get_function<std::string(void)>("identity");
      return identityFunc();
    }
  }
  return "";
}

std::string version(const std::string& name) {
  if (name.empty()) {
    return "";
  }
  auto val = HolderFactory<dylib>::instance().get(name);
  if (val) {
    if (val->has_symbol("version")) {
      auto versionFunc = val->get_function<std::string(void)>("version");
      return versionFunc();
    }
  }
  return "";
}

std::string description(const std::string& name) {
  if (name.empty()) {
    return "";
  }
  auto val = HolderFactory<dylib>::instance().get(name);
  if (val) {
    if (val->has_symbol("description")) {
      auto descriptionFunc =
          val->get_function<std::string(void)>("description");
      return descriptionFunc();
    }
  }
  return "";
}
}  // namespace Hub
}  // namespace Backbone
