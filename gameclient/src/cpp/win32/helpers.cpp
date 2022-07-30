
std::tuple<bool, json> try_parse(std::string jsonString) {

  json result;
  std::tuple<bool, json> parsed = std::tuple(false, result);
  try {
    result = json::parse(jsonString);
    parsed = std::tuple(true, result);
    return parsed;
  } catch (...) {
    std::cout << "json parse exception" << std::endl;
    parsed = std::tuple(false, result);
    return parsed;
  }

}

constexpr unsigned int hash(const char *s, int off = 0) {
  return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}
