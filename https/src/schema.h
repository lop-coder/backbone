#ifndef _JSON_SCHEMA_H_
#define _JSON_SCHEMA_H_

#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

json schemaOfItem(std::string type);
json schemaOfRoot();
json schemaOfDescription();
json schemaOfName();
json schemaOfAlgoType();
json schemaOfNoEmptyArray();

bool jsonParseAndSchemaCheck(const std::string& body, json& schema,
                             json& request, json& response,
                             bool need_patch = false);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NLOHMANN_JSON_FROM(v1)
#undef NLOHMANN_JSON_FROM(v1)
#define NLOHMANN_JSON_FROM(v1)                          \
  if (nlohmann_json_j.contains(#v1)) {                  \
    nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1); \
  }
#endif

#ifdef NLOHMANN_JSON_FROM_WITH_DEFAULT(v1)
#undef NLOHMANN_JSON_FROM_WITH_DEFAULT(v1)
#define NLOHMANN_JSON_FROM_WITH_DEFAULT(v1) \
  nlohmann_json_t.v1 = nlohmann_json_j.value(#v1, nlohmann_json_t.v1)
#endif

template <typename T>
struct has_member_to_json {
 private:
  template <typename U>
  static std::true_type Check(decltype(&U::to_json));
  template <typename U>
  static std::false_type Check(...);

 public:
  static constexpr bool value =
      std::is_same<decltype(Check<T>(nullptr)), std::true_type>::value;
};

template <typename T>
struct has_member_from_json {
 private:
  template <typename U>
  static std::true_type Check(decltype(&U::from_json));
  template <typename U>
  static std::false_type Check(...);

 public:
  static constexpr bool value =
      std::is_same<decltype(Check<T>(nullptr)), std::true_type>::value;
};

template <class T>
class JsonSerializer {
 public:
  T from_json(json& s, bool need_patch = false) {
    check();
    T t;
    {
      if (!_schema.empty()) {
        json_validator validator;
        validator.set_root_schema(_schema);
        const auto default_patch = validator.validate(s);
        if (need_patch) {
          if (!default_patch.empty()) {
            s = s.patch(default_patch);
          } else {
            //LogError("Need patch but patch is empty schema:{} input:{}",
            //         _schema.dump(), s.dump());
          }
        }
      }
    }
    t.from_json(s, t);
    return t;
  }
  T from_json(json& s, T& t, bool need_patch = false) {
    check();
    {
      if (!_schema.empty()) {
        json_validator validator;
        validator.set_root_schema(_schema);
        const auto default_patch = validator.validate(s);
        if (need_patch) {
          if (!default_patch.empty()) {
            s = s.patch(default_patch);
          } else {
            //LogError("Need patch but patch is empty schema:{} input:{}",
            //         _schema.dump(), s.dump());
          }
        }
      }
    }
    t.from_json(s, t);
    return t;
  }
  json to_json(T& t) {
    check();
    json s;
    t.to_json(s, t);
    {
      if (!_schema.empty() && !s.empty()) {
        json_validator validator;
        validator.set_root_schema(_schema);
        validator.validate(s);
      }
    }
    return s;
  }
  json get_schema() { return _schema; }
  void set_schema(json schema = json()) { _schema = schema; }

 private:
  void check() {
    if (!has_member_to_json<T>::value) {
      std::stringstream ss;
      ss << "Lost to_json function !!";
      throw std::invalid_argument(ss.str());
    }
    if (!has_member_from_json<T>::value) {
      std::stringstream ss;
      ss << "Lost from_json function !!";
      throw std::invalid_argument(ss.str());
    }
  }
  json _schema = json();
};
#endif  //_JSON_SCHEMA_H_
