# Makefile for RFC865 QOTD Server
# Supports fortune-cowsay variant

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = 

# Targets
TARGETS = qotd-server

# Source files
QOTD_SERVER_SRC = qotd_server.c

# Default target
all: $(TARGETS)

# Server variant
qotd-server: $(QOTD_SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(TARGETS)

# Install targets (requires sudo for port 17)
install: $(TARGETS)
	sudo cp qotd-server /usr/local/bin/

# Uninstall targets
uninstall:
	sudo rm -f /usr/local/bin/qotd-server

# Run targets (requires sudo for privileged port)
run: qotd-server
	sudo ./qotd-server

# Test targets
test: qotd-server
	@echo "Testing server variant..."
	@echo "Starting server in background..."
	sudo ./qotd-server &
	@sleep 1
	@echo "TCP test:"
	@nc localhost 17
	@echo ""
	@echo "UDP test:"
	@echo "" | nc -u localhost 17
	@echo ""
	@echo "Stopping server..."
	sudo pkill -f qotd-server

# Help target
help:
	@echo "Available targets:"
	@echo "  all                    - Build server variant"
	@echo "  qotd-server           - Build server variant"
	@echo "  run                    - Run server variant (requires sudo)"
	@echo "  test                   - Test server variant (requires sudo)"
	@echo "  help                  - Show this help"

.PHONY: all clean install uninstall run test help