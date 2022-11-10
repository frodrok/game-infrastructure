#include <stdio.h>
#include <optional>
#include <winsock2.h>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include <GL/gl.h>

#include "typedefs.h"
#include "structs.h"
#include "collision.cpp"
#include "helpers.cpp"

#define SERVER "127.0.0.1"
#define BUFLEN 1024
#define PORT 8001
#define CHARACTER_WIDTH 25;

#include "server_communication.cpp"


class Logger {
private:
public:
  Logger() {};
  info(std::string logString) {
    std::cout << "[INFO]: " << logString << std::endl;
  }
};

SDL_Texture* textToTexture(SDL_Renderer* renderer,
                           TTF_Font* font,
                           const char* text,
                           SDL_Color color) {
  
  SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
  SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

  SDL_FreeSurface(surfaceMessage);
  
  return message;
  
}


void renderPlayingCharacterScreen(SDL_Renderer* renderer,
                                  TTF_Font* font,
                                  SDL_Color color,
                                  ClientState* state) {

  // TODO: Assert state.currentCharacterName not null or empty

  // This can easily be cached
  // SDL_Texture* characterNameTexture = textToTexture(renderer,
  //                                                   font,
  //                                                   state->currentCharacterName.c_str(),
  //                                                   color);

  /*   SDL_Texture* characterNameTexture = textToTexture(renderer,
                                                    font,
                                                    "true face",
                                                    color);

  

  SDL_Rect playerPosition { 10, 50, 200, 75 };

  SDL_RenderCopy(renderer, characterNameTexture, NULL, &playerPosition); */
                                                    
}

void renderPlayerUI(SDL_Renderer* renderer,
                    TTF_Font* font,
                    SDL_Color color,
                    ClientState* state) {

  if (state->currentCharacter.has_value()) {

    Character currentCharacter = state->currentCharacter.value();
    
    SDL_Texture* characterNameTexture = textToTexture(renderer,
                                                      font,
                                                      currentCharacter.characterName.c_str(),
                                                      color);
    
    SDL_Rect playerNamePosition { 10, 25, 200, 75 };

    SDL_Texture* characterHealthTexture = textToTexture(renderer,
                                                        font,
                                                        std::to_string(currentCharacter.stats.health).c_str(),
                                                        color);

    SDL_Rect playerHealthPos { 10, 650, 200, 75 };

    SDL_Texture* characterManaTexture = textToTexture(renderer,
                                                      font,
                                                      std::to_string(currentCharacter.stats.mana).c_str(),
                                                      color);

    SDL_Rect playerManaPos = { 800, 650, 200, 75 };    
    
    SDL_RenderCopy(renderer, characterNameTexture, NULL, &playerNamePosition);
    SDL_RenderCopy(renderer, characterHealthTexture, NULL, &playerHealthPos);
    SDL_RenderCopy(renderer, characterManaTexture, NULL, &playerManaPos);
  }


}

void renderChooseCharacterScreen(SDL_Renderer* renderer,
                           TTF_Font* font,
                           SDL_Color color, 
                           SDL_Texture* logoutTextTexture,
                           SDL_Rect* logoutTextRect,
                           std::vector<Character> characters) {

  SDL_Texture* characterNameTexture = textToTexture(renderer,
                                                    font,
                                                    characters[0].characterName.c_str(),
                                                    color);

  SDL_Rect characterNameRect { 10, 50, 200, 75 };

  SDL_Texture* characterRaceTexture = textToTexture(renderer,
                                                    font,
                                                    characters[0].race.c_str(),
                                                    color);
  
  SDL_Rect characterRaceRect { 10, 110, 150, 60 };
  
  SDL_Texture* characterClassTexture = textToTexture(renderer,
                                                    font,
                                                    characters[0].className.c_str(),
                                                    color);
  
  SDL_Rect characterClassRect { 200, 110, 150, 65 };

  SDL_Texture* characterLevelTexture = textToTexture(renderer,
                                                     font,
                                                     std::to_string(characters[0].level).c_str(),
                                                     color);
  
  SDL_Rect characterLevelRect { 300, 50, 100, 75 };

  
  SDL_RenderCopy(renderer, characterLevelTexture, NULL, &characterLevelRect);
  SDL_RenderCopy(renderer, characterRaceTexture, NULL, &characterRaceRect);
  SDL_RenderCopy(renderer, characterClassTexture, NULL, &characterClassRect);
  SDL_RenderCopy(renderer, characterNameTexture, NULL, &characterNameRect);
  SDL_RenderCopy(renderer, logoutTextTexture, NULL, logoutTextRect);
}

int main(int argc, char** argv) {

  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 600;
  
  struct sockaddr_in si_other;
  
  int s, slen = sizeof(si_other);

  ClientState state = ClientState { "",
                                    "",
                                    "username",
                                    "login",
                                    std::vector<Character>(0),
                                    std::nullopt,
                                    Point { 10, 10 }
                                    
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

  // Send ping to see if we can connect to a server
  bool canPingServer = false;
  
  auto pingPayload = std::string("{\"type\":\"ping\"}");

  auto messageStr = std::string("{\"username\":\"fredrik\",\"password\":\"hollinger\",\"type\":\"login\"}");

  const char* message = messageStr.c_str();

  memset( (char*) &si_other, 0, sizeof(si_other));
  
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

  std::cout << "1 addr: " << si_other.sin_addr.S_un.S_addr << std::endl;

  if ( sendto(s,
              pingPayload.c_str(),
              strlen(message),
              0,
              (struct sockaddr *) &si_other,
              slen) == SOCKET_ERROR) {
    printf("sendto() failed with error code: %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  printf("sent\n");

  memset(buf, '\0', BUFLEN);

  printf("receiving\n");

  std::tuple<Uint32, Uint32> gameResolution = std::make_tuple(1280, 768);

  if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR) {
    printf("recvfrom() failed %d", WSAGetLastError());
    //exit(EXIT_FAILURE);
    canPingServer = false;
  } else {
    canPingServer = true;
  }

  printf("received\n");

  printf("server sent: %s\n", buf);

  SDL_Window* window = NULL;
  SDL_Event event;
  SDL_Surface* screenSurface = NULL;
  SDL_Renderer* renderer = NULL;

  Logger lo = Logger();
  
  char* text;
  char* composition;

  Sint32 cursor;
  Sint32 selection_len;

  /* If we can't reach a game server just make it a single player example game */
  if (!canPingServer) {
    
    state.currentView = "playing_character";

    auto playingCharacter = Character {
      "mage",
      "07ff-00aa-5555-4343-ffff",
      70,
      655500,
      "Fredrik",
      "human",
      CharacterStats { 32, 32, 32, 32, 32, 100, 100 },
    };

    state.currentCharacter = playingCharacter;
  }

  /*   const SDL_VideoInfo* info = NULL;
  
  int bpp = 0;
  int flags = 0; */

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
  } else {

    /* this only exists in sdl1.2, i'm not sure why it was in the libsdl opengl example :/
      info = SDL_GetVideoInfo();

    if (!info) {
      fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
      } */

    int windowWidth = std::get<0>(gameResolution);
    int windowHeight = std::get<1>(gameResolution);
    uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    int success = SDL_CreateWindowAndRenderer(windowWidth, windowHeight, windowFlags, &window, &renderer);

    bool fullscreen = false;

    if (success || window == NULL) {
      printf("Could not create wiondow: SDL_Error: %s\n", SDL_GetError());      
    } else {

      TTF_Init();
      SDL_StartTextInput();

      TTF_Font* sans = TTF_OpenFont("lib/nokiafc22.ttf", 24);

      SDL_GLContext context = SDL_GL_CreateContext(window);

      SDL_Color white = {255, 255, 255};

      std::string loginUsernameText = std::string("username");
      std::string loginPasswordText = std::string("password");
      std::string loginStatusText = "";

      SDL_Texture* loginTextTexture = textToTexture(renderer,
                                                    sans,
                                                    "login prompt",
                                                    white);
      
      SDL_Texture* loginStatusTexture = textToTexture(renderer,
                                                      sans,
                                                      loginStatusText.c_str(),
                                                      white);
      SDL_Rect loginStatusRect { 500, 500, 150, 100 };
      
      SDL_Texture* loginButtonTexture = textToTexture(renderer,
                                                      sans,
                                                      "login",
                                                      white);
      
      SDL_Rect loginButtonRect { 500, 300, 150, 100 };

      SDL_Texture* logoutTextTexture = textToTexture(renderer,
                                                       sans,
                                                       "logout",
                                                       white);

      SDL_Rect logoutButtonRect { 600, 500, 150, 100};
      
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
      
      SDL_Rect textRect;
      textRect.x = 0;
      textRect.y = 0;
      textRect.w = 200;
      textRect.h = 100;
      
      screenSurface = SDL_GetWindowSurface(window);
      
      /*       SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00) ); */

      bool running = true;      
      
      while (running) {

        /* START EVENT HANDLING */
        if (SDL_PollEvent(&event)) {
          switch (event.type) {
          case SDL_QUIT: 
            running = false;
            break;
          case SDL_KEYDOWN:

            lo.info(std::string("selectedTextbox: ") + state.selectedTextbox);

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


            /* START PLAYING INPUT */
            if (state.currentView == "playing_character") {
              lo.info("playing character key event");
              lo.info(std::to_string(event.key.keysym.sym));

              switch (event.key.keysym.sym) {
              case SDLK_DOWN:
                lo.info("down arrow");
                break;
              case SDLK_UP:
                lo.info("up arrow");
                break;
              case SDLK_LEFT:
                lo.info("left arrow");
                break;
              case SDLK_RIGHT:
                lo.info("right arrow");
                break;
              case 'f':
                fullscreen = !fullscreen;
                if (fullscreen) {
                  SDL_SetWindowFullscreen(window, windowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                } else {
                  SDL_SetWindowFullscreen(window, windowFlags);
                }
                break;
              default:
                lo.info("key not handled");
                break;
              }
            }
            /* END PLAYING INPUT */
            
            
            break;
          case SDL_TEXTINPUT:

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
            
            std::cout << "composition: " << event.edit.text << event.edit.start << event.edit.length << std::endl;
            break;

          case SDL_MOUSEBUTTONDOWN:

            int mousePosX, mousePosY = 0;

            lo.info(state.currentView);

            SDL_GetMouseState(&mousePosX, &mousePosY);

            auto clickedPoint = Point { mousePosX, mousePosY };

            bool loginButtonCollision = checkForCollision(clickedPoint, &loginButtonRect);

            bool usernameTextboxClicked = checkForCollision(clickedPoint, &loginUsernameRect);
            bool passwordTextboxClicked = checkForCollision(clickedPoint, &loginPasswordRect);

            bool logoutTextboxClicked = checkForCollision(clickedPoint, &logoutButtonRect);

            
            SDL_Rect firstCharacterTextRect { 10, 50, 200, 75 };
            
            bool characterClicked = checkForCollision(clickedPoint, &firstCharacterTextRect) &&
              state.currentView == "character_screen";

            if (characterClicked) {

              // TODO: Get which character was clicked, implement box highlighting
              // and ability to select with arrows and enter
              lo.info("character selected");

              state.currentView = "playing_character";
            }

            if (logoutTextboxClicked && state.currentView == "character_screen") {
              
              lo.info("logout clicked");

              ServerResponse serverResponse = sendLogoutRequest(s, state.username);

              switch (serverResponse.status) {
              case 200:                
                state.currentView = "login";
                
                loginStatusText = "Logout success";
                break;
                
              default:
                break;
              }


            } else if (usernameTextboxClicked && state.currentView == "login") {
              state.selectedTextbox = "username";
            } else if (passwordTextboxClicked) {
              state.selectedTextbox = "password";
            } else if (loginButtonCollision && state.currentView == "login") {

              ServerResponse serverResponse = sendLoginRequest(s,
                                                               loginUsernameText,
                                                               loginPasswordText);

              std::string accessToken = "";
              std::string username = "";
              json responseBodyParsed;
              std::tuple<bool, json> jsonParseResult;
                            
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

                  ServerResponse charactersResponse = sendGetCharactersRequest(s,
                                                                       username,
                                                                       accessToken);

                  lo.info("get characters response");
                  lo.info(std::to_string(charactersResponse.status));

                  lo.info(charactersResponse.body);
                  
                  json parsed = json::parse(charactersResponse.body);

                  std::vector<Character> characters;
                  
                  for (auto& elem : parsed["characters"]) {
                    auto character = Character {
                      elem["class"],
                      elem["id"],
                      (int) elem["level"],
                      (unsigned int) elem["experience"],
                      elem["name"],
                      elem["race"]
                    };

                    std::cout << character.characterName << ", " << character.level << ", " << character.experience << std::endl;

                    characters.push_back(character);
                    
                  }

                  state.characters = characters;
                  
                }

                state.currentView = "character_screen";
                
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

            

            // TODO: Check for mouse click collision with the login button
            std::cout << "mouse click: " << event.button.button << std::endl;

            
          }
          
        } /* END EVENT HANDLING */
        

        
        /*         SDL_RenderClear(renderer); */
                
        switch( hash(state.currentView.c_str())) {
          
        case hash("login"): 
          
          SDL_RenderCopy(renderer, loginTextTexture, NULL, &textRect);
          SDL_RenderCopy(renderer, loginStatusTexture, NULL, &loginStatusRect);
          SDL_RenderCopy(renderer, loginUsernameTexture, NULL, &loginUsernameRect);
          SDL_RenderCopy(renderer, loginPasswordTexture, NULL, &loginPasswordRect);
          SDL_RenderCopy(renderer, loginButtonTexture, NULL, &loginButtonRect);
          
          break;
        
          
        case hash("character_screen"):
          
          renderChooseCharacterScreen(renderer,
                                sans,
                                white,
                                logoutTextTexture,
                                &logoutButtonRect,
                                state.characters);
          
          break;

        case hash("playing_character"):

          /* renderPlayerUI(renderer,
                         sans,
                         white,
                         &state);
          
          renderPlayingCharacterScreen(renderer,
                                       sans,
                                       white,
                                       &state); */

          glViewport(0, 0, windowWidth, windowHeight);
          glClearColor(1.f, 0.f, 1.f, 0.f);
          glClear(GL_COLOR_BUFFER_BIT);

          SDL_GL_SwapWindow(window);
                                       
          
        
        default:
          break;
        }

        /* SDL_RenderPresent(renderer);         */

      }

      SDL_StopTextInput();
      SDL_FreeSurface(screenSurface);
      SDL_DestroyTexture(loginTextTexture);
      SDL_DestroyTexture(loginStatusTexture);
      SDL_DestroyTexture(loginUsernameTexture);
      SDL_DestroyTexture(loginPasswordTexture);
      SDL_DestroyTexture(loginButtonTexture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      closesocket(s);
      WSACleanup();
    }
  }

  return 0;
}
