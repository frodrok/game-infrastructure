#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <curl/curl.h>
#include <cstring>
#include <cstdlib>
#include <map>

#include "option.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


class DebugLogger {
private:
public:
  DebugLogger() {};

  void info(std::string str) {
    std::string dateString = "";
    std::cout << dateString << "[INFO]: " << str << std::endl; 
  }

  void error(std::string errStr) {
    std::string dateString = "";
    std::cout << dateString << "[DEBUG]: " << errStr << std::endl;
  }
  
};                          


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

DebugLogger lo = DebugLogger();


#define PORT    8001
#define MAXLINE 1024

#include "http_handler.h"

/* enum MESSAGE_TYPES {
  LOGIN = "login",
  GET_CHARACTERS = "get_chars",                       
  }; */

struct ServerResponse {
  int status;
  std::string body;
};

void response_to_json(json& j, const ServerResponse& response) {
  j["status"] = response.status;
  j["body"] = response.body;
}

ServerResponse process_message(std::map<std::string, std::string> SERVER_URLS,
                               HttpHandler* httpHandler,
                               json message) {

  auto response = ServerResponse { 0, "" };
  
  if (message["type"] == "login") {
    
    lo.info("got a message to login");
    lo.info(message["username"]);

    auto loginUrl = SERVER_URLS["loginUrl"];
    
    auto loginResponse = httpHandler->post_request(loginUrl, message.dump());
    
    auto qt = json::parse(loginResponse.body);
    
    lo.info(format("user_token %s", qt["user_token"]));
    
    response.status = 200;
    response.body = qt.dump();
    
  } else {
    
    response.status = 0;
    response.body = "failure";
    
  }

  return response;
}


using namespace std::chrono_literals;

int main(int argc, char** argv) {
  
  lo.info("Starting gameserver");

  std::string gameServiceURL = "";

  if (const char* gameServiceURLGet = std::getenv("GAME_SERVICE_URL")) {
    gameServiceURL = gameServiceURLGet;
  };

  auto httpHandler = HttpHandler();
  lo.info("pinging game-service REST API");

  bool running = true;

  if (gameServiceURL.empty()) {
    lo.error("Could not get GAME_SERVICE_URL from the environment, not starting");
    return 1;
  }

  auto t = httpHandler.get_request(gameServiceURL);

  lo.info(t.body);

  const std::map<std::string, std::string> SERVER_URLS = {
    { "loginUrl", std::string(gameServiceURL) + "account/login" },
    { "x", std::string(gameServiceURL) + "account/login" },
    { "y", std::string(gameServiceURL) + "account/login" },
  };
  
  int sockfd;
  char buffer[MAXLINE];
  char *hello = "Hello from server";

  struct sockaddr_in servaddr, cliaddr;

  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }                

  while (running) {

    auto start = std::chrono::high_resolution_clock::now();

    // Open for UDP packets
    unsigned int len, n;
    
    len = sizeof(cliaddr);
    
    n = recvfrom( sockfd,
                  (char*) buffer,
                  MAXLINE,
                  MSG_WAITALL,
                  (struct sockaddr *) &cliaddr,
                  &len);
    
    buffer[n] = '\0';
    printf("client : %s\n", buffer);

    json incomingMessage = json::parse(buffer);
    
    lo.info(format("inc msg type: %s", incomingMessage["type"]));

    ServerResponse response = process_message(SERVER_URLS,
                                              &httpHandler,
                                              incomingMessage);

    json responseAsJson;
    response_to_json(responseAsJson, response);

    lo.info(responseAsJson.dump());

    std::string as_json = responseAsJson.dump();
    const char* rr = as_json.c_str();
    
    sendto(sockfd,
           (const char*)rr,
           strlen(rr),
           MSG_CONFIRM,
           (const struct sockaddr *) &cliaddr,
           len);
    
    lo.info(format("responded with %s", as_json));

    std::this_thread::sleep_for(100ms);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;

    // lo.info(format("Waited %s ms", elapsed.count()));

            
  }
  return 0;
}

