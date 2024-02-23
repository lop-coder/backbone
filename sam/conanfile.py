from conans import ConanFile, CMake, tools
import platform


class samConan(ConanFile):
    requires = (
    "nlohmann_json/3.11.3",
    "cxxopts/3.1.1",
        "opencv/4.5.5",
        "onnxruntime/1.14.1")

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
        "onnx:disable_static_registration":True,
        "onnx:shared":False
    }

    name = "sam"
    version = "0.0.1"
    description = "Some common modules or scaffolding codes based on C++ to improve development efficiency"
    url = "https://github.com/lop-coder/backbone"
    license = "None"
    author = "leo"
    topics = "C++", "extension", "sam"

    def configure(self):
        self.options["onnxruntime"].shared = False
        self.options["opencv"].shared = False

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
            self.copy("sam.dll",
                dst='bin',
                src=f'build/{self.settings.build_type}')
            self.copy("sam.lib",
                dst='lib',
                src=f'build/{self.settings.build_type}')
        else:
            self.copy("sam", dst='bin', src='.')

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
