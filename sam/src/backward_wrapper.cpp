#include "backward_wrapper.h"

#include <stdio.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <vector>

#include <iostream>
#include <sstream>
#include <stdexcept>
#if ISDEBUG
#include <backward.hpp>
#if defined(_WIN32)
#include <windows.h>
#endif
#endif

#ifndef ISDEBUG
#define ISDEBUG
#endif

class BackwardWrapperImpl : public BackwardWrapper {
 public:
  BackwardWrapperImpl();
  ~BackwardWrapperImpl();
  virtual void Init(std::string dumpfilepath) override;

 private:
#if ISDEBUG
  backward::SignalHandling* backwardSH = nullptr;
#endif
};
//////////////////////////////////////////////////////////////////////////
void BackwardWrapperImpl::Init(std::string dumpfilepath) {

#if ISDEBUG
  if (!backwardSH) {
#if defined(_WIN32)
    //The system does not display the Windows Error Reporting dialog.
    SetErrorMode(SetErrorMode(0) | SEM_NOGPFAULTERRORBOX);

    //Disable the debug assertion dialog on Windows
    //This code will disable display of dialog. Instead, it will print an error in the output window, and stderr.
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

    std::vector<int> signals;
    signals.push_back(SIGSEGV);
    signals.push_back(SIGILL);
    signals.push_back(SIGFPE);
    signals.push_back(SIGSEGV);
    signals.push_back(SIGTERM);
    signals.push_back(SIGBREAK);
    signals.push_back(SIGABRT);

    backwardSH = new backward::SignalHandling(signals, dumpfilepath);
  }
#endif
}
#if ISDEBUG
BackwardWrapperImpl::BackwardWrapperImpl() : backwardSH(nullptr) {}
#else
BackwardWrapperImpl::BackwardWrapperImpl() {}
#endif
BackwardWrapperImpl::~BackwardWrapperImpl() {
#if ISDEBUG
  if (backwardSH) {
    delete backwardSH;
    backwardSH = nullptr;
  }
#endif
}

std::shared_ptr<BackwardWrapper> newBackwardWrapper() {
  return std::make_shared<BackwardWrapperImpl>();
}
