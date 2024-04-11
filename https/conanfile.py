from conans import ConanFile, CMake, tools
import platform


class httpsConan(ConanFile):
    requires = ("cxxopts/3.1.1","fmt/10.0.0","nlohmann_json/3.11.3","json-schema-validator/2.3.0","hwinfo/0.0.2")

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
        "shared": False,
        "fPIC": True,
        "with_app": True,
        "with_test": False,
    }

    name = "https"
    version = "0.0.1"
    description = "Some common modules or scaffolding codes based on C++ to improve development efficiency"
    url = "https://github.com/lop-coder/backbone"
    license = "None"
    author = "leo"
    topics = "C++", "extension", "http"

    def configure(self):
        ...

    def requirements(self):
        ...

    def build(self):
        cmake = CMake(self)
        cmake.definitions['WITH_APP'] = self.options.with_app
        cmake.definitions['WITH_TEST'] = self.options.with_test
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*", dst="include", src='include')
        if self.settings.os == "Windows":
            self.copy("https.dll",
                dst='bin',
                src=f'build/{self.settings.build_type}')
            self.copy("https.lib",
                dst='lib',
                src=f'build/{self.settings.build_type}')
        else:
            self.copy("https", dst='bin', src='.')

            def package_info(self):
                self.cpp_info.libs = tools.collect_libs(self)

            def imports(self):
                if self.settings.os == "Windows":
                    self.copy("*.dll", dst='dep', src="bin") # From bin to bin
                    self.copy("*.dll", dst='dep', src="lib") # From bin to bin
                else:
                    self.copy("*.so*", dst=".", src="lib") # From lib to bin
    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
    def imports(self):
        if self.settings.os == "Windows":
            self.copy("*.dll", dst='dep', src="bin") # From bin to bin
            self.copy("*.dll", dst='dep', src="lib") # From bin to bin
        else:
            self.copy("*.so*", dst=".", src="lib") # From lib to bin
