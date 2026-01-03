# Makefile for RFC865 QOTD Server
# Supports both 8ball and fortune-cowsay variants

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = 

# Targets
TARGETS = qotd-8ball qotd-fortune-cowsay

# Source files
QOTD_8BALL_SRC = qotd_server.c
QOTD_FORTUNE_COWSAY_SRC = qotd_server_cowsay_fortune.c

# Default target
all: $(TARGETS)

# 8ball variant
qotd-8ball: $(QOTD_8BALL_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Fortune-cowsay variant
qotd-fortune-cowsay: $(QOTD_FORTUNE_COWSAY_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(TARGETS)

# Install targets (requires sudo for port 17)
install: $(TARGETS)
	sudo cp qotd-8ball /usr/local/bin/
	sudo cp qotd-fortune-cowsay /usr/local/bin/

# Uninstall targets
uninstall:
	sudo rm -f /usr/local/bin/qotd-8ball
	sudo rm -f /usr/local/bin/qotd-fortune-cowsay

# Run targets (requires sudo for privileged port)
run-8ball: qotd-8ball
	sudo ./qotd-8ball

run-fortune: qotd-fortune-cowsay
	sudo ./qotd-fortune-cowsay

# Test targets
test-8ball: qotd-8ball
	@echo "Testing 8ball variant..."
	@echo "Starting server in background..."
	sudo ./qotd-8ball &
	@sleep 1
	@echo "TCP test:"
	@nc localhost 17
	@echo ""
	@echo "UDP test:"
	@echo "" | nc -u localhost 17
	@echo ""
	@echo "Stopping server..."
	sudo pkill -f qotd-8ball

test-fortune: qotd-fortune-cowsay
	@echo "Testing fortune-cowsay variant..."
	@echo "Starting server in background..."
	sudo ./qotd-fortune-cowsay &
	@sleep 1
	@echo "TCP test:"
	@nc localhost 17
	@echo ""
	@echo "UDP test:"
	@echo "" | nc -u localhost 17
	@echo ""
	@echo "Stopping server..."
	sudo pkill -f qotd-fortune-cowsay

# Help target
help:
	@echo "Available targets:"
	@echo "  all                    - Build both variants"
	@echo "  qotd-8ball            - Build 8ball variant"
	@echo "  qotd-fortune-cowsay   - Build fortune-cowsay variant"
	@echo "  clean                 - Remove build artifacts"
	@echo "  install               - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall             - Remove from /usr/local/bin (requires sudo)"
	@echo "  run-8ball             - Run 8ball variant (requires sudo)"
	@echo "  run-fortune           - Run fortune-cowsay variant (requires sudo)"
	@echo "  test-8ball            - Test 8ball variant (requires sudo)"
	@echo "  test-fortune          - Test fortune-cowsay variant (requires sudo)"
	@echo "  help                  - Show this help"

.PHONY: all clean install uninstall run-8ball run-fortune test-8ball test-fortune help