
#pragma once

#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#define BUFFER_SIZE 3000
#define LISTEN_BACKLOG 10

/*

    Creating a listening socket requires at least 3 API calls:
    Obtain a socket handle via socket().
    Set the listening IP:port via bind().
    Create the listening socket via listen().
    Then use the accept() API to wait for incoming TCP connections. Pseudo code:

    ----------------------------------------------------------------------------


    AF_INET is for IPv4. Use AF_INET6 for IPv6 or dual-stack sockets.
    SOCK_STREAM is for TCP. Use SOCK_DGRAM for UDP.
    The 3rd argument is 0 and useless for our purposes.

 */
class Server {

private:
  int port;
  int sock_fd;
  char buffer[BUFFER_SIZE] = {0};

  inline sockaddr_in set();

public:
  Server(int port) { this->port = port; };
  inline void server_create(std::string_view url, std::string_view content);
  inline void temp(int conn_fd);
};

sockaddr_in Server::set() {
  struct sockaddr_in socket_addr;

  socket_addr.sin_family = AF_INET;
  socket_addr.sin_port = htons(port);
  socket_addr.sin_addr.s_addr = INADDR_ANY;

  return socket_addr;
};

void Server::server_create(std::string_view url, std::string_view content) {

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  struct sockaddr_in addr = set();

  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));

  if (rv) {
    std::cout << "bind()" << std::endl;
    return;
  };

  // listen
  rv = listen(fd, SOMAXCONN);
  if (rv) {
    std::cout << "listen()" << std::endl;
    return;
  };

  while (true) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0) {
      continue; // error
    }

    temp(connfd);
    close(connfd);
  }
}

void Server::temp(int conn_fd) {
  char rbuf[64] = {};
  ssize_t n = read(conn_fd, rbuf, sizeof(rbuf) - 1);
  if (n < 0) {
    std::cout << "read() error" << std::endl;
    return;
  };
  fprintf(stderr, "client says: %s\n", rbuf);

  std::string wbuf = "world";
  write(conn_fd, wbuf.c_str(), wbuf.size());
};
