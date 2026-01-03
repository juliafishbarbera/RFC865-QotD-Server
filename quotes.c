/**
 * Quote generation functions for QOTD Server
 * MIT License (c) 2025 Jack Kingsman <jack@jackkingsman.me>
 */

#include "qotd.h"

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