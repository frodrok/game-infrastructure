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

//#include <GL/glew.h>
//#include <GL/glut.h>
//#include <GL/glu.h>
//#include <GL/glext.h>
//#include <glm/glm.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


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

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
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

  //  SDL_Window* window = NULL;
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

  
  int windowWidth = std::get<0>(gameResolution);
  int windowHeight = std::get<1>(gameResolution);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
  bool fullscreen = false;

  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);

  if (window == NULL) {
    lo.info("failed to create glfw window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
  // works?
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cout << "failed to initialize glad" << std::endl;
    return -1;
  }

  /* if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "failed to initialize glad" << std::endl;
    return -1;
    } */
  
  glViewport(0, 0, windowWidth, windowHeight);
  
  lo.info(std::string("after init font"));
  
  /* SDL_GLContext context = SDL_GL_CreateContext(window); */

  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
  };
  
  /* GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID); */

  unsigned int VBO_one;
  unsigned int VBO_two;
  glGenBuffers(1, &VBO_one);
  glGenBuffers(1, &VBO_two);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_one);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER: " << infoLog << std::endl;
  }

  const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

  const char* fragmentShaderYellowSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\0";

  unsigned int fragmentShader, fragmentShaderYellow;
  
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
  
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glShaderSource(fragmentShaderYellow, 1, &fragmentShaderYellowSource, NULL);

  unsigned int shaderProgramOrange;
  unsigned int shaderProgramYellow;
  shaderProgramOrange = glCreateProgram();
  shaderProgramYellow = glCreateProgram();

  glAttachShader(shaderProgramOrange, vertexShader);
  glAttachShader(shaderProgramOrange, fragmentShader);
  glLinkProgram(shaderProgramOrange);

  glGetProgramiv(shaderProgramOrange, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(shaderProgramOrange, 512, NULL, infoLog);
    std::cout << "ERROR::GL_LINK: " << infoLog << std::endl;
  }


  glDeleteShader(fragmentShader);

  glUseProgram(shaderProgramOrange);

  glAttachShader(shaderProgramYellow, vertexShader);
  glAttachShader(shaderProgramYellow, fragmentShaderYellow);
  glLinkProgram(shaderProgramYellow);

  glGetProgramiv(shaderProgramYellow, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(shaderProgramYellow, 512, NULL, infoLog);
    std::cout << "ERROR:GL_LINK: " << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);  

  unsigned int VAO_one;
  unsigned int VAO_two;
  glGenVertexArrays(1, &VAO_one);
  glGenVertexArrays(1, &VAO_two);
  
  /*  glBindVertexArray(VAO_one);
  glBindVertexArray(VAO_two); */

  // glDrawArrays(GL_TRIANGLES, 0, 3);

  float ebo_vertices[] = {
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f
  };

  unsigned int ebo_indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  /*   unsigned int EBO;

  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo_indices), ebo_indices, GL_STATIC_DRAW); */

  /*   float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
    }; */

  float first_triangle[] = {
    -0.25f, -0.25f, 0.0f,
    0.25f, -0.25f, 0.0f,
    0.0f, 0.25f, 0.0f,
  
  };

  float second_triangle[] = {
    -0.75f, -0.25f, 0.0f,
    -0.25f, -0.25f, 0.0f,
    -0.5f, 0.25f, 0.0f
  };

  glBindVertexArray(VAO_one);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_one);
  glBufferData(GL_ARRAY_BUFFER, sizeof(first_triangle), first_triangle, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(VAO_two);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_two);
  glBufferData(GL_ARRAY_BUFFER, sizeof(second_triangle), second_triangle, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);

  //  glEnableVertexAttribArray(1);

  /*  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
      glEnableVertexAttribArray(0); */

  /* Draw wireframes */
  //  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  while(!glfwWindowShouldClose(window)) {
    /* handle input */
    processInput(window);

    /* render */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgramOrange);
    glBindVertexArray(VAO_one);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);

    glUseProgram(shaderProgramYellow);
    glBindVertexArray(VAO_two);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glBindVertexArray(0);

    /* handle events and swap buffers */
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  
  SDL_StopTextInput();
  SDL_FreeSurface(screenSurface);
  
  lo.info(std::string("terminating.."));

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteVertexArrays(1, &VAO_one);
  glDeleteVertexArrays(1, &VAO_two);
  glDeleteBuffers(1, &VBO_one);
  glDeleteBuffers(1, &VBO_two);
  glfwTerminate();
  /*       SDL_DestroyTexture(loginTextTexture);
           SDL_DestroyTexture(loginStatusTexture);
           SDL_DestroyTexture(loginUsernameTexture);
           SDL_DestroyTexture(loginPasswordTexture);
           SDL_DestroyTexture(loginButtonTexture);
           SDL_DestroyRenderer(renderer); */
  //SDL_DestroyWindow(window);
  SDL_Quit();
  closesocket(s);
  WSACleanup();
  
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

