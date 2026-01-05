/**
 * QOTD Server with Fortune and Cowsay
 * Implements RFC 865 on TCP/UDP port 17
 * MIT License (c)  2026 Julia Barbera <julia@fishcat.fish>
 */

#ifndef QOTD_H
#define QOTD_H

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
#define NUM_FORTUNES 20
#define MODE '8'
#define QUOTES_FILE "./quotes.txt"
#define MAX_QUOTES 64

extern int tcp_socket;
extern int udp_socket;
extern char **file_quotes;
extern int file_quote_count;
extern char server_mode;
extern int tcp_en;
extern int udp_en;
extern char *quote_command;
extern char *prefix;
extern char *suffix;

extern const char *fortunes[NUM_FORTUNES];

char **read_file(const char *filename, int *count);
void init_server_config();
void handle_signal(int sig);
char *get_fortune(char *buffer, size_t buffer_size);
char *get_command_output(char *buffer, size_t buffer_size);
char *get_file_quote(char *buffer, size_t buffer_size, char **quotes,
                     int num_quotes);
char *get_quote(char *buffer, size_t buffer_size, char mode);
int setup_tcp_server();
int setup_udp_server();
void handle_tcp_connection();
void handle_udp_request();

#endif
