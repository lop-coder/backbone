from conans import ConanFile, CMake, tools
import platform

class libTinyProcessConan(ConanFile):

    generators = "cmake"
    settings = "os", "build_type", "arch"
    options = {
        "shared": [True, False],
    }

    name = "libtinyprocess"
    version = "2.0.4"
    description = "A small platform independent library making it simple to create and stop new processes in C++, as well as writing to stdin and reading from stdout and stderr of a new process."
    url = "https://gitlab.com/eidheim/tiny-process-library"
    license = "None"
    author = ""
    topics = "C++", "process"

    def configure(self):
        self.options.shared = False

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
   
    def package(self):
        self.copy("process.hpp", dst="include/libtinyprocess", src='.')
        if self.settings.os == "Windows":
            self.copy("*.exe",
                      dst='bin',
                      src=f'build/{self.settings.build_type}')
            self.copy("*.dll",
                      dst='bin',
                      src=f'build/{self.settings.build_type}')
            self.copy("*.lib",
                      dst='lib',
                      src=f'build/{self.settings.build_type}')
            self.copy("*.pdb",
                      dst='lib',
                      src=f'build/{self.settings.build_type}')
        else:
            self.copy("libtinyprocess.*",
                      dst='lib',
                      src=f'build/')
            self.copy("examples",
                      dst='bin',
                      src=f'build/')

            
    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)


