/**
 * QOTD Server with Fortune and Cowsay
 * Implements RFC 865 on TCP/UDP port 17
 * MIT License (c) 2025 Jack Kingsman <jack@jackkingsman.me>
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define QOTD_PORT 17
#define BACKLOG 10
#define MAX_BUFFER_SIZE 512
#define QUOTE_COMMAND "fortune | cowsay"
#define NUM_FORTUNES 20
#define MODE '8'
#define QUOTES_FILE "./quotes.txt"
#define MAX_QUOTES 64

const char *fortunes[NUM_FORTUNES] = {"It is certain.",
                                      "It is decidedly so.",
                                      "Without a doubt.",
                                      "Yes - definitely.",
                                      "You may rely on it.",
                                      "As I see it, yes.",
                                      "Most likely.",
                                      "Outlook good.",
                                      "Yes.",
                                      "Signs point to yes.",
                                      "Reply hazy, try again.",
                                      "Ask again later.",
                                      "Better not tell you now.",
                                      "Cannot predict now.",
                                      "Concentrate and ask again.",
                                      "Don't count on it.",
                                      "My reply is no.",
                                      "My sources say no.",
                                      "Outlook not so good.",
                                      "Very doubtful."};

int tcp_socket = -1;
int udp_socket = -1;

// Global variables for file quotes
char **file_quotes = NULL;
int file_quote_count = 0;
char server_mode = '8'; // Default to 8 ball mode

// Function declarations
char **read_file(const char *filename, int *count);
void init_server_config();

void handle_signal(int sig) {
  (void)sig; // Suppress unused parameter warning
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

void init_server_config() {
  // Get mode from environment variable
  const char *mode_env = getenv("QOTD_MODE");
  if (mode_env) {
    if (strcmp(mode_env, "file") == 0) {
      server_mode = 'f';
    } else if (strcmp(mode_env, "8ball") == 0) {
      server_mode = '8';
    } else if (strcmp(mode_env, "command") == 0) {
      server_mode = 'c';
    }
  }

  // Load file quotes if in file mode
  if (server_mode == 'f') {
    const char *quotes_file_env = getenv("QUOTES_FILE");
    const char *quotes_file = quotes_file_env ? quotes_file_env : QUOTES_FILE;

    printf("[%ld] Loading quotes from: %s\n", time(NULL), quotes_file);
    file_quotes = read_file(quotes_file, &file_quote_count);

    if (file_quotes) {
      printf("[%ld] Loaded %d quotes from file\n", time(NULL),
             file_quote_count);
    } else {
      printf("[%ld] Failed to load quotes file, falling back to 8ball mode\n",
             time(NULL));
      server_mode = '8';
    }
  }
}

char *get_fortune(char *buffer, size_t buffer_size) {

  int fortune_index = rand() % NUM_FORTUNES;
  const char *fortune = fortunes[fortune_index];
  snprintf(buffer, buffer_size, "%s", fortune);
  return buffer;
}

char *get_command_output(char *buffer, size_t buffer_size) {
  FILE *pipe = popen(QUOTE_COMMAND, "r");
  if (!pipe) {
    snprintf(buffer, buffer_size, "Error: Command execution failed\n");
    return buffer;
  }

  size_t total_read = 0;
  size_t bytes_read;

  while (total_read < buffer_size - 1 &&
         (bytes_read = fread(buffer + total_read, 1,
                             buffer_size - total_read - 1, pipe)) > 0) {
    total_read += bytes_read;
  }

  buffer[total_read] = '\0';
  pclose(pipe);
  return buffer;
}

char **read_file(const char *filename, int *count) {
  FILE *fquotes;

  fquotes = fopen(filename, "r");
  if (fquotes == NULL) {
    printf("Unable to open quotes file: %s\n", filename);
    return NULL;
  }

  int num_quotes = 0;
  char buffer[MAX_BUFFER_SIZE];

  // First pass: count lines
  while (fgets(buffer, sizeof(buffer), fquotes)) {
    num_quotes++;
  }

  if (num_quotes == 0) {
    fclose(fquotes);
    printf("No quotes found in file\n");
    return NULL;
  }

  char **quotes = malloc(num_quotes * sizeof(char *));
  if (!quotes) {
    fclose(fquotes);
    return NULL;
  }

  rewind(fquotes);
  int i = 0;
  while (fgets(buffer, sizeof(buffer), fquotes) && i < num_quotes) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    quotes[i] = malloc(strlen(buffer) + 1);
    if (quotes[i]) {
      strcpy(quotes[i], buffer);
      i++;
    }
  }

  fclose(fquotes);
  *count = i;
  return quotes;
}

char *get_file_quote(char *buffer, size_t buffer_size, char **quotes,
                     int num_quotes) {
  if (!quotes || num_quotes <= 0) {
    snprintf(buffer, buffer_size, "Error: No quotes available\n");
    return buffer;
  }

  int quote_index = rand() % num_quotes;
  const char *quote = quotes[quote_index];
  snprintf(buffer, buffer_size, "%s", quote);
  return buffer;
}

char *get_quote(char *buffer, size_t buffer_size, char mode) {
  switch (mode) {
  case 'c':
    return get_command_output(buffer, buffer_size);
  case '8':
    return get_fortune(buffer, buffer_size);
  case 'f':
    return get_file_quote(buffer, buffer_size, file_quotes, file_quote_count);
  default:
    return get_fortune(buffer, buffer_size);
  }
}

int main() {
  setbuf(stdout, NULL);

  char quote_buffer[MAX_BUFFER_SIZE];
  struct sockaddr_in tcp_addr, udp_addr;
  int opt = 1;

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Initialize server configuration
  init_server_config();

  // wear your socks
  if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("TCP socket creation failed");
    exit(EXIT_FAILURE);
  }

  if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("UDP socket creation failed");
    close(tcp_socket);
    exit(EXIT_FAILURE);
  }

  if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 ||
      setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  memset(&tcp_addr, 0, sizeof(tcp_addr));
  tcp_addr.sin_family = AF_INET;
  tcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  tcp_addr.sin_port = htons(QOTD_PORT);
  memcpy(&udp_addr, &tcp_addr, sizeof(tcp_addr));

  // tcp bind
  if (bind(tcp_socket, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) < 0) {
    perror("TCP bind failed");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  if (listen(tcp_socket, BACKLOG) < 0) {
    perror("TCP listen failed");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  // udp bind
  if (bind(udp_socket, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
    perror("UDP bind failed");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  // non-blocking socks
  int flags;
  flags = fcntl(tcp_socket, F_GETFL, 0);
  fcntl(tcp_socket, F_SETFL, flags | O_NONBLOCK);
  flags = fcntl(udp_socket, F_GETFL, 0);
  fcntl(udp_socket, F_SETFL, flags | O_NONBLOCK);

  printf("[%ld] QOTD server started on port %d (TCP and UDP)\n", time(NULL),
         QOTD_PORT);

  // main loop
  while (1) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(tcp_socket, &read_fds);
    FD_SET(udp_socket, &read_fds);

    int max_fd = (tcp_socket > udp_socket) ? tcp_socket : udp_socket;

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
      if (errno == EINTR)
        continue;
      perror("select failed");
      break;
    }

    // tcp
    if (FD_ISSET(tcp_socket, &read_fds)) {
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);

      int client_fd =
          accept(tcp_socket, (struct sockaddr *)&client_addr, &client_len);
      if (client_fd >= 0) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[%ld] TCP connection from %s:%d\n", time(NULL), client_ip,
               ntohs(client_addr.sin_port));

        // send quote
        get_quote(quote_buffer, sizeof(quote_buffer), server_mode);
        send(client_fd, quote_buffer, strlen(quote_buffer), 0);
        close(client_fd);
      } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("TCP accept failed");
      }
    }

    // udp
    if (FD_ISSET(udp_socket, &read_fds)) {
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);
      char recv_buffer[MAX_BUFFER_SIZE];

      ssize_t recv_size =
          recvfrom(udp_socket, recv_buffer, sizeof(recv_buffer), 0,
                   (struct sockaddr *)&client_addr, &client_len);

      if (recv_size >= 0) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[%ld] UDP datagram from %s:%d\n", time(NULL), client_ip,
               ntohs(client_addr.sin_port));

        // send quote
        get_quote(quote_buffer, sizeof(quote_buffer), server_mode);
        sendto(udp_socket, quote_buffer, strlen(quote_buffer), 0,
               (struct sockaddr *)&client_addr, client_len);
      } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("UDP recvfrom failed");
      }
    }
  }

  close(tcp_socket);
  close(udp_socket);

  return EXIT_SUCCESS;
}
