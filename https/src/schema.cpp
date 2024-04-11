#include "schema.h"

//#include "logger.h"
//#include "logictype.h"

json schemaOfItem(std::string type) {
  if (type == "string") {
    auto stringSchema = json::object();
    stringSchema["type"] = type;
    stringSchema["minLength"] = 0;
    // stringSchema["maxLength"] = 100;
    // stringSchema["pattern"] =
    //     "^[\u4E00-\u9FA5A-Za-z0-9][\u4E00-\u9FA5A-Za-z0-9_\\-\\.]{0,49}$";
    return stringSchema;
  } else if (type == "number") {
    auto numberSchema = json::object();
    numberSchema["type"] = type;
    // numberSchema["minimum"] = 0.00;
    // numberSchema["maximum"] = 4.01;
    // numberSchema["multipleOf"] = 1;
    // numberSchema["exclusiveMinimum"] = 1;
    // numberSchema["exclusiveMaximum"] = 333;
    return numberSchema;
  } else if (type == "integer") {
    auto integerSchema = json::object();
    integerSchema["type"] = type;
    // integerSchema["minimum"] = 0;
    // integerSchema["maximum"] = 4;
    // integerSchema["multipleOf"] = 1;
    // integerSchema["exclusiveMinimum"] = 1;
    // integerSchema["exclusiveMaximum"] = 333;
    return integerSchema;
  } else if (type == "object") {
    json objectSchema = json::object();
    objectSchema["type"] = type;
    objectSchema["required"] = json::array();
    objectSchema["properties"] = json::object();
    return objectSchema;
  } else if (type == "array") {
    auto arraySchema = json::object();
    arraySchema["type"] = type;
    arraySchema["items"] = schemaOfName();
    arraySchema["minItems"] = 0;
    arraySchema["uniqueItems"] = true;
    // arraySchema["maxItems"] = 0;

    return arraySchema;
  } else if (type == "boolean") {
    auto booleanSchema = json::object();
    booleanSchema["type"] = type;
    return booleanSchema;
  } else if (type == "null") {
    auto nullSchema = json::object();
    nullSchema["type"] = type;
    return nullSchema;
  } else {
    auto emptySchema = json::object();
    return emptySchema;
  }
}
json schemaOfRoot() {
  json schema = schemaOfItem("object");
  return schema;
}
json schemaOfDescription() {
  auto description = schemaOfItem("string");
  description["minLength"] = 0;
  description["maxLength"] = 100;
  description.erase("pattern");
  return description;
}
json schemaOfName() {
  auto name = schemaOfItem("string");
  name["minLength"] = 1;
  name["maxLength"] = 50;
  name["pattern"] = "[\u4E00-\u9FA5A-Za-z0-9_\\-\\.]{0,50}$";
  return name;
}
json schemaOfAlgoType() {
  auto algoType = schemaOfItem("integer");
  algoType["minimum"] = 0;
  algoType["maximum"] = 5;
  return algoType;
}

json schemaOfNoEmptyArray() {
  auto arraySchema = json::object();
  arraySchema["type"] = "array";
  arraySchema["items"] = schemaOfName();
  arraySchema["minItems"] = 1;
  arraySchema["uniqueItems"] = true;
  return arraySchema;
}

bool jsonParseAndSchemaCheck(const std::string& body, json& schema,
                             json& request, json& response, bool need_patch) {
  json_validator validator;  // create validator
  try {
    //LogTrace("jsonParseAndSchemaCheck schema: {}\n", schema.dump());
    validator.set_root_schema(schema);  // insert root-schema
    try {
      request = json::parse(body);
      try {
        const auto default_patch = validator.validate(request);
        if (need_patch) {
          if (!default_patch.empty()) {
            request = request.patch(default_patch);
          } else {
            //LogError("Need patch but patch is empty schema:{} input:{}",
            //schema.dump(), request.dump());
          }
        }
        return true;
      } catch (const std::exception& e) {
        std::string msg = "Json validation failed, here is why:";
        msg += e.what();
        response["msg"] = msg;
        //LogWarn(response["msg"].get<std::string>());
        return false;
      }
    } catch (const nlohmann::detail::parse_error& e) {
      std::string msg = "Invalid json:\n";
      msg += body;
      response["msg"] = msg;
      //LogWarn(response["msg"].get<std::string>());
      return false;
    }
  } catch (const std::exception& e) {
    std::string msg = "Validation of schema failed, here is why: ";
    msg += e.what();
    response["msg"] = msg;
    //LogWarn(response["msg"].get<std::string>());
    return false;
  }
}
