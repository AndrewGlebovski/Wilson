/**
 * \file
 * \brief Contains TCP client implementation.
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
class TcpClient {
 public:
  // Creates socket and binds it to the provided address.
  TcpClient(const in_addr& ip, in_port_t port) {
    buffer = new char[BufferSize];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(server_socket > -1, "Failed to create server socket.\n");

    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr = ip;
    addr.sin_port = port;

    int error = bind(server_socket, (sockaddr*)(&addr), sizeof(addr));
    ASSERT(error == 0, "Failed to bind server socket.\n");
  }

  // Non-Copyable and Non-Movable
  TcpClient(const TcpClient&) = delete;
  TcpClient& operator=(const TcpClient&) = delete;

  bool Connect(const in_addr& ip, in_port_t port) {
    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr = ip;
    addr.sin_port = port;

    int error = connect(server_socket, (sockaddr*)(&addr), sizeof(sockaddr_in));
    
    if (error == 0) {
      SendTest();
      return true;
    }

    return false;
  }

  // Sends user message then waits for response.
  // Returns false if server disconnected.
  bool SendReceive() {
    if (server_socket == -1) {
      return false;
    }

    printf("You: ");
    EnterMessage(buffer, BufferSize);

    int error = send(server_socket, buffer, strlen(buffer) + 1, 0);
    ASSERT(error > -1, "Failed to send message.\n");

    error = recv(server_socket, buffer, BufferSize, 0);
    ASSERT(error > -1, "Failed to receive message.\n");

    if (error == 0) {
      printf("Server disconnected.\n");
      Disconnect();
      return false;
    }

    PrintMessage("Server", buffer);
    return true;
  }

  ~TcpClient() {
    delete[] buffer;
    Disconnect();
  }

 private:
  void SendTest() {
    printf("Send test message with size of %lu bytes.\n", BufferSize);

    char value = rand() % 128;

    for (size_t i = 0; i < BufferSize; i++) {
      buffer[i] = value;
    }

    int error = send(server_socket, buffer, BufferSize, 0);
    ASSERT(error > -1, "Failed to send message.\n");

    error = recv(server_socket, buffer, BufferSize, 0);
    ASSERT(error > 0, "Failed to receive message.\n");

    for (size_t i = 0; i < BufferSize; i++) {
      ASSERT(buffer[i] == value, "Value changed.");
    }

    printf("Test passed.\n");
  }

  void Disconnect() {
    if (server_socket == -1) {
      return;
    }

    ASSERT(close(server_socket) == 0, "Failed to close.\n");
    server_socket = -1;
  }

  int server_socket = -1;
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
  if (argc != 5) {
    printf("Usage: local_ip local_port server_ip server_port\n");
    exit(1);
  }

  in_addr_t laddr = 0;
  ASSERT(inet_pton(AF_INET, argv[1], &laddr), "Invalid IP.\n");
  
  in_port_t lport = atoi(argv[2]);
  lport = htons(lport);

  in_addr_t saddr = 0;
  ASSERT(inet_pton(AF_INET, argv[3], &saddr), "Invalid IP.\n");
  
  in_port_t sport = atoi(argv[4]);
  sport = htons(sport);

  TcpClient<20 * 1024> client({laddr}, lport);

  if (!client.Connect({saddr}, sport)) {
    printf("Failed to connect to server.\n");
    return 1;
  }

  while (Action()) {
    client.SendReceive();
  }
  
  printf("Disconnected.\n");

  return 0;
}
