#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>

#include "../extensions/dummy/extension-disruptor.h"
#include "../extensions/dummy/extension-dummy.h"
#include "extension-hub.h"

TEST(ExtensionHubTest, Base) {
  EXPECT_NO_THROW(Backbone::Hub::load("./", "dummy", "dummy"));
  EXPECT_NO_THROW(Backbone::Hub::load("./", "disruptor", "disruptor"));
  EXPECT_ANY_THROW(Backbone::Hub::load("./", "null", "null"));
  {
    std::cout << "   " << std::endl;
    std::cout << "extention list:" << std::endl;
    auto list = Backbone::Hub::list();
    for (auto& item : list) {
      std::cout << "    " << item << std::endl;
    }
    std::cout << "   " << std::endl;
  }
  EXPECT_EQ(Backbone::Hub::list().size(), 2);

  EXPECT_EQ(Backbone::Hub::version("dummy"), "v1.0.0");
  EXPECT_EQ(Backbone::Hub::description("dummy"), "ExtensionDummy");

  EXPECT_EQ(Backbone::Hub::description("disruptor"), "ExtensionDisruptor");
  EXPECT_EQ(Backbone::Hub::version("disruptor"), "v1.0.0");

  EXPECT_NO_THROW(Backbone::Hub::create<ExtensionDummy>("dummy", ""));
  EXPECT_NO_THROW(Backbone::Hub::create<ExtensionDisruptor>("disruptor", ""));
  EXPECT_NE(Backbone::Hub::create<ExtensionDummy>("dummy", ""), nullptr);
  EXPECT_NE(Backbone::Hub::create<ExtensionDisruptor>("disruptor", ""),
            nullptr);

  EXPECT_NO_THROW(Backbone::Hub::create<ExtensionDummy>("dummy", ""));
  EXPECT_EQ(Backbone::Hub::create<ExtensionDummy>("dummy", "")->name(),
            "ExtensionDummy");
  DisruptorParam p{1, 2};
  EXPECT_EQ(Backbone::Hub::create<ExtensionDummy>("dummy", "dummy1")->add(p),
            3);
  EXPECT_NO_THROW(Backbone::Hub::unload("disruptor"));
  EXPECT_EQ(Backbone::Hub::list().size(), 1);
  EXPECT_EQ(Backbone::Hub::description("dummy"), "ExtensionDummy");
  EXPECT_EQ(Backbone::Hub::version("dummy"), "v1.0.0");
  EXPECT_NE(Backbone::Hub::create<ExtensionDummy>("dummy", ""), nullptr);
  EXPECT_NO_THROW(Backbone::Hub::create<ExtensionDummy>("dummy", ""));
  Backbone::Hub::unload("dummy");
  EXPECT_EQ(Backbone::Hub::list().size(), 0);
  EXPECT_EQ(Backbone::Hub::description("dummy"), "");
  EXPECT_EQ(Backbone::Hub::version("dummy"), "");
  EXPECT_EQ(Backbone::Hub::create<ExtensionDummy>("dummy", ""), nullptr);
  EXPECT_NO_THROW(Backbone::Hub::create<ExtensionDummy>("dummy", ""));
}
