#include <iostream>
#include <unordered_map>
#include <vector>
#include <charconv>
#include <regex>
#include "print.hpp"

struct JSONObject {
  std::variant<
      std::nullptr_t, // null
      bool,           // true
      int,            // 42
      double,         // 3.14
      std::string,    // "hello"
      std::vector<JSONObject>, // [42, "hello"]
      std::unordered_map<std::string, JSONObject> // {"hello": 95, "world": 211}
  > inner;

  void do_print() const {
    print(inner);
  }
};

template<class T>
std::optional<T> try_parse_num(std::string_view str) {
  T value;
  auto res = std::from_chars(str.data(), str.data() + str.size(), value);
  if (res.ec == std::errc() and res.ptr == str.data() + str.size()) {
    return value;
  }
  return std::nullopt;
}

JSONObject parse(std::string_view json) {
  if (json.empty()) {
    return JSONObject{std::nullptr_t{}};
  }
  if ('0' <= json[0] and json[0] <= '9' or json[0] == '+' or json[0] == '-') {
    std::regex num_re{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
    std::cmatch match;
    if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) {
      std::string str = match.str();
      if (auto num = try_parse_num<int>(str); num.has_value()) {
        return JSONObject{num.value()};
      }
      if (auto num = try_parse_num<double>(str); num.has_value()) {
        return JSONObject{num.value()};
      }
    }
  }
  return JSONObject{std::nullptr_t{}};
}

int main() {
  std::string_view str3 = "-3e+14";
  print(parse(str3));
  return 0;
}
