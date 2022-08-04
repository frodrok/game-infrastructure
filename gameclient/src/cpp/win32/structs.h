struct ServerResponse {
  int status;
  std::string body;
};

struct ClientState {
  std::string username;
  std::string accessToken;
  std::string selectedTextbox;
  std::string currentView;
};

// where have i seen this before?
struct Point {
  int x, y;
};
