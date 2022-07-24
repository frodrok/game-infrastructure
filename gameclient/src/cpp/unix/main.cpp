
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define PORT 8001
#define MAXLINE 1024

struct ServerResponse {
  int status;
  std::string body;
};

int main(int argc, char** argv) {
  
  int sockfd;
  char buffer[MAXLINE];
  
  struct sockaddr_in servaddr;

  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  unsigned int n, len;

  json loginRequest;
  loginRequest["username"] = "fredrik";
  loginRequest["password"] = "hollinger";
  loginRequest["type"] = "login";

  std::string as_json = loginRequest.dump();

  const char* heyhey = as_json.c_str();

  std::cout << "json " << as_json << std::endl;

  sendto(sockfd,
         (const char *)heyhey,
         strlen(heyhey),
         MSG_CONFIRM,
         (const struct sockaddr *) &servaddr,
         sizeof(servaddr));

  printf("hello message sent\n");

  n = recvfrom(sockfd,
               (char *)buffer,
               MAXLINE,
               MSG_WAITALL,
               (struct sockaddr *) &servaddr,
               &len);

  printf("received\n");
  
  buffer[n] = '\n';

  printf("Server: %s\n", buffer);

  close(sockfd);

  return 0;
}
