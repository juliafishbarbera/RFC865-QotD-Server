# Makefile for RFC865 QOTD Server

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = 

# Targets
TARGETS = qotd_server

# Source files
SOURCES = src/main.c src/quotes.c src/network.c src/config.c
HEADERS = src/qotd.h

# Default target
all: $(TARGETS)

# Server variant
qotd_server: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(TARGETS)

# Run targets (requires sudo for privileged port)
run: qotd_server
	sudo ./qotd_server

# Test targets
test: qotd_server
	@echo "Testing server variant..."
	@echo "Starting server in background..."
	sudo ./qotd_server &
	@sleep 1
	@echo "TCP test:"
	@nc localhost 17
	@echo ""
	@echo "UDP test:"
	@timeout 3s bash -c 'echo "test" | nc -u -w 1 127.0.0.1 17' || echo "No UDP response"
	@echo ""
	@echo "Stopping server..."
	sudo pkill -f qotd_server

# Help target
help:
	@echo "Available targets:"
	@echo "  all                    - Build server variant"
	@echo "  qotd_server            - Build server"
	@echo "  run                    - Run server variant (requires sudo)"
	@echo "  test                   - Test server variant (requires sudo)"
	@echo "  help                  - Show this help"

.PHONY: all clean install uninstall run test help
