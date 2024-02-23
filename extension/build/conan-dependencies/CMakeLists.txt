
        cmake_minimum_required(VERSION 3.15)
        project(conan-setup)
        include("C:/Qt/Tools/QtCreator/share/qtcreator/package-manager/conan.cmake")
        conan_cmake_run(
          CONANFILE "D:/work/src/backbone/extension/conanfile.py"
          INSTALL_FOLDER "D:/work/src/backbone/extension/build/conan-dependencies"
          GENERATORS cmake_paths json
          BUILD missing
          ENV CONAN_CMAKE_TOOLCHAIN_FILE="D:/work/src/backbone/extension/build/conan-dependencies/toolchain.cmake"
        )