/**
 * \file
 * \brief Contains TCP server implementation.
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

template <size_t BufferSize>
class TcpServer{
  static constexpr int kClientQueueSize = 5;
 
 public:
  // Creates socket and sets it to listening state.
  TcpServer(const in_addr& ip, in_port_t port) {
    buffer = new char[BufferSize];

    passive_socket = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(passive_socket > -1, "Failed to create passive socket.\n");

    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr = ip;
    addr.sin_port = port;

    int error = bind(passive_socket, (sockaddr*)(&addr), sizeof(addr));
    ASSERT(error == 0, "Failed to bind passive socket.\n");

    error = listen(passive_socket, kClientQueueSize);
    ASSERT(error == 0, "Failed to listen on passive socket.\n");
  }

  // Non-Copyable and Non-Movable
  TcpServer(const TcpServer&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;

  // Blocks until establish connection with next client in queue.
  void AcceptNext() {
    // Close previous connection
    if (client_socket > -1) {
      ASSERT(close(client_socket) == 0, "Failed to close socket.\n");
    }

    // Accept new client
    sockaddr_in addr = {};
    socklen_t addr_len = 0;

    client_socket = accept(passive_socket, (sockaddr*)(&addr), &addr_len);
    ASSERT(client_socket > - 1, "Failed to accept connection.\n");
    ASSERT(addr_len > 0, "Invalid addr len.\n");

    printf("Client [");
    PrintSocketAddress(addr);
    printf("] connected.\n");

    ReceiveTest();
  }

  // Prints message from client and sends response entered from console.
  // Returns false if client dropped connection.
  bool ReceiveSend() {
    if (client_socket == -1) {
      return false;
    }

    int error = recv(client_socket, buffer, BufferSize, 0);
    ASSERT(error > -1, "Failed to receive message.\n");

    if (error == 0) {
      printf("Client disconnected.\n");
      Disconnect();
      return false;
    }

    PrintMessage("Client", buffer);

    printf("You: ");
    EnterMessage(buffer, BufferSize - 1);

    error = send(client_socket, buffer, strlen(buffer) + 1, 0);
    ASSERT(error > -1, "Failed to send message.\n");

    return true;
  }

  ~TcpServer() {
    delete[] buffer;
    
    ASSERT(close(passive_socket) == 0, "Failed to close.\n");

    Disconnect();
  }

 private:
  void ReceiveTest() {
    printf("Wait for test message with size of %lu bytes.\n", BufferSize);

    int error = recv(client_socket, buffer, BufferSize, 0);
    ASSERT(error > 0, "Failed to receive message.\n");

    error = send(client_socket, buffer, BufferSize, 0);
    ASSERT(error > -1, "Failed to send message.\n");

    printf("Test passed.\n");
  }

  void Disconnect() {
    if (client_socket == -1) {
      return;
    }

    ASSERT(close(client_socket) == 0, "Failed to close.\n");
    client_socket = -1;
  }

  int passive_socket = -1;
  int client_socket = -1;
  char* buffer = nullptr;
};

// ============================================================================

bool Action() {
  char str[32];

  while (true) {
    printf("Enter 'stop' to stop server, 'next' to accept next: ");

    ASSERT(scanf("%31s", str) == 1, "Invalid action.\n");
    getchar();

    if (strcmp(str, "stop") == 0) {
      return false;
    }

    if (strcmp(str, "next") == 0) {
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

  TcpServer<20 * 1024> server({server_addr}, server_port);

  while (Action()) {
    server.AcceptNext();
    while (server.ReceiveSend());
  }

  return 0;
}
