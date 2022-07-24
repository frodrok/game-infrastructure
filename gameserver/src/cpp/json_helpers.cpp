void response_to_json(json& j, const ServerResponse& response) {
  j["status"] = response.status;
  j["body"] = response.body;
}

std::tuple<bool, json> parse_json(const char* str) {
  // Just abstracting away an ugly try-catch
  try {
    json parsed = json::parse(str);
    return std::tuple<bool, json>(true, parsed);
  } catch (...) {
    json empty;
    return std::tuple<bool, json>(false, empty);
  }
}

// wrap function call to handle exceptions
/* template<typename T>
std::tuple<bool, T> get_or(T (*f)(, T other) {
  T val;
  try {
    val = F();
    return std::tuple<bool, T>(true, val);
  } catch (...) {
    return std::tuple<bool, T>(false, other);
  }
  } */

template<typename T>
std::tuple<bool, T> invoke_wrap(T (*f)(int, int), int x, int y) {
  T val;
  try {
    val = f(x, y);
    return std::tuple<bool, T>(true, val);
  } catch (...) {
    T empty;
    return std::tuple<bool, T>(false, empty);
  }
}
