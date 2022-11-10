struct ServerResponse {
  int status;
  std::string body;
};

struct CharacterStats {
  Uint32 strength, stamina, agility, intellect, spirit;
  Uint32 health, mana;
};

struct Character {
  std::string className;
  std::string id;
  int level;
  unsigned int experience;
  std::string characterName;
  std::string race;
  CharacterStats stats;
};

struct Point {
  int32 x, y;
};

struct Vec2 {
  float32 x, y;
  uint32 w, h;  
};

struct Vec3 {
  float32 x, y, z;
  uint32 w, h, d;
};

struct ClientState {
  std::string username;
  std::string accessToken;
  std::string selectedTextbox;
  std::string currentView;
  std::vector<Character> characters;
  std::optional<Character> currentCharacter;
  Point currentPlayerPosition;  
};


