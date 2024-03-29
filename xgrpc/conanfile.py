from conans import ConanFile, CMake, tools
import platform


class xgrpcConan(ConanFile):
    requires = (
        "grpc/1.54.3",
        "protobuf/3.21.12")

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

    name = "xgrpc"
    version = "0.0.1"
    description = "Some common modules or scaffolding codes based on C++ to improve development efficiency"
    url = "https://github.com/lop-coder/backbone"
    license = "None"
    author = "leo"
    topics = "C++", "extension", "grpc"

    def configure(self):
        self.options["protobuf"].shared = False
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
        self.copy("*.proto", dst="proto", src='proto')
        if self.settings.os == "Windows":
            self.copy("xgrpc.dll",
                dst='bin',
                src=f'build/{self.settings.build_type}')
            self.copy("xgrpc.lib",
                dst='lib',
                src=f'build/{self.settings.build_type}')
        else:
            self.copy("xgrpc", dst='bin', src='.')

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
