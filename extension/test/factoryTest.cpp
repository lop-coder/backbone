#include <stdio.h>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include "factory.h"

//非常重要
#ifdef DLL_EXPORTS
#undef DLL_EXPORTS
#endif
// Demonstrate some basic assertions.

class A {
 public:
  A(std::string p) : _p(p){};
  ~A(){};
  virtual std::string name() { return "A"; };
  virtual std::string value() { return _p; };
  std::string _p;
};
class B : public A {
 public:
  B(std::string p) : A("B->" + p), _p(p){};
  ~B(){};
  virtual std::string name() { return "B"; };
  virtual std::string value() { return _p; };
  std::string _p;
};
class C : public B {
 public:
  C(std::string p) : B("C->" + p), _p(p){};
  ~C(){};
  virtual std::string name() { return "C"; };
  virtual std::string value() { return _p; };
  std::string _p;
};
class D {
 public:
  D(std::string p) : _p(p){};
  D(std::string p1, std::string p2) : _p(p1 + p2){};
  D(std::string p1, std::string p2, std::string p3) : _p(p1 + p2 + p3){};
  ~D(){};
  virtual std::string name() { return "D"; };
  virtual std::string value() { return _p; };
  std::string _p;
};
class E : public D {
 public:
  E(std::string p) : D("D->" + p), _p(p){};
  E(std::string p1, std::string p2) : D("D->" + p1 + p2), _p(p1 + p2){};

  ~E(){};
  virtual std::string name() { return "E"; };
  virtual std::string value() { return _p; };
  std::string _p;
};
TEST(BuilderFactoryTest, Base) {
  Backbone::BuilderFactory<A>::enroll<A>("A");
  Backbone::BuilderFactory<A>::enroll<B>("B");
  Backbone::BuilderFactory<A>::enroll<C>("C");

  Backbone::BuilderFactory<D>::enroll<D>("D1");
  Backbone::BuilderFactory<D>::enroll<D>("D2");
  Backbone::BuilderFactory<D>::enroll<E>("E1");
  Backbone::BuilderFactory<D>::enroll<E>("E2");

  EXPECT_EQ(Backbone::BuilderFactory<A>::instance().list().size(), 3);
  EXPECT_EQ(Backbone::BuilderFactory<D>::instance().list().size(), 4);

  EXPECT_NE(Backbone::BuilderFactory<A>::build("A", ""), nullptr);
  EXPECT_NE(Backbone::BuilderFactory<A>::build("B", ""), nullptr);
  EXPECT_NE(Backbone::BuilderFactory<A>::build("C", ""), nullptr);

  EXPECT_EQ(Backbone::BuilderFactory<A>::build("A", "")->name(), "A");
  EXPECT_EQ(Backbone::BuilderFactory<A>::build("B", "")->name(), "B");
  EXPECT_EQ(Backbone::BuilderFactory<A>::build("C", "")->name(), "C");

  Backbone::BuilderFactory<A>::instance().remove("B");

  EXPECT_EQ(Backbone::BuilderFactory<A>::instance().list().size(), 2);

  EXPECT_EQ(Backbone::BuilderFactory<A>::build("B", ""), nullptr);

  EXPECT_NE(Backbone::BuilderFactory<A>::build("A", ""), nullptr);
  EXPECT_NE(Backbone::BuilderFactory<A>::build("C", ""), nullptr);

  EXPECT_EQ(Backbone::BuilderFactory<A>::setProperty("A", "1", "1"), 1);
  EXPECT_EQ(Backbone::BuilderFactory<A>::setProperty("A", "2", "2"), 1);
  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("A", "1"), "1");
  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("A", "2"), "2");

  EXPECT_EQ(Backbone::BuilderFactory<A>::eraseProperty("A", "2"), 1);

  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("A", "2"), "");
  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("A", "1"), "1");

  EXPECT_EQ(Backbone::BuilderFactory<A>::eraseProperty("A", "1"), 1);
  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("A", "1"), "");

  EXPECT_EQ(Backbone::BuilderFactory<A>::setProperty("B", "1", "1"), 0);
  EXPECT_EQ(Backbone::BuilderFactory<A>::setProperty("B", "2", "2"), 0);
  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("B", "1"), "");
  EXPECT_EQ(Backbone::BuilderFactory<A>::getProperty("B", "2"), "");

  Backbone::BuilderFactory<A>::instance().clear();

  EXPECT_EQ(Backbone::BuilderFactory<D>::instance().list().size(), 4);
  EXPECT_EQ(Backbone::BuilderFactory<A>::instance().list().size(), 0);

  Backbone::BuilderFactoryAssistant<A, A> a("A");
  Backbone::BuilderFactoryAssistant<A, B> b("B");
  Backbone::BuilderFactoryAssistant<A, C> c("C");

  EXPECT_EQ(Backbone::BuilderFactory<A>::instance().list().size(), 3);
  EXPECT_EQ(Backbone::BuilderFactory<D>::instance().list().size(), 4);

  EXPECT_NE(Backbone::BuilderFactory<A>::build("A", ""), nullptr);
  EXPECT_NE(Backbone::BuilderFactory<A>::build("B", ""), nullptr);
  EXPECT_NE(Backbone::BuilderFactory<A>::build("C", ""), nullptr);

  EXPECT_EQ(Backbone::BuilderFactory<A>::build("A", "")->name(), "A");
  EXPECT_EQ(Backbone::BuilderFactory<A>::build("B", "")->name(), "B");
  EXPECT_EQ(Backbone::BuilderFactory<A>::build("C", "")->name(), "C");
}

TEST(HolderFactoryTest, Base) {
  Backbone::HolderFactory<D>::enroll("D1", "1");
  Backbone::HolderFactory<D>::enroll("D2", "1", "2");
  Backbone::HolderFactory<D>::enroll("D3", "1", "2", "3");
  Backbone::HolderFactory<D>::enroll("D4", std::make_shared<D>("4"));
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().list().size(), 4);

  Backbone::HolderFactory<E>::enroll("E1", "1");
  Backbone::HolderFactory<E>::enroll("E2", "1", "2");
  EXPECT_EQ(Backbone::HolderFactory<E>::instance().list().size(), 2);

  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D1"), nullptr);
  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D2"), nullptr);
  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D3"), nullptr);
  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D4"), nullptr);

  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D1")->value(), "1");
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D2")->value(), "12");
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D3")->value(), "123");
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D4")->value(), "4");

  EXPECT_NE(Backbone::HolderFactory<E>::instance().get("E1"), nullptr);
  EXPECT_NE(Backbone::HolderFactory<E>::instance().get("E2"), nullptr);

  EXPECT_EQ(Backbone::HolderFactory<E>::instance().get("E1")->value(), "1");
  EXPECT_EQ(Backbone::HolderFactory<E>::instance().get("E2")->value(), "12");

  Backbone::HolderFactory<D>::instance().remove("D2");
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().list().size(), 3);

  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D1"), nullptr);
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D2"), nullptr);
  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D3"), nullptr);
  EXPECT_NE(Backbone::HolderFactory<D>::instance().get("D4"), nullptr);

  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D1")->value(), "1");
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D3")->value(), "123");
  EXPECT_EQ(Backbone::HolderFactory<D>::instance().get("D4")->value(), "4");

  EXPECT_EQ(Backbone::HolderFactory<E>::instance().list().size(), 2);
  Backbone::HolderFactory<E>::instance().clear();
  EXPECT_EQ(Backbone::HolderFactory<E>::instance().list().size(), 0);

  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D1", "1", "1"), 1);
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D1", "2", "2"), 1);
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D1", "1"), "1");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D1", "2"), "2");

  EXPECT_EQ(Backbone::HolderFactory<D>::eraseProperty("D1", "2"), 1);

  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D1", "2"), "");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D1", "1"), "1");

  EXPECT_EQ(Backbone::HolderFactory<D>::eraseProperty("D1", "1"), 1);
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D1", "1"), "");

  //
  Backbone::HolderFactory<D>::instance().remove("D4");
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D4", "1", "1"), 0);
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D4", "2", "2"), 0);
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D4", "1"), "");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D4", "2"), "");

  //
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D3", "1"), "");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D3", "2"), "");
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D3", "1", "1"), 1);
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D3", "2", "2"), 1);
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D3", "1"), "1");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D3", "2"), "2");

  Backbone::HolderFactory<D>::instance().remove("D3");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D3", "1"), "");
  EXPECT_EQ(Backbone::HolderFactory<D>::getProperty("D3", "2"), "");
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D3", "1", "1"), 0);
  EXPECT_EQ(Backbone::HolderFactory<D>::setProperty("D3", "2", "2"), 0);
}
