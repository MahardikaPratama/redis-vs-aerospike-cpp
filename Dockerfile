# Use Ubuntu 22.04 as base image
FROM ubuntu:22.04

# Set environment variables to avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    libcrypto++-dev \
    libcrypto++-utils \
    pkg-config \
    wget \
    curl \
    liblua5.1-0-dev \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

    # Install Aerospike C Client first
    RUN wget https://download.aerospike.com/artifacts/aerospike-client-c/7.0.2/aerospike-client-c_7.0.2_ubuntu22.04_x86_64.tgz \
        && tar -xzf aerospike-client-c_7.0.2_ubuntu22.04_x86_64.tgz \
        && cd aerospike-client-c_7.0.2_ubuntu22.04_x86_64 \
        && dpkg -i aerospike-client-c_7.0.2-ubuntu22.04_amd64.deb \
        && dpkg -i aerospike-client-c-devel_7.0.2-ubuntu22.04_amd64.deb \
        && cd .. \
        && rm -rf aerospike-client-c_7.0.2_ubuntu22.04_x86_64*# Set working directory
WORKDIR /app

# Copy source code
COPY src/ ./src/

# Copy build files
COPY Makefile ./

# Build the application
RUN make

# Expose any ports if needed (currently this app doesn't use networking)
# EXPOSE 8080

# Set the default command
CMD ["./aerospike-app"]
