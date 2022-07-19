#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H 1

struct MemoryStruct {
  char* memory;
  size_t size;
};

size_t WriteMemoryCallback(void* contents,
                           size_t size,
                           size_t nmemb,
                           void* userp);

struct HttpResult {
  unsigned int code;
  unsigned int body_len;
  std::string body;
};

class HttpHandler {
 private:
 public:
  HttpHandler() {};

  HttpResult get_request(std::string url);
  HttpResult get_request(std::string url, std::map<std::string, std::string> queryParams);

  HttpResult post_request(std::string url, std::string postBody);
};

#include "http_handler.cpp"

#endif
