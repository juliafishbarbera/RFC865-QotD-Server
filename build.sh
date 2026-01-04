#!/usr/bin/env bash

# ./build.sh [version] [registry]

if [ $# -eq 0 ]; then
  echo "Error: Version argument is required"
  echo "Usage: $0 <version> [registry]"
  echo "Example: $0 1.0.0"
  echo "Example: $0 1.0.0 myregistry/qotd-appliance"
  exit 1
fi

# Set the version and optional registry from command line arguments
VERSION="$1"
REGISTRY="${2:-juliabarbera/rfc865-qotd-server}"

# Check dependencies
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed or not in PATH"
    exit 1
fi

if ! docker buildx version &> /dev/null; then
    echo "Error: Docker buildx is not available. Please install buildx."
    exit 1
fi

# Build image
echo "Building qotd_server image..."
echo "Registry: $REGISTRY"
docker buildx build --platform linux/arm64,linux/amd64 --target qotd_server -t $REGISTRY:latest -t $REGISTRY:${VERSION} --push .

# Also build local image for current platform
echo "Building local image for current platform..."
docker build -t ${REGISTRY}:local .

echo "All images built and pushed successfully!"
echo "Registry: $REGISTRY"
echo "Version: $VERSION"
