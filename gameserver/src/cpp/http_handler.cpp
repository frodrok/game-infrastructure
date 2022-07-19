enum HttpRequestType { GET, PUT, POST, DELETE };

HttpResult make_http_post_request(std::string url,
                                  std::string data) {
  
  CURL *curl_handle;
  CURLcode res;

  struct MemoryStruct chunk;
  chunk.memory = (char*) malloc(1);
  chunk.size = 0;

  auto httpRes = HttpResult{200, 0, ""};

  curl_handle = curl_easy_init();

  lo.info("making http post request");

  if (curl_handle) {

      struct curl_slist *headers = NULL;
      
      headers = curl_slist_append(headers, "Content-Type: application/json");
      curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data.c_str());
      curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
      curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk); 
      //      curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "name=fredrik&project=curl");
      curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-argent/1.0");
      
      res = curl_easy_perform(curl_handle);
      
      if (res != CURLE_OK) {
        lo.error(format("curl error %s", curl_easy_strerror(res)));
        httpRes.code = 999;
        httpRes.body = std::string("curl error ") + curl_easy_strerror(res);
      } else {

        lo.info("curl response ok");
        lo.info(format("size %s", (unsigned long) chunk.size));
        lo.info(format("data %s", chunk.memory));
        
        httpRes.code = 200;
        httpRes.body_len = (unsigned long) chunk.size;
        httpRes.body = std::string(chunk.memory);
      }
      
      curl_easy_cleanup(curl_handle);
      curl_slist_free_all(headers);
      free(chunk.memory);
      
  }

  lo.info("returning from http post request");
  
  return httpRes;
}

HttpResult make_http_request(std::string url,
                             HttpRequestType requestType) {

  CURL *curl_handle;
  CURLcode res;

  lo.info("initiating stuff http request");
  lo.info(url);

  struct MemoryStruct chunk;
  chunk.memory = (char*) malloc(1);
  chunk.size = 0;

  auto httpRes = HttpResult{200, 0, ""};

  curl_handle = curl_easy_init();

  if (curl_handle) {
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-argent/1.0");

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
      lo.error(format("curl error %s", curl_easy_strerror(res)));
      httpRes.code = 999;
      httpRes.body = std::string("curl error ") + curl_easy_strerror(res);
    } else {
      
      lo.info(format("size %s", (unsigned long) chunk.size));
      lo.info(format("data %s", chunk.memory));
      lo.info(format("data %s", res));

      httpRes.code = 200;
      httpRes.body_len = (unsigned long) chunk.size;
      httpRes.body = std::string(chunk.memory);
    }
    
    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    
  }
  
  return httpRes;
}

size_t WriteMemoryCallback(void* contents,
                           size_t size,
                           size_t nmemb,
                           void* userp) {

  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char*) realloc(mem->memory, mem->size + realsize + 1);

  if (ptr == NULL) {
    printf("error: not enough memory\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}


HttpResult HttpHandler::get_request(std::string url) {
  return make_http_request(url, HttpRequestType::GET);
}

HttpResult HttpHandler::post_request(std::string url, std::string body) {
  return make_http_post_request(url, body);
}
