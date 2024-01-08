#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>

//非常重要
#ifdef DLL_EXPORTS
#undef DLL_EXPORTS
#endif
#include "backbone.h"

#include "../../extensions/threadpool/extension-threadpool.h"

TEST(ThreadPoolTest, Base) {
  EXPECT_NO_THROW(Backbone::Hub::load("./", "threadpool", "ThreadPool"));
  EXPECT_NE(Backbone::Hub::create<ThreadPool>("ThreadPool", ""), nullptr);
  EXPECT_NO_THROW(Backbone::Hub::create<ThreadPool>("ThreadPool", ""));
  auto threadpool = Backbone::Hub::create<ThreadPool>("ThreadPool", "");
  auto func1 = []() -> bool {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cerr<<"----------------------func1----------------------------"<<std::endl;
    return true;
  };
  auto func2 = []() -> bool {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cerr<<"----------------------func2----------------------------"<<std::endl;
    return true;
  };
  auto func3 = []() -> bool {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cerr<<"----------------------func3----------------------------"<<std::endl;
    return true;
  };
  auto func4 = []() -> bool {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cerr<<"----------------------func4----------------------------"<<std::endl;
    return true;
  };
  auto func5 = []() -> bool {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    //std::cerr<<"----------------------func5----------------------------"<<std::endl;
    return true;
  };
  threadpool->addTask(func1);
  threadpool->addTask(func2);
  threadpool->addTask(func3);
  threadpool->addTask(func4);
  EXPECT_EQ(threadpool->taskQueueSize(), 4);
  threadpool->addTask(func5);
  EXPECT_EQ(threadpool->taskQueueSize(), 5);
  EXPECT_TRUE(threadpool->start(1, 6));
  std::this_thread::sleep_for(std::chrono::seconds(40));
  EXPECT_EQ(threadpool->threadNum(), 1);
  EXPECT_TRUE(threadpool->stop());
  threadpool->addTask(func1);
  threadpool->addTask(func2);
  threadpool->addTask(func3);
  threadpool->addTask(func4);
  threadpool->addTask(func5);
  EXPECT_TRUE(threadpool->start(1, 1));
  EXPECT_TRUE(threadpool->stop());
  threadpool.reset();
  EXPECT_NO_THROW(Backbone::Hub::unload("ThreadPool"));
}
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
