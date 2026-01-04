/**
 * QOTD Server
 * Implements RFC 865 on TCP/UDP port 17
 * MIT License (c) 2025 Jack Kingsman <jack@jackkingsman.me>
 */

#include "qotd.h"

int tcp_socket = -1;
int udp_socket = -1;

char **file_quotes = NULL;
int file_quote_count = 0;
char server_mode = '8';
int tcp_en = 0;
int udp_en = 0;
char *quote_command = NULL;

void handle_signal(int sig) {
  (void)sig;
  printf("[%ld] Shutting down QOTD server...\n", time(NULL));
  if (tcp_socket >= 0)
    close(tcp_socket);
  if (udp_socket >= 0)
    close(udp_socket);

  // Free file quotes memory
  if (file_quotes) {
    for (int i = 0; i < file_quote_count; i++) {
      if (file_quotes[i]) {
        free(file_quotes[i]);
      }
    }
    free(file_quotes);
  }

  exit(EXIT_SUCCESS);
}

int main() {
  setbuf(stdout, NULL);

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Initialize server configuration
  init_server_config();

  // Setup TCP server
  if (tcp_en) {
    if (setup_tcp_server() < 0) {
      exit(EXIT_FAILURE);
    }
  }

  // Setup UDP server
  if (udp_en) {
    if (setup_udp_server() < 0) {
      if (tcp_en)
        close(tcp_socket);
      exit(EXIT_FAILURE);
    }
  }

  printf("[%ld] QOTD server started on port %d (", time(NULL), QOTD_PORT);
  if (tcp_en && udp_en) {
    printf("TCP and UDP");
  } else if (tcp_en) {
    printf("TCP only");
  } else if (udp_en) {
    printf("UDP only");
  } else {
    printf("no protocols");
  }
  printf(") in ");
  if (server_mode == '8') {
    printf("8 ball");
  } else if (server_mode == 'f') {
    printf("file");
  } else {
    printf("command");
  }

  // Main loop
  while (1) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    if (tcp_en)
      FD_SET(tcp_socket, &read_fds);
    if (udp_en)
      FD_SET(udp_socket, &read_fds);

    int max_fd = -1;
    if (tcp_en && tcp_socket > max_fd)
      max_fd = tcp_socket;
    if (udp_en && udp_socket > max_fd)
      max_fd = udp_socket;

    if (max_fd >= 0 && select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR)
        continue;
      perror("select failed");
      break;
    }

    // Handle TCP connections
    if (tcp_en && FD_ISSET(tcp_socket, &read_fds)) {
      handle_tcp_connection();
    }

    // Handle UDP requests
    if (udp_en && FD_ISSET(udp_socket, &read_fds)) {
      handle_udp_request();
    }
  }

  if (tcp_en)
    close(tcp_socket);
  if (udp_en)
    close(udp_socket);

  return EXIT_SUCCESS;
}
