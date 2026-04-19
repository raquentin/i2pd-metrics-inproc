#!/usr/bin/env bash
set -euo pipefail

echo "=== i2pd-metrics unit tests ==="
echo

# build and run unit tests
make test
