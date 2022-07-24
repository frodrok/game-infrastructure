#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <SDL2/SDL.h>
#include <stdio.h>

#define SERVER "127.0.0.1"
#define BUFLEN 1024
#define PORT 8001

int main(int argc, char** argv) {

  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 600;
  
  struct sockaddr_in si_other;
  
  int s, slen = sizeof(si_other);

  char buf[BUFLEN];
  //  char message[BUFLEN];

  WSADATA wsa;

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    printf("faled. error code: %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if ( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
    printf("socket() failed %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  //   gets(message);

  auto messageStr = std::string("{\"username\":\"fredrik\",\"password\":\"hollinger\",\"type\":\"login\"}");

  const char* message = messageStr.c_str();

  memset( (char*) &si_other, 0, sizeof(si_other));
  
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

  if ( sendto(s,
              message,
              strlen(message),
              0,
              (struct sockaddr *) &si_other,
              slen) == SOCKET_ERROR) {
    printf("sendto() failed with error code: %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  printf("sent");

  memset(buf, '\0', BUFLEN);

  printf("receiving");

  if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR) {
    printf("recvfrom() failed %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  printf("received");

  printf("server sent: %s", buf);

  closesocket(s);
  WSACleanup();

  SDL_Window* window = NULL;

  SDL_Surface* screenSurface = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
  } else {
    window = SDL_CreateWindow("SDL test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
      printf("Could not create wiondow: SDL_Error: %s\n", SDL_GetError());      
    } else {
      screenSurface = SDL_GetWindowSurface(window);
      SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00) );

      SDL_UpdateWindowSurface(window);
      SDL_Delay(2000);

      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }

  return 0;
}
