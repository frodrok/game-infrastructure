#include <stdio.h>
#include <winsock2.h>

#define SERVER "127.0.0.1"
#define BUFLEN 1024
#define PORT 8888

int main(int argc, char** argv) {
  
  struct sockaddr_in si_other;
  
  int s, slen = sizeof(si_other);

  char buf[BUFLEN];
  char message[BUFLEN];

  WSADATA wsa;

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    printf("faled. error code: %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if ( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
    printf("socket() failed %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  gets(message);

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

  memset(buf, '\0', BUFLEN);

  if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR) {
    printf("recvfrom() failed %d", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  closesocket(s);
  WSACleanup();

  return 0;
}
