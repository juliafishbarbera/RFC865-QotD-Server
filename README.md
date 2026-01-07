# RFC865 QotD/Cookie Server for Docker

Run a [Quote of the Day Protocol](https://datatracker.ietf.org/doc/html/rfc865)
server, with a Dockerfile to set it up containerized. Now you can finally use
DHCP option 8!

This project was originally based on
[Jack Kingsman's implementation](https://github.com/jkingsman/RFC865-QotD-Server-for-Docker).

Note that the application and Dockerfile expect to run on port 17, which will
require admin privileges to access a privileged port.

Provides multiple quote sources: `fortune | cowsay` output, 8ball-style
fortunes, or custom quotes from a file.

## Quickstart

Needs sudo to bind to privileged port.

```bash
sudo docker run -p 17:17/tcp -p 17:17/udp juliabarbera/rfc865-qotd-server:latest
```

Note: The server defaults to both TCP and UDP protocols when QOTD_NET is not
set.

See https://hub.docker.com/r/juliabarbera/rfc865-qotd-server

## Docker

### Docker Compose

Create a `docker-compose.yml` file with the following configuration:

```yaml
services:
  qotd:
    image: juliabarbera/rfc865-qotd-server
    restart: unless-stopped
    ports:
      - 17:17/tcp
      - 17:17/udp
    environment:
      - QOTD_MODE=8ball
      - QOTD_NET=tcp_udp
      - QUOTES_FILE=/quotes/custom.txt
    volumes:
      - ./quotes.txt:/quotes/custom.txt:ro
networks: {}
```

Then run:

```bash
sudo docker-compose up -d
```

### Docker Compilation

Build and run directly with

```bash
# server version
docker build --target qotd_server -t qotd_server .
```

### Docker Execution

```bash
# Default: 8ball mode with both TCP and UDP (QOTD_NET optional)
sudo docker run -p 17:17/tcp -p 17:17/udp \
  -e QOTD_MODE=8ball \
  --name qotd_server_container qotd_server

# Custom command mode - fortune | cowsay (TCP only)
sudo docker run -p 17:17/tcp \
  -e QOTD_MODE=command \
  -e QOTD_NET=tcp \
  --name qotd_server_container qotd_server

# Custom quotes file mode (UDP only)
sudo docker run -p 17:17/udp \
  -v /path/to/your/quotes.txt:/quotes/custom.txt \
  -e QOTD_MODE=file \
  -e QUOTES_FILE=/quotes/custom.txt \
  -e QOTD_NET=udp \
  --name qotd_server_container qotd_server
```

## Local

### Local Compilation

```bash
# server version
make
```

Or compile manually:

```bash
gcc -o qotd_server main.c quotes.c network.c config.c
```

## Local Execution

```bash
# Default: 8ball mode with both TCP and UDP (QOTD_NET optional)
sudo ./qotd_server

# File mode with TCP only
QOTD_MODE=file QOTD_NET=tcp QUOTES_FILE=./quotes.txt sudo ./qotd_server

# Command mode with UDP only
QOTD_MODE=command QOTD_NET=udp sudo ./qotd_server
```

## Configuration

When the server starts, it will display a message indicating which network
protocols are enabled:

- `QOTD server started on port 17 (TCP and UDP)` - Both protocols enabled
  (default when QOTD_NET unset)
- `QOTD server started on port 17 (TCP only)` - Only TCP enabled
- `QOTD server started on port 17 (UDP only)` - Only UDP enabled

### Environment Variables

#### Basic Configuration

- `QOTD_MODE`: Quote source mode
  - `8ball` (default): Use built-in 8ball fortunes
  - `command`: Use `fortune | cowsay`
  - `file`: Use custom quotes file
- `QUOTES_FILE`: Path to quotes file (used only when `QOTD_MODE=file`)
- `QOTD_NET`: Network protocol selection (optional)
  - `tcp_udp` or `udp_tcp`: Enable both TCP and UDP
  - `tcp`: Enable TCP only
  - `udp`: Enable UDP only
  - If unset, defaults to both TCP and UDP enabled
- `QOTD_PREFIX` `QOTD_SUFFIX`: Adds a prefix or suffix to the served quote
  (optional)
- `QOTD_COMMAND`: Command to source quote from (used only when
  `QOTD_MODE=command`)
- `QOTD_RATE`: Max requests per second per ip
- `QOTD_BURST`: Max rate limiting burst size

### Quote File Format

The quotes file should contain one quote per line, newline-separated.

## Interaction

```bash
# tcp
nc localhost 17

# udp
echo "" | nc -u localhost 17
```

```
## License

MIT License, Copyright (c) 2026 Julia Barbera <julia@fishcat.fish>

See `LICENSE.md` for full text.
```
