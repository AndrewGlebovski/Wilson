/**
 * \file
 * \brief Contains UDP client implementation.
*/

// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/ip.h>     // Socket API.
#include <arpa/inet.h>      // IP converting functions.

#include <utils/utils.hpp>

// ============================================================================

class UdpClient {
  static constexpr size_t kBufferSize = 508;
 
 public:
  // Creates socket.
  UdpClient() {
    buffer = new char[kBufferSize];

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(client_socket > -1, "Failed to create client socket.\n");
  }

  // Non-Copyable and Non-Movable
  UdpClient(const UdpClient&) = delete;
  UdpClient& operator=(const UdpClient&) = delete;

  // Sends user message then waits for response.
  void SendReceive(in_addr_t ip, in_port_t port) {
    printf("You: ");
    EnterMessage(buffer, kBufferSize);

    sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr = {ip};
    addr.sin_port = port;

    int error = sendto(client_socket, buffer, strlen(buffer) + 1, 0, (sockaddr*)(&addr), sizeof(addr));
    ASSERT(error > -1, "Failed to send message.\n");

    error = recv(client_socket, buffer, kBufferSize, 0);
    ASSERT(error > -1, "Failed to receive response.\n");

    PrintMessage("Server", addr, buffer);
  }

  ~UdpClient() {
    delete[] buffer;
    
    ASSERT(close(client_socket) == 0, "Failed to close client socket.");
  }

 private:
  int client_socket = -1;
  char* buffer = nullptr;
};

// ============================================================================

bool Action() {
  char str[32];

  while (true) {
    printf("Enter 'quit' to disconnect, 'msg' to send message: ");

    ASSERT(scanf("%31s", str) == 1, "Invalid action.\n");
    getchar();

    if (strcmp(str, "quit") == 0) {
      return false;
    }

    if (strcmp(str, "msg") == 0) {
      return true;
    }

    printf("Unknown action.\n");
  }
}

// ============================================================================

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: server_ip server_port\n");
    exit(1);
  }

  in_addr_t saddr = 0;
  ASSERT(inet_pton(AF_INET, argv[1], &saddr), "Invalid IP.\n");
  
  in_port_t sport = atoi(argv[2]);
  sport = htons(sport);

  UdpClient client;
  
  while (Action()) {
    client.SendReceive(saddr, sport);
  }
  
  printf("Disconnected.\n");

  return 0;
}
