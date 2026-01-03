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
REGISTRY="${2:-jkingsman/qotd-appliance}"

# Check dependencies
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed or not in PATH"
    exit 1
fi

if ! docker buildx version &> /dev/null; then
    echo "Error: Docker buildx is not available. Please install buildx."
    exit 1
fi

# Build both images
echo "Building qotd_8ball image..."
echo "Registry: $REGISTRY"
docker buildx build --platform linux/arm/v7,linux/arm64/v8,linux/amd64 --target qotd_8ball -t $REGISTRY:latest -t $REGISTRY:8ball-${VERSION} -t $REGISTRY:8ball-latest .

echo "Building qotd_fortune_cowsay image..."
docker buildx build --platform linux/arm/v7,linux/arm64/v8,linux/amd64 --target qotd_fortune_cowsay -t $REGISTRY:fortune-cowsay-${VERSION} -t $REGISTRY:fortune-cowsay-latest .

# Push all tags to registry
echo "Pushing all images to registry..."
docker push $REGISTRY:latest
docker push $REGISTRY:8ball-${VERSION}
docker push $REGISTRY:8ball-latest
docker push $REGISTRY:fortune-cowsay-${VERSION}
docker push $REGISTRY:fortune-cowsay-latest

echo "All images built and pushed successfully!"
echo "Registry: $REGISTRY"
echo "Version: $VERSION"
