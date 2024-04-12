from conans import ConanFile, CMake, tools
import platform


class loggerConan(ConanFile):

    requires = ("nlohmann_json/3.11.3","fmt/10.0.0","spdlog/1.11.0")
    generators = "cmake_find_package"
    #generators = "cmake_find_package,cmake"
    #generators = "cmake"
    settings = "os", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_app": [True, False],
        "with_test": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "with_app": True,
        "with_test": False,
    }

    name = "logger"
    version = "2.1.0"
    description = "Turbo Core is a C++ library that manages tasks and extensions."
    url = ""
    license = "None"
    author = "duanjiaqi,luoping2"
    topics = "C++", "extension", "logger", "turbo"

    def configure(self):
        ...
       #self.options["fmt"].shared = True
       #self.options["spdlog"].shared = True
        #self.options["spdlog"].header_only = True

    def build(self):
        cmake = CMake(self)
        cmake.definitions['WITH_APP'] = self.options.with_app
        cmake.definitions['WITH_TEST'] = self.options.with_test

        cmake.configure()
        cmake.build()

    def package(self):
        if self.settings.os == "Windows":
            self.copy("logger.dll",
                      dst='bin',
                      src=f'build/bin')
            self.copy("logger.lib",
                                dst='lib',
                                src=f'build/lib')
            self.copy("liblogger.lib",
                                dst='lib',
                                src=f'build/lib')
            self.copy("*.h", dst="include", src='include')
        else:
            self.copy("logger", dst='bin', src='.')

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

    def imports(self):
        if self.settings.os == "Windows":
            self.copy("*.dll", dst='bin', src="bin") # From bin to bin
        else:
            self.copy("*.so*", dst=".", src="lib") # From lib to bin
