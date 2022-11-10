

bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) {
    return false;
  }

  str.replace(start_pos, from.length(), to);
  return true;
}

static bool endsWith(std::string_view str, std::string_view suffix) {
  return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

template<class T>
std::string format(std::string fmtStr, T test) {
  /* char buff[100];
  snprintf(buff, sizeof(buff), fmtStr.c_str(), test);
  std::string buffAsStdStr = buff;
  return buffAsStdStr; */

  std::ostringstream stringStream;
  stringStream << fmtStr;
  stringStream << " " << test;
  return stringStream.str();
}
