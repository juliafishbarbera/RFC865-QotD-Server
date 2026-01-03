# Common builder stage for both targets
FROM alpine:3.19 AS builder

RUN apk add --no-cache gcc musl-dev
WORKDIR /app

COPY qotd_server.c .

RUN gcc -O3 -static -o qotd_server qotd_server.c



FROM alpine:3.19 AS qotd_server

RUN apk update && \
    apk add --no-cache \
    cowsay@testing \
    fortune@community

# Create directory for quotes
RUN mkdir -p /quotes

# Copy default quotes file
COPY quotes.txt /quotes/quotes.txt

# Set default environment variables
ENV QOTD_MODE="8ball"
ENV QUOTES_FILE="/quotes/quotes.txt"

COPY --from=builder /app/qotd_server /qotd_server
EXPOSE 17/tcp 17/udp
ENTRYPOINT ["/qotd_server"]
