FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libhiredis-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/sewenew/redis-plus-plus.git /tmp/redis-plus-plus && \
    cd /tmp/redis-plus-plus && \
    mkdir -p build && cd build && \
    cmake .. && make && make install && \
    rm -rf /tmp/redis-plus-plus

WORKDIR /app
COPY . .

RUN git submodule update --init --recursive

RUN mkdir -p build && cd build && cmake ../docker && make

ENTRYPOINT ["./build/redis_test"]