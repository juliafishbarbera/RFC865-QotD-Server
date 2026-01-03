# Common builder stage for both targets
FROM alpine:3.19 AS builder

RUN apk add --no-cache gcc musl-dev
WORKDIR /app

COPY qotd_server.c .

RUN gcc -O3 -static -o qotd_server qotd_server.c



FROM alpine:3.19 AS qotd_server
RUN echo "@testing https://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories
RUN echo "@community http://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories

RUN apk update && \
    apk add --no-cache \
    cowsay@testing \
    fortune@community

COPY --from=builder /app/qotd_server /qotd_server
EXPOSE 17/tcp 17/udp
ENTRYPOINT ["/qotd_server"]
