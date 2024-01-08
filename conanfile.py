from conans import ConanFile, CMake, tools
import platform


class backboneConan(ConanFile):

    requires = ("doxygen/1.9.4","cxxopts/3.1.1","fmt/10.0.0","nlohmann_json/3.11.2","libtinyprocess/2.0.4",)
    generators = "cmake"
    settings = "os", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_app": [True, False],
        "with_test": [True, False],
        "with_doc": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "with_app": True,
        "with_test": True,
        "with_doc": True,
    }

    name = "backbone"
    version = "3.1.0"
    description = "Some common modules or scaffolding codes based on C++ to improve development efficiency"
    url = "https://github.com/lop-coder/backbone"
    license = "None"
    author = "luoping2"
    topics = "C++", "extension"

    def configure(self):
       ...

    def build(self):
        cmake = CMake(self)
        #if self.settings.os == "Windows":
        #    cmake = CMake(self, generator="Visual Studio 16 2019")
        #else:
        #    cmake = CMake(self, generator="Unix Makefiles")
        cmake.definitions['WITH_APP'] = self.options.with_app
        cmake.definitions['WITH_TEST'] = self.options.with_test
        cmake.definitions['WITH_DOC'] = self.options.with_doc
        cmake.definitions['WITH_VERSION'] = self.version
        cmake.definitions['WITH_EXTENSION'] = True
        cmake.configure()
        cmake.build()

    def package(self):
        if self.settings.os == "Windows":
            self.copy("*",dst='bin',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/bin')
            self.copy("*",dst='lib',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/lib')
            self.copy("*",dst='doc',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/doc')
            self.copy("*",dst='demo',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/demo')
            self.copy("*",dst='include',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/include')
            self.copy("*",dst='extension',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/extension')
        elif self.settings.os == "Linux":
            self.copy("*",dst='bin',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/bin')
            self.copy("*",dst='lib',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/lib')
            self.copy("*",dst='doc',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/doc')
            self.copy("*",dst='demo',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/demo')
            self.copy("*",dst='include',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/include')
            self.copy("*",dst='extension',src=f'output/'+str(self.settings.os)+'/'+str(self.settings.build_type)+'/'+str(self.settings.arch)+'/backbone/extension')

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

    def imports(self):
        if self.settings.os == "Windows":
            self.copy("*.dll", dst='bin', src="bin") # From bin to bin
        else:
            self.copy("*.so*", dst=".", src="lib") # From lib to bin
