# RFC865 QotD/Cookie Server for Docker

Run a [Quote of the Day Protocol](https://datatracker.ietf.org/doc/html/rfc865) server, with a Dockerfile to set it up containerized. Now you can finally use DHCP option 8!

Note that the application and Dockerfile expect to run on port 17, which will require admin privileges to access a privileged port.

Provides a `fortune | cowsay` output server.

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
sudo docker run -p 17:17/tcp -p 17:17/udp --name qotd_server_container qotd_server
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
sudo ./qotd_server
```

## Interaction

```bash
# tcp
nc localhost 17

# udp
echo "" | nc -u localhost 17
```

## License

MIT License, Copyright (c) 2025 Jack Kingsman <jack@jackkingsman.me>

See `LICENSE.md` for full text.
