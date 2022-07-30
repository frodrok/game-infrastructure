#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "structs.h"
#include "collision.cpp"
#include "helpers.cpp"

#define SERVER "127.0.0.1"
#define BUFLEN 1024
#define PORT 8001
#define CHARACTER_WIDTH 25;


SDL_Texture* textToTexture(SDL_Renderer* renderer,
                           TTF_Font* font,
                           const char* text,
                           SDL_Color color) {
  
  SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
  SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

  SDL_FreeSurface(surfaceMessage);
  
  return message;
  
}

int sendUdpPacket(int s,
                  sockaddr* si_other,
                  const char* message,
                  int slen) {

  if (sendto(s,
             message,
             strlen(message),
             0,
             si_other,
             slen) == SOCKET_ERROR) {
    
    printf("sendto failed with error code: %d", WSAGetLastError());
    return 1;
    
  } else {
    return 0;
  }             
}

std::string receiveUdpPacket(int s,
                       sockaddr* si_other,
                       int* slen) {

  char buf[BUFLEN];
  memset(buf, '\0', BUFLEN);

  if (recvfrom(s, buf, BUFLEN, 0, si_other, slen) == SOCKET_ERROR) {
    printf("recvfrom() failed %d", WSAGetLastError());
   
  }

  std::cout << "BUF: " << buf << std::endl;

  return std::string(buf);
}

ServerResponse sendLoginRequest(int s,
                                std::string username,
                                std::string password) {

  std::cout << "slr uname: " << username << ", pword: " << password << std::endl;

  struct sockaddr_in si_other;
  int slen = sizeof(si_other);
  memset( (char*) &si_other, 0, sizeof(si_other));

  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
  
  json loginMessageJson;
  loginMessageJson["type"] = "login";
  loginMessageJson["username"] = username;
  loginMessageJson["password"] = password;

  auto messageStr = loginMessageJson.dump();

  const char* message = messageStr.c_str();

  int result = sendUdpPacket(s,
                             (struct sockaddr *) &si_other,
                             message,
                             slen);

  if (result > 0) {
    ServerResponse response { 0, "error" };
    return response;
  }

  std::string packet = receiveUdpPacket(s,
                                  (struct sockaddr *) &si_other,
                                  &slen);

  std::cout << "received packet: " << packet << std::endl;

  json receivedParsed = json::parse(packet);

  std::cout << "received status: " << receivedParsed["status"] << std::endl;

  int status = receivedParsed["status"];
  std::string body = receivedParsed["body"];

  ServerResponse response { status,
                            body };
  
  return response;
}




int main(int argc, char** argv) {

  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 600;
  
  struct sockaddr_in si_other;
  
  int s, slen = sizeof(si_other);

  ClientState state = ClientState { "",
                                    "",
                                    "username"
  };

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

  auto messageStr = std::string("{\"username\":\"fredrik\",\"password\":\"hollinger\",\"type\":\"login\"}");

  const char* message = messageStr.c_str();

  memset( (char*) &si_other, 0, sizeof(si_other));
  
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

  std::cout << "1 addr: " << si_other.sin_addr.S_un.S_addr << std::endl;

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

  SDL_Window* window = NULL;
  SDL_Event event;
  SDL_Surface* screenSurface = NULL;
  SDL_Renderer* renderer = NULL;

  char* text;
  char* composition;

  Sint32 cursor;
  Sint32 selection_len;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
  } else {
    
    int success = SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer);

    if (success || window == NULL) {
      printf("Could not create wiondow: SDL_Error: %s\n", SDL_GetError());      
    } else {

      TTF_Init();
      SDL_StartTextInput();

      TTF_Font* sans = TTF_OpenFont("lib/nokiafc22.ttf", 24);

      SDL_Color white = {255, 255, 255};

      SDL_Texture* loginTextTexture = textToTexture(renderer,
                                               sans,
                                               "login prompt",
                                               white);

      SDL_Rect textRect;
      textRect.x = 0;
      textRect.y = 0;
      textRect.w = 200;
      textRect.h = 100;

      std::string loginUsernameText = std::string("username");
      std::string loginPasswordText = std::string("password");

      SDL_Texture* loginUsernameTexture = textToTexture(renderer,
                                                        sans,
                                                        loginUsernameText.c_str(),
                                                        white);
      SDL_Rect loginUsernameRect;
      loginUsernameRect.x = 250;
      loginUsernameRect.y = 0;
      loginUsernameRect.w = 200;
      loginUsernameRect.h = 100;
      
      SDL_Texture* loginPasswordTexture = textToTexture(renderer,
                                                        sans,
                                                        loginPasswordText.c_str(),
                                                        white);
      SDL_Rect loginPasswordRect;
      loginPasswordRect.x = 250;
      loginPasswordRect.y = 150;
      loginPasswordRect.w = 200;
      loginPasswordRect.h = 100;

      SDL_Texture* loginButtonTexture = textToTexture(renderer,
                                                      sans,
                                                      "login",
                                                      white);
      SDL_Rect loginButtonRect { 500, 300, 150, 100 };

      std::string loginStatusText = "";
      SDL_Texture* loginStatusTexture = textToTexture(renderer,
                                                      sans,
                                                      loginStatusText.c_str(),
                                                      white);
      SDL_Rect loginStatusRect { 500, 500, 150, 100 };
      
      
      screenSurface = SDL_GetWindowSurface(window);
      
      SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00) );

      bool running = true;      
      
      while (running) {
        
        if (SDL_PollEvent(&event)) {
          switch (event.type) {
          case SDL_QUIT: 
            running = false;
            break;
          case SDL_KEYDOWN:

            std::cout << "selectedTextbox: " << state.selectedTextbox << std::endl;

            switch (hash(state.selectedTextbox.c_str())) {
            case hash("username"):
              if (loginUsernameText.length() > 0 &&
                  event.key.keysym.sym == SDLK_BACKSPACE) {
                loginUsernameText.pop_back();
                loginUsernameRect.w = loginUsernameRect.w - CHARACTER_WIDTH;
              }
              
              loginUsernameTexture = textToTexture(renderer,
                                                   sans,
                                                   loginUsernameText.c_str(),
                                                   white);
              
              break;
            case hash("password"):
              if (loginPasswordText.length() > 0 &&
                  event.key.keysym.sym == SDLK_BACKSPACE) {
                loginPasswordText.pop_back();
                loginPasswordRect.w = loginPasswordRect.w - CHARACTER_WIDTH;
              }
              
              loginPasswordTexture = textToTexture(renderer,
                                                   sans,
                                                   loginPasswordText.c_str(),
                                                   white);
              break;
            default:
              break;
            }
            
            
            break;
          case SDL_TEXTINPUT:

            // can you switch on a string?
            switch (hash(state.selectedTextbox.c_str())) {
            case hash("username"):
              loginUsernameText = loginUsernameText + std::string(event.text.text);

              loginUsernameTexture = textToTexture(renderer,
                                                   sans,
                                                   loginUsernameText.c_str(),
                                                   white);
              
              loginUsernameRect.w = loginUsernameRect.w + CHARACTER_WIDTH;
              
              break;
            case hash("password"):

              loginPasswordText = loginPasswordText + std::string(event.text.text);

              loginPasswordTexture = textToTexture(renderer,
                                                   sans,
                                                   loginPasswordText.c_str(),
                                                   white);

              loginPasswordRect.w = loginPasswordRect.w + CHARACTER_WIDTH;
              
              break;
            default:
              break;
            
            }

            
            std::cout << "text: " << event.text.text << std::endl;
            std::cout << "total: " << loginUsernameText << std::endl;
            
            break;
            
          case SDL_TEXTEDITING:
            /*  composition = event.edit.text;
            cursor = event.edit.start;
            selection_len = event.edit.length; */

            std::cout << "composition: " << event.edit.text << event.edit.start << event.edit.length << std::endl;
            break;

          case SDL_MOUSEBUTTONDOWN:

            int mousePosX, mousePosY = 0;

            SDL_GetMouseState(&mousePosX, &mousePosY);

            auto clickedPoint = Point { mousePosX, mousePosY };

            bool loginButtonCollision = checkForCollision(clickedPoint, &loginButtonRect);

            bool usernameTextboxClicked = checkForCollision(clickedPoint, &loginUsernameRect);
            bool passwordTextboxClicked = checkForCollision(clickedPoint, &loginPasswordRect);

            if (usernameTextboxClicked) {
              state.selectedTextbox = "username";
            } else if (passwordTextboxClicked) {
              state.selectedTextbox = "password";
            }

            // TODO: Check for mouse click collision with the login button
            std::cout << "mouse click: " << event.button.button << std::endl;

            if (loginButtonCollision) {

              ServerResponse serverResponse = sendLoginRequest(s,
                                                               loginUsernameText,
                                                               loginPasswordText);

              std::cout << "after login request" << std::endl;

              std::string accessToken = "";
              std::string username = "";
              json responseBodyParsed;
              std::tuple<bool, json> jsonParseResult;

              std::cout << "before switch" << std::endl;
                            
              switch (serverResponse.status) {
              case 200:
                loginStatusText = "Login succeeded";
                
                std::cout << "before try_parse"  << std::endl;
                std::cout << serverResponse.body << std::endl;

                jsonParseResult = try_parse(serverResponse.body);

                if (std::get<0>(jsonParseResult)) {
                  responseBodyParsed = std::get<1>(jsonParseResult);
                  accessToken = responseBodyParsed["user_token"];
                  username = responseBodyParsed["username"];
                }
                
                break;
              default:
                std::cout << "default serverRespons.status: " << serverResponse.status << std::endl;
                loginStatusText = "Failed to log in";
                break;
              }

              state.username = username;
              state.accessToken = accessToken;
              
              loginStatusTexture = textToTexture(renderer,
                                                 sans,
                                                 loginStatusText.c_str(),
                                                 white);
              
              
              break;
              
            }
            
          }
          
        }        

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, loginTextTexture, NULL, &textRect);
        SDL_RenderCopy(renderer, loginStatusTexture, NULL, &loginStatusRect);
        SDL_RenderCopy(renderer, loginUsernameTexture, NULL, &loginUsernameRect);
        SDL_RenderCopy(renderer, loginPasswordTexture, NULL, &loginPasswordRect);
        SDL_RenderCopy(renderer, loginButtonTexture, NULL, &loginButtonRect);
        SDL_RenderPresent(renderer);

      }





      SDL_StopTextInput();
      SDL_FreeSurface(screenSurface);
      SDL_DestroyTexture(loginTextTexture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      closesocket(s);
      WSACleanup();
    }
  }

  return 0;
}
