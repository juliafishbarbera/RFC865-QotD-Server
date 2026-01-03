# RFC865 QotD/Cookie Server for Docker

Run a [Quote of the Day Protocol](https://datatracker.ietf.org/doc/html/rfc865)
server, with a Dockerfile to set it up containerized. Now you can finally use
DHCP option 8!

Note that the application and Dockerfile expect to run on port 17, which will
require admin privileges to access a privileged port.

Provides multiple quote sources: `fortune | cowsay` output, 8ball-style
fortunes, or custom quotes from a file.

## Quickstart

Needs sudo to bind to privileged port.

```bash
sudo docker run -p 17:17/tcp -p 17:17/udp jkingsman/qotd-appliance:latest
```

See https://hub.docker.com/repository/docker/jkingsman/qotd-appliance/general.

## Docker

### Docker Compilation

Build and run directly with

```bash
# server version
docker build --target qotd_fortune_cowsay -t qotd_server .
```

### Docker Execution

```bash
# Default: 8ball mode
sudo docker run -p 17:17/tcp -p 17:17/udp \
  -e QOTD_MODE=8ball \
  --name qotd_server_container qotd_server

# Custom command mode - fortune | cowsay
sudo docker run -p 17:17/tcp -p 17:17/udp \
-e QOTD_MODE=command \
--name qotd_server_container qotd_server

# Custom quotes file mode
sudo docker run -p 17:17/tcp -p 17:17/udp \
  -v /path/to/your/quotes.txt:/quotes/custom.txt \
  -e QOTD_MODE=file \
  -e QUOTES_FILE=/quotes/custom.txt \
  --name qotd_server_container qotd_server

# Default: 8ball mode
sudo docker run -p 17:17/tcp -p 17:17/udp \
  -e QOTD_MODE=8ball \
  --name qotd_server_container qotd_server
```

## Local

### Local Compilation

```bash
# server version
gcc -o qotd_server qotd_server.c
```

if you're feeling aggressive.

## Local Execution

```bash
# Default: 8ball mode
sudo ./qotd_server

# File mode
QOTD_MODE=file QUOTES_FILE=./quotes.txt sudo ./qotd_server

# Command mode (fortune | cowsay)
QOTD_MODE=command sudo ./qotd_server
```

## Configuration

### Environment Variables

- `QOTD_MODE`: Quote source mode
  - `8ball` (default): Use built-in 8ball fortunes
  - `command`: Use `fortune | cowsay`
  - `file`: Use custom quotes file
- `QUOTES_FILE`: Path to quotes file (used only when `QOTD_MODE=file`)

### Quote File Format

The quotes file should contain one quote per line, newline-separated. Example:

## Interaction

```bash
# tcp
nc localhost 17

# udp
echo "" | nc -u localhost 17
```

## License

MIT License, Copyright (c) 2026 Jack Kingsman <jack@jackkingsman.me>

See `LICENSE.md` for full text.
