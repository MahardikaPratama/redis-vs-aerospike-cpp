#!/bin/bash

# Aerospike C++ Application - Docker Management Script

show_help() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  build         Build the Docker image"
    echo "  run           Run the application in a container"
    echo "  compose-up    Start using docker-compose (includes Aerospike)"
    echo "  compose-down  Stop docker-compose services"
    echo "  aerospike-only Start only Aerospike server"
    echo "  logs          View application logs"
    echo "  clean         Remove Docker images and containers"
    echo "  help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build"
    echo "  $0 run"
    echo "  $0 compose-up"
}

build_image() {
    echo "Building aerospike-cpp Docker image..."
    docker build -t aerospike-cpp .
}

run_container() {
    echo "Running aerospike-cpp application..."
    docker run --rm aerospike-cpp
}

compose_up() {
    echo "Starting application with docker-compose..."
    docker compose up --build
}

compose_down() {
    echo "Stopping docker-compose services..."
    docker compose down
}

aerospike_only() {
    echo "Starting only Aerospike server..."
    docker compose up -d aerospike
}

view_logs() {
    echo "Viewing application logs..."
    docker compose logs -f aerospike-cpp
}

clean_docker() {
    echo "Cleaning up Docker resources..."
    docker compose down 2>/dev/null || true
    docker rmi aerospike-cpp 2>/dev/null || true
    docker rmi aerospike-cpp-aerospike-cpp 2>/dev/null || true
    echo "Cleanup completed."
}

case "$1" in
    build)
        build_image
        ;;
    run)
        run_container
        ;;
    compose-up)
        compose_up
        ;;
    compose-down)
        compose_down
        ;;
    aerospike-only)
        aerospike_only
        ;;
    logs)
        view_logs
        ;;
    clean)
        clean_docker
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Error: Unknown command '$1'"
        echo ""
        show_help
        exit 1
        ;;
esac
