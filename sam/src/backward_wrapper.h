#ifndef _BACKWARDWRAPPER_H_
#define _BACKWARDWRAPPER_H_

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

class BackwardWrapper {
 public:
  BackwardWrapper(){};
  virtual ~BackwardWrapper(){};
  virtual void Init(std::string dumpfilepath){};
};
std::shared_ptr<BackwardWrapper> __declspec(dllexport) newBackwardWrapper();

/*
#include "backward_wrapper.h"
int main(int argc, char** argv) {
    auto backward=newBackwardWrapper();
    backward->Init("./backward.dumpcore");
}
*/
#endif
