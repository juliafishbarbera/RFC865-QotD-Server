# Common builder stage for both targets
FROM alpine:3.19 AS builder

RUN apk add --no-cache gcc musl-dev
WORKDIR /app

COPY src/ .

RUN gcc -O3 -static -o qotd_server main.c quotes.c network.c config.c

FROM alpine:3.19 AS qotd_server

# Create directory for quotes
RUN mkdir -p /quotes

# Copy default quotes file
COPY quotes.txt /quotes/quotes.txt

# Set default environment variables
ENV QOTD_MODE="8ball"
ENV QOTD_FILE="/quotes/quotes.txt"
ENV QOTD_PREFIX="prefixprefix"
ENV QOTD_SUFFIX="suffixsuffix"

COPY --from=builder /app/qotd_server /qotd_server
EXPOSE 17/tcp 17/udp
ENTRYPOINT ["/qotd_server"]
