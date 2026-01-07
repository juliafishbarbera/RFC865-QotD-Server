/**
 * Server configuration functions for QOTD Server
 * MIT License (c) 2026 Julia Barbera <julia@fishcat.fish>
 */

#include "qotd.h"

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

  const char *net_env = getenv("QOTD_NET");
  if (net_env) {
    if (strcmp(net_env, "udp_tcp") == 0) {
      udp_en = 1;
      tcp_en = 1;
    } else if (strcmp(net_env, "tcp_udp") == 0) {
      udp_en = 1;
      tcp_en = 1;
    } else if (strcmp(net_env, "tcp") == 0) {
      tcp_en = 1;
    } else if (strcmp(net_env, "udp") == 0) {
      udp_en = 1;
    } else {
      tcp_en = 0;
      udp_en = 0;
    }
  } else {
    tcp_en = 1;
    udp_en = 1;
    printf("[%ld] QOTD_NET unset! Defaulting to TCP and UDP.\n", time(NULL));
  }

  const char *qotd_prefix = getenv("QOTD_PREFIX");
  if (qotd_prefix) {
    prefix = qotd_prefix;
  }
  const char *qotd_suffix = getenv("QOTD_SUFFIX");
  if (qotd_suffix) {
    suffix = qotd_suffix;
  }

  const char *qotd_command = getenv("QOTD_COMMAND");
  if (qotd_command) {
    quote_command = qotd_command;
  } else {
    quote_command = "echo \"I didn't set a quote command!\"";
  }

  const char *qotd_rate = getenv("QOTD_RATE");
  if (qotd_rate) {
    int tps = atoi(qotd_rate);
    if (tps < 1 || tps > 1024)
      tps = 16;
    tokens_per_second = tps;
  }

  const char *qotd_burst = getenv("QOTD_BURST");
  if (qotd_burst) {
    int burst = atoi(qotd_burst);
    if (burst < 1 || burst > 2048)
      burst = 64;
    burst_size = burst;
  }

  // Load file quotes if in file mode
  if (server_mode == 'f') {
    const char *quotes_file_env = getenv("QOTD_FILE");
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
