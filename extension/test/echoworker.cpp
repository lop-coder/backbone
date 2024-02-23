#include <stdio.h>

#include <cxxopts.hpp>

#include <iostream>
std::string getMsg(){
    std::string cmd="";
    while (true) {
      char c = -1;
      c = std::cin.get();
      if (c == 13 || c == 10) {
        break;
      }
      cmd += c;
    }
    return cmd;
}
int main(int argc, char** argv) {
    cxxopts::Options options("echo",
                             "Echo Worker!");
    // clang-format off
    options.add_options()
      ("h,help", "Show help");

    // clang-format on
    options.parse_positional({"path"});
    auto args = options.parse(argc, argv);

    if (args.count("help")) {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }
    std::string msg;

    while ((msg=getMsg())!="") {
        if(msg=="quit"){
            std::cerr<<msg;
            break;
        }else{
            std::cout<<msg;
        }
    }
    exit(0);
}
