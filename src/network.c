/**
 * Network server functions for QOTD Server
 * MIT License (c) 2026 Jack Kingsman <jack@jackkingsman.me>
 * MIT License (c) 2026 Julia Barbera <julia@fishcat.fish>
 */

#include "qotd.h"

int setup_tcp_server() {
  struct sockaddr_in tcp_addr;
  int opt = 1;

  if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("TCP socket creation failed");
    return -1;
  }

  if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("TCP setsockopt failed");
    close(tcp_socket);
    return -1;
  }

  memset(&tcp_addr, 0, sizeof(tcp_addr));
  tcp_addr.sin_family = AF_INET;
  tcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  tcp_addr.sin_port = htons(QOTD_PORT);

  if (bind(tcp_socket, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) < 0) {
    perror("TCP bind failed");
    close(tcp_socket);
    return -1;
  }

  if (listen(tcp_socket, BACKLOG) < 0) {
    perror("TCP listen failed");
    close(tcp_socket);
    return -1;
  }

  // Set non-blocking
  int flags = fcntl(tcp_socket, F_GETFL, 0);
  fcntl(tcp_socket, F_SETFL, flags | O_NONBLOCK);

  return 0;
}

int setup_udp_server() {
  struct sockaddr_in udp_addr;
  int opt = 1;

  if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("UDP socket creation failed");
    return -1;
  }

  if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("UDP setsockopt failed");
    close(udp_socket);
    return -1;
  }

  memset(&udp_addr, 0, sizeof(udp_addr));
  udp_addr.sin_family = AF_INET;
  udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  udp_addr.sin_port = htons(QOTD_PORT);

  if (bind(udp_socket, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
    perror("UDP bind failed");
    close(udp_socket);
    return -1;
  }

  // Set non-blocking
  int flags = fcntl(udp_socket, F_GETFL, 0);
  fcntl(udp_socket, F_SETFL, flags | O_NONBLOCK);

  return 0;
}

uint32_t ip_cast(char *str_ip) {
  uint32_t int_ip;
  if (inet_pton(AF_INET, str_ip, &int_ip) <= 0) {
    printf("Junk IP: %s\n", str_ip);
    return 0;
  }
  printf("Good IP: %s to %u\n", str_ip, int_ip);
  return int_ip;
}

void handle_tcp_connection() {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  char quote_buffer[MAX_BUFFER_SIZE];

  int client_fd =
      accept(tcp_socket, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd >= 0) {
    char client_ip[INET_ADDRSTRLEN];
    if (!rate_allow(ip_cast(client_ip)))
      return;
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("[%ld] TCP connection from %s:%d\n", time(NULL), client_ip,
           ntohs(client_addr.sin_port));

    // Send quote
    get_quote(quote_buffer, sizeof(quote_buffer), server_mode);
    send(client_fd, quote_buffer, strlen(quote_buffer), 0);
    close(client_fd);
  } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
    perror("TCP accept failed");
  }
}

void handle_udp_request() {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  char recv_buffer[MAX_BUFFER_SIZE];
  char quote_buffer[MAX_BUFFER_SIZE];

  ssize_t recv_size = recvfrom(udp_socket, recv_buffer, sizeof(recv_buffer), 0,
                               (struct sockaddr *)&client_addr, &client_len);

  if (recv_size >= 0) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("[%ld] UDP datagram from %s:%d\n", time(NULL), client_ip,
           ntohs(client_addr.sin_port));

    // Send quote
    get_quote(quote_buffer, sizeof(quote_buffer), server_mode);
    sendto(udp_socket, quote_buffer, strlen(quote_buffer), 0,
           (struct sockaddr *)&client_addr, client_len);
  } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
    perror("UDP recvfrom failed");
  }
}
