/**
 * \file
 * \brief Contains UDP server implementation.
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

class UdpServer {
  static constexpr size_t kBufferSize = 508;
 
 public:
  // Creates socket and binds it to the provided address.
  UdpServer(const in_addr& ip, in_port_t port) {
    buffer = new char[kBufferSize];

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(server_socket > -1, "Failed to create passive socket.\n");

    sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr = ip;
    addr.sin_port = port;

    int error = bind(server_socket, (sockaddr*)(&addr), sizeof(addr));
    ASSERT(error == 0, "Failed to bind server socket.\n");
  }

  // Non-Copyable and Non-Movable
  UdpServer(const UdpServer&) = delete;
  UdpServer& operator=(const UdpServer&) = delete;

  // Waits for message from random client and sends him answer.
  void ReceiveSend() {
    sockaddr_in addr = {};
    socklen_t addr_len = sizeof(sockaddr_in);

    int error = recvfrom(server_socket, buffer, kBufferSize, 0, (sockaddr*)(&addr), &addr_len);
    ASSERT(error > -1, "Failed to receive message.\n");

    PrintMessage("Client", addr, buffer);

    printf("You: ");
    EnterMessage(buffer, kBufferSize);

    error = sendto(server_socket, buffer, strlen(buffer) + 1, 0, (sockaddr*)(&addr), addr_len);
    ASSERT(error > -1, "Failed to send message.\n");
  }

  // Deletes buffer and closes server buffer.
  ~UdpServer() {
    delete[] buffer;
    
    ASSERT(close(server_socket) == 0, "Failed to close.\n");
  }

 private:
  int server_socket = -1;
  char* buffer = nullptr;
};

// ============================================================================

bool Action() {
  char str[32];

  while (true) {
    printf("Enter 'stop' to stop server, 'listen' to continue: ");

    ASSERT(scanf("%31s", str) == 1, "Invalid action.\n");
    getchar();

    if (strcmp(str, "stop") == 0) {
      return false;
    }

    if (strcmp(str, "listen") == 0) {
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

  in_addr_t server_addr = 0;
  ASSERT(inet_pton(AF_INET, argv[1], &server_addr), "Invalid IP.\n");
  
  in_port_t server_port = atoi(argv[2]);
  server_port = htons(server_port);

  UdpServer server({server_addr}, server_port);

  while (Action()) {
    server.ReceiveSend();
  }

  return 0;
}
