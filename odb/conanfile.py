from conans import ConanFile, CMake, tools
import platform

class ormConan(ConanFile):
    requires = (
            "fmt/10.0.0",
            "libodb/2.4.2","nlohmann_json/3.11.3",)

    #generators = "cmake_find_package"
    #generators = "cmake_find_package，cmake"
    generators = "cmake"
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
        "with_app": False,
        "with_test": False,
    }

    name = "orm"
    version="1.0.0"
    description = "Some common modules or scaffolding codes based on C++ to improve development efficiency"
    url = "https://github.com/lop-coder/backbone"
    license = "None"
    author = "leo"
    topics = "C++", "extension", "sam"

    def configure(self):
        self.options["sqlite3"].shared = True
        self.options["spdlog"].shared = True

    def requirements(self):
        ...

    def build(self):
        cmake = CMake(self)
        cmake.definitions['WITH_APP'] = self.options.with_app
        cmake.definitions['WITH_TEST'] = self.options.with_test
        cmake.definitions['WITH_VERSION'] = self.version
        cmake.configure()
        cmake.build()


    def package(self):
        self.copy("*", dst="include", src='include')
        if self.settings.os == "Windows":
            self.copy("orm.dll",
                      dst='bin',
                      src=f'build/bin')

        else:
            self.copy("orm.so", dst='lib', src='.')
            self.copy("orm.a", dst='lib', src='.')

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

	#依赖
    def imports(self):
        if self.settings.os == "Windows":
            self.copy("*.dll", dst='bin', src="bin")
            self.copy("*.dll", dst='bin', src="lib")
        else:
            self.copy("*.so*", dst=".", src="lib") # From lib to bin
