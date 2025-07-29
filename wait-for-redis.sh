#!/bin/bash
set -e

host="$REDIS_HOST"
port="$REDIS_PORT"

echo "Waiting for Redis at $host:$port..."

until nc -z "$host" "$port"; do
  >&2 echo "Redis is unavailable - sleeping"
  sleep 1
done

echo "Redis is up - executing command"
exec "$@"
