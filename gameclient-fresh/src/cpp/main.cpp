#include <stdio.h>
#include <optional>
#include <winsock2.h>
#include <string>
#include <iostream>
#include <chrono>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define SERVER "172.20.214.157"
//#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 27015
#define CHARACTER_WIDTH 25;

int sendKeepAlive(int s, sockaddr_in si_other) {
  auto keepAliveMessage = std::string("client:keepalive");
  if ( sendto(s,
              keepAliveMessage.c_str(),
              strlen(keepAliveMessage.c_str()),
              0,
              (struct sockaddr *) &si_other,
              sizeof(si_other)) == SOCKET_ERROR) {
    return 1;
  } else {
    return 0;
  }
}

int main(int argc, char** argv) {
  std::cout << "hello world" << std::endl;

  /* What does the client do?
     send a client:connect udp
   */

  struct sockaddr_in si_other;

  int s, slen = sizeof(si_other);

  char buf[BUFLEN];

  WSADATA wsa;

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    printf("failed. error code: %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }
  
  if ( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
    printf("socket() failed %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }
  
  // Send ping to see if we can connect to a server
  bool canPingServer = false;

  auto pingPayload = std::string("client:connect:Fredrike");
    
  memset( (char*) &si_other, 0, sizeof(si_other));
  
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

  std::cout << "1 addr: " << si_other.sin_addr.S_un.S_addr << std::endl;

  if ( sendto(s,
              pingPayload.c_str(),
              strlen(pingPayload.c_str()),
              0,
              (struct sockaddr *) &si_other,
              slen) == SOCKET_ERROR) {
    
    printf("sendto() failed with error code: %d", WSAGetLastError());
    exit(EXIT_FAILURE);
    
  } else {
    std::cout << "sent successfully" << std::endl;
  }

  std::cout << "sent, we are connected" << std::endl;

  /* What does the client want to do?
     Listen for UDP packets in and send a client:keepalive every 5 seconds */
  
  bool running = true;

  auto begin = std::chrono::steady_clock::now();
  auto receivedLastPacketAt = std::chrono::steady_clock::now();

  while (running) {

    memset(buf, '\0', BUFLEN);
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR) {
      std::cout << "recvfrom() failed with error code : " << WSAGetLastError() << std::endl;
      exit(EXIT_FAILURE);
    }

    std::cout << "received: " << std::string(buf) << std::endl;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin).count();
    std::cout << "elapsed (ms) = " << elapsed << std::endl;

    if (elapsed > 2500) {
      std::cout << "4.5s elapsed, sending keepalive" << std::endl;
      int success = sendKeepAlive(s, si_other);

      if (success > 0) {
        std::cout << "failed to send keepalive" << std::endl;
        exit(EXIT_FAILURE);
      } else {
        std::cout << "successfully sent keepalive" << std::endl;
        begin = std::chrono::steady_clock::now();
      }
    }
    
  }
  
  closesocket(s);
  WSACleanup();
              
  
  return 0;
}
