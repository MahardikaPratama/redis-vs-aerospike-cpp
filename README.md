# Aerospike C++ Application

This is a C++ application that demonstrates various database operations using **Aerospike Database** with performance timing measurements. The application uses Crypto++ for hashing and integrates with Aerospike for persistent data storage.

## Features

- **Aerospike Integration**: Full integration with Aerospike database
- Create records in Aerospike database
- Update records by primary key
- Search records by primary key
- Search records by multiple field values (using scan operations)
- Search records by field value with range
- Delete operations
- Performance timing for each operation
- Docker containerization with Aerospike server
- Environment-based configuration

## Dependencies

- C++17 or later
- Crypto++ library (for RIPEMD-160 hashing)
- Aerospike C++ Client Library
- Aerospike Database Server

## Docker Usage

### Quick Start with Script

The easiest way to manage the Docker containers is using the provided script:

```bash
# Make the script executable (first time only)
chmod +x docker.sh

# Start the full stack (Aerospike + Application)
./docker.sh compose-up

# Or start only Aerospike server
./docker.sh aerospike-only

# View application logs
./docker.sh logs

# Stop everything
./docker.sh compose-down
```

### Manual Docker Commands with Aerospike

#### Full Stack (Recommended)

1. **Start Aerospike and Application:**
   ```bash
   docker compose up --build
   ```

2. **Start only Aerospike server:**
   ```bash
   docker compose up -d aerospike
   ```

3. **Run application separately (after Aerospike is running):**
   ```bash
   docker compose up aerospike-cpp
   ```

### Manual Docker Commands

#### Development Build

1. **Build the Docker image:**
   ```bash
   docker build -t aerospike-cpp .
   ```

2. **Run the container:**
   ```bash
   docker run --rm aerospike-cpp
   ```

#### Production Build (Optimized)

For production deployment, use the multi-stage Dockerfile:

```bash
# Build production image (smaller size, security optimized)
docker build -f Dockerfile.production -t aerospike-cpp:prod .

# Run production container
docker run --rm aerospike-cpp:prod
```

### Using Docker Compose

1. **Build and run with docker-compose:**
   ```bash
   docker compose up --build
   ```

2. **Run in detached mode:**
   ```bash
   docker compose up -d --build
   ```

3. **View logs:**
   ```bash
   docker compose logs -f
   ```

4. **Stop and remove:**
   ```bash
   docker compose down
   ```

### Environment Variables

The application supports the following environment variables:

- `AEROSPIKE_HOST`: Aerospike server hostname (default: localhost)
- `AEROSPIKE_PORT`: Aerospike server port (default: 3000)
- `AEROSPIKE_NAMESPACE`: Aerospike namespace (default: test)
- `AEROSPIKE_SET`: Aerospike set name (default: books)

### Aerospike Web Console

When running with Docker Compose, you can access the Aerospike database on:
- **Service Port**: localhost:3000
- **Info Port**: localhost:3003

### Docker Features

- **Full Stack**: Includes Aerospike server and C++ application
- **Multi-stage Production Build**: Optimized runtime images
- **Health Checks**: Aerospike health monitoring
- **Persistent Storage**: Data persistence via Docker volumes
- **Network Isolation**: Dedicated Docker network for services
- **Environment Configuration**: Configurable via environment variables

## Local Development

If you want to build and run locally without Docker:

### Prerequisites

- g++ with C++17 support
- libcrypto++-dev package

### Build

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential libcrypto++-dev

# Build the application
make

# Run the application
./aerospike-app
```

## Project Structure

```
src/
├── app.cpp                 # Main application file
├── data_types/
│   └── book_data_type.h   # Book data structure definitions
└── operations/
    ├── operations.h       # Function declarations
    └── operations.cpp     # Implementation of database operations
```

## Performance Testing

The application creates 10,000 book records and measures the performance of various operations:

- Record creation
- Primary key updates
- Primary key searches
- Multi-field searches
- Range searches
- Record deletion

All timing measurements are displayed in microseconds.
