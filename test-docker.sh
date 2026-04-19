#!/usr/bin/env bash
set -euo pipefail

# test-docker.sh - Docker integration test for i2pd metrics patch
#
# This script:
# 1. Ensures the patch file exists
# 2. Builds a Docker image with i2pd + metrics patch
# 3. Runs i2pd in a container with metrics enabled
# 4. Validates the metrics endpoint
# 5. Cleans up

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="i2pd-metrics-test"
CONTAINER_NAME="i2pd-metrics-test-container"

cleanup() {
    echo ""
    echo "==> Cleaning up..."
    docker stop "$CONTAINER_NAME" 2>/dev/null || true
    docker rm "$CONTAINER_NAME" 2>/dev/null || true
}

trap cleanup EXIT

echo "==> Docker integration test for i2pd metrics"
echo ""

# Check if patch exists
if [ ! -f "$REPO_ROOT/i2pd-metrics.patch" ]; then
    echo "ERROR: Patch file not found. Run ./generate-patch.sh first"
    exit 1
fi

echo "==> Building Docker image..."
docker build -f Dockerfile.test -t "$IMAGE_NAME" .

echo ""
echo "==> Starting i2pd container..."
docker run -d \
    --name "$CONTAINER_NAME" \
    -p 9600:9600 \
    "$IMAGE_NAME"

echo ""
echo "==> Waiting for i2pd to start (checking health)..."
MAX_WAIT=30
WAITED=0
while [ $WAITED -lt $MAX_WAIT ]; do
    if docker inspect --format='{{.State.Health.Status}}' "$CONTAINER_NAME" 2>/dev/null | grep -q "healthy"; then
        echo "[pass] Container is healthy"
        break
    fi

    if [ $WAITED -eq 0 ]; then
        printf "Waiting"
    else
        printf "."
    fi

    sleep 1
    WAITED=$((WAITED + 1))
done

if [ $WAITED -ge $MAX_WAIT ]; then
    echo ""
    echo "[fail] Container did not become healthy within ${MAX_WAIT}s"
    echo ""
    echo "Container logs:"
    docker logs "$CONTAINER_NAME"
    exit 1
fi

echo ""
echo ""
echo "==> Running metrics endpoint tests..."
echo ""

# Test 1: Check HTTP status code
echo -n "Test 1: GET /metrics returns 200... "
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:9600/metrics)
if [ "$STATUS" = "200" ]; then
    echo "[pass]"
else
    echo "[fail] (got $STATUS)"
    exit 1
fi

# Test 2: Check Content-Type
echo -n "Test 2: Content-Type is OpenMetrics... "
CONTENT_TYPE=$(curl -s -I http://localhost:9600/metrics | grep -i content-type)
if echo "$CONTENT_TYPE" | grep -qi "openmetrics"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Got: $CONTENT_TYPE"
    exit 1
fi

# Fetch metrics for remaining tests
METRICS=$(curl -s http://localhost:9600/metrics)

# Test 3: Check for EOF marker
echo -n "Test 3: Response contains '# EOF'... "
if echo "$METRICS" | grep -q "# EOF"; then
    echo "[pass]"
else
    echo "[fail]"
    exit 1
fi

# Test 4: Check for router metrics
echo -n "Test 4: Router metrics present... "
if echo "$METRICS" | grep -q "i2pd_router_uptime_seconds" && \
   echo "$METRICS" | grep -q "i2pd_router_info"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Missing router metrics"
    exit 1
fi

# Test 5: Check for tunnel metrics
echo -n "Test 5: Tunnel metrics present... "
if echo "$METRICS" | grep -q "i2pd_tunnels_participating" && \
   echo "$METRICS" | grep -q "i2pd_tunnels_creation_success_rate"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Missing tunnel metrics"
    exit 1
fi

# Test 6: Check for transport metrics
echo -n "Test 6: Transport metrics present... "
if echo "$METRICS" | grep -q "i2pd_transport_peers_total" && \
   echo "$METRICS" | grep -q "i2pd_transport_bytes_total"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Missing transport metrics"
    exit 1
fi

# Test 7: Check for NetDb metrics
echo -n "Test 7: NetDb metrics present... "
if echo "$METRICS" | grep -q "i2pd_netdb_routers" && \
   echo "$METRICS" | grep -q "i2pd_netdb_floodfills"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Missing NetDb metrics"
    exit 1
fi

# Test 8: Check for Client metrics
echo -n "Test 8: Client metrics present... "
if echo "$METRICS" | grep -q "i2pd_client_destinations" && \
   echo "$METRICS" | grep -q "i2pd_client_sam_sessions"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Missing Client metrics"
    exit 1
fi

# Test 9: Check 404 for unknown path
echo -n "Test 9: GET /unknown returns 404... "
STATUS_404=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:9600/unknown)
if [ "$STATUS_404" = "404" ]; then
    echo "[pass]"
else
    echo "[fail] (got $STATUS_404)"
    exit 1
fi

# Test 10: Validate OpenMetrics format (TYPE and HELP directives)
echo -n "Test 10: OpenMetrics TYPE and HELP present... "
if echo "$METRICS" | grep -q "# TYPE i2pd_router_uptime_seconds gauge" && \
   echo "$METRICS" | grep -q "# HELP i2pd_router_uptime_seconds"; then
    echo "[pass]"
else
    echo "[fail]"
    echo "Missing TYPE or HELP directives"
    exit 1
fi

echo ""
echo "==> Sample metrics output:"
echo "---"
echo "$METRICS" | head -50
echo "..."
echo ""

echo "==> All tests passed! [pass]"
echo ""
echo "Patch successfully integrates with i2pd 2.59.0"
