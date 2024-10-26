/**
 * \file
 * \brief Contains utility functions implementation.
*/

// ============================================================================

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>      // IP converting functions.

#include <utils/utils.hpp>

// ============================================================================

void PrintSocketAddress(const sockaddr_in& sockaddr) {
  char str[INET_ADDRSTRLEN] = "";
  inet_ntop(AF_INET, &sockaddr.sin_addr, str, INET_ADDRSTRLEN);

  in_port_t port = ntohs(sockaddr.sin_port);

  printf("%s:%hu", str, port);
}

void PrintMessage(const char* entry, const char* msg) {
  printf("%s: %s\n", entry, msg);
}

void PrintMessage(const char* entry, const sockaddr_in& addr, const char* msg) {
  printf("%s [", entry);
  PrintSocketAddress(addr);
  printf("]: %s\n", msg);
}

size_t EnterMessage(char* buffer, size_t len) {
  for (size_t i = 0; i < len - 1; i++) {
    char c = getchar();

    if (c == '\n' || c == EOF) {
      buffer[i] = '\0';
      return i + 1;
    }

    buffer[i] = c;
  }

  buffer[len - 1] = '\0';
  return len;
}
