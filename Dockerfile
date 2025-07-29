FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# 1. Install system dependencies and Redis
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    curl \
    gpg \
    lsb-release \
    ca-certificates \
    wget \
    && curl -fsSL https://packages.redis.io/gpg | gpg --dearmor -o /usr/share/keyrings/redis-archive-keyring.gpg \
    && chmod 644 /usr/share/keyrings/redis-archive-keyring.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/redis-archive-keyring.gpg] https://packages.redis.io/deb $(lsb_release -cs) main" \
    | tee /etc/apt/sources.list.d/redis.list \
    && apt-get update && apt-get install -y redis \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 2. Install hiredis
RUN git clone https://github.com/redis/hiredis.git /tmp/hiredis && \
    cd /tmp/hiredis && make && make install && rm -rf /tmp/hiredis

# 3. Install redis-plus-plus (requires hiredis)
RUN git clone https://github.com/sewenew/redis-plus-plus.git /tmp/redis-plus-plus && \
    cd /tmp/redis-plus-plus && mkdir -p build && cd build && \
    cmake .. && make && make install && rm -rf /tmp/redis-plus-plus

# 4. Build your app
WORKDIR /app
COPY . .


RUN rm -rf external/json && \
    git clone https://github.com/nlohmann/json.git external/json && \
    cd external/json && git submodule update --init --recursive

RUN rm -rf build/ && \
    mkdir -p build && cd build && cmake .. && cmake --build .

COPY wait-for-redis.sh /wait-for-redis.sh
RUN chmod +x /wait-for-redis.sh

# 6. Run binary
ENTRYPOINT ["./build/redis_test"]
