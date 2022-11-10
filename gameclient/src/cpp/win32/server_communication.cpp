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

ServerResponse sendLogoutRequest(int s,
                                 std::string username) {

  std::cout << "logout request username " << username << std::endl;

  struct sockaddr_in si_other;
  int slen = sizeof(si_other);
  memset( (char*) &si_other, 0, sizeof(si_other));

  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

  json logoutMessageJson;
  logoutMessageJson["type"] = "logout";
  logoutMessageJson["username"] = username;

  auto messageStr = logoutMessageJson.dump();

  int result = sendUdpPacket(s,
                             (struct sockaddr *) &si_other,
                             messageStr.c_str(),
                             slen);

  if (result > 0) {
    ServerResponse response {0, "error"};
    return response;
  }

  std::string packet = receiveUdpPacket(s, (struct sockaddr *) &si_other,
                                        &slen);

  std::cout << "received packet: " << packet << std::endl;

  json receivedParsed = json::parse(packet);

  std::cout << "received logout result: " << receivedParsed["status"] << std::endl;

  auto response = ServerResponse { (int) receivedParsed["status"], "" };

  return response;
    
}

ServerResponse sendGetCharactersRequest(int s,
                                        std::string username,
                                        std::string accessToken) {

  std::cout << "get characters request username " << username << std::endl;

  struct sockaddr_in si_other;
  int slen = sizeof(si_other);
  memset( (char*) &si_other, 0, sizeof(si_other));

  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

  json getCharactersMessageJson;
  getCharactersMessageJson["type"] = "getCharacters";
  getCharactersMessageJson["username"] = username;
  getCharactersMessageJson["accessToken"] = accessToken;

  auto messageStr = getCharactersMessageJson.dump();

  int result = sendUdpPacket(s,
                             (struct sockaddr *) &si_other,
                             messageStr.c_str(),
                             slen);

  if (result > 0) {
    ServerResponse response {0, "error"};
    return response;
  }

  std::string packet = receiveUdpPacket(s, (struct sockaddr *) &si_other,
                                        &slen);

  std::cout << "received packet: " << packet << std::endl;

  json receivedParsed = json::parse(packet);

  std::cout << "received logout result: " << receivedParsed["status"] << std::endl;

  auto response = ServerResponse { (int) receivedParsed["status"],
                                   receivedParsed["body"] };

  return response;
 
}
