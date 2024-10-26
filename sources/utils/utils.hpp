/**
 * \file
 * \brief Contains utility functions declaration.
*/

#pragma once

// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>

// ============================================================================

#define ASSERT(condition, ...)  \
do {                            \
  if (!(condition)) {           \
    perror("Error");            \
    printf(__VA_ARGS__);        \
    exit(1);                    \
  }                             \
} while(0)

// ============================================================================

// Prints IP and Port in "ip:port" format.
void PrintSocketAddress(const sockaddr_in& sockaddr);

// Prints message in "entry: msg" format. 
void PrintMessage(const char* entry, const char* msg);

// Prints message in "entry [ip:port]: msg" format. 
void PrintMessage(const char* entry, const sockaddr_in& addr, const char* msg);

// Reads no more then len symbols from console.
// Buffer must be at least len + 1 bytes long to hold message + '\0'. 
// Returns total number of written bytes.
size_t EnterMessage(char* buffer, size_t len);
