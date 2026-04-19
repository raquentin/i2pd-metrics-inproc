#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
I2PD_DIR="$REPO_ROOT/submodules/i2pd"
PATCH_OUTPUT="$REPO_ROOT/i2pd-metrics.patch"

echo "==> Checking i2pd submodule..."
if [ ! -f "$I2PD_DIR/daemon/Daemon.cpp" ]; then
    echo "ERROR: i2pd submodule not initialized. Run: git submodule update --init submodules/i2pd"
    exit 1
fi
cd "$I2PD_DIR"
COMMIT=$(git rev-parse HEAD)
if [ "$COMMIT" != "896f548175aa605efd15ecbfb744588e0c14f64f" ]; then
    echo "ERROR: i2pd submodule is not at 2.59.0 commit (got $COMMIT)"
    exit 1
fi

echo "==> Resetting i2pd submodule to clean state..."
git reset --hard HEAD
git clean -fdx

echo "==> Copying metrics files..."
mkdir -p "$I2PD_DIR/daemon/collectors"
cp "$REPO_ROOT/daemon/Metrics.h"           "$I2PD_DIR/daemon/"
cp "$REPO_ROOT/daemon/Metrics.cpp"         "$I2PD_DIR/daemon/"
cp "$REPO_ROOT/daemon/MetricsRenderer.h"   "$I2PD_DIR/daemon/"
cp "$REPO_ROOT/daemon/MetricsRenderer.cpp" "$I2PD_DIR/daemon/"
cp "$REPO_ROOT/daemon/collectors"/*.h      "$I2PD_DIR/daemon/collectors/"
cp "$REPO_ROOT/daemon/collectors"/*.cpp    "$I2PD_DIR/daemon/collectors/"

echo "==> Fixing include paths for i2pd tree..."
# Our dev workspace uses ../../submodules/i2pd/libi2pd/... but inside i2pd's
# tree daemon/collectors/ is 2 levels from root, so the correct path is ../../libi2pd/...
python3 - "$I2PD_DIR/daemon/collectors" <<'PYEOF'
import sys, os, pathlib

collectors_dir = pathlib.Path(sys.argv[1])
for f in collectors_dir.glob("*.cpp"):
    text = f.read_text()
    text = text.replace("../../submodules/i2pd/libi2pd/", "../../libi2pd/")
    text = text.replace("../../submodules/i2pd/libi2pd_client/", "../../libi2pd_client/")
    f.write_text(text)
    print(f"  fixed: {f.name}")
PYEOF

echo "==> Patching Daemon.cpp..."
python3 - "$I2PD_DIR/daemon/Daemon.cpp" <<'PYEOF'
import sys, re

path = sys.argv[1]
text = open(path).read()

# Add #include "Metrics.h" after #include "I2PControl.h"
text = text.replace(
    '#include "I2PControl.h"',
    '#include "I2PControl.h"\n#include "Metrics.h"'
)

# Add metricsServer member after m_I2PControlService
text = text.replace(
    '\t\tstd::unique_ptr<i2p::client::I2PControlService> m_I2PControlService;',
    '\t\tstd::unique_ptr<i2p::client::I2PControlService> m_I2PControlService;\n\t\tstd::unique_ptr<i2p::metrics::MetricsServer> metricsServer;'
)

# Add metrics start block before "return true;" in start()
# Find the last "return true;" in the start() function
metrics_start = '''
\t\t// start metrics server
\t\tbool metricsEnabled; i2p::config::GetOption("metrics.enabled", metricsEnabled);
\t\tif (metricsEnabled) {
\t\t\tstd::string metricsAddr; i2p::config::GetOption("metrics.address", metricsAddr);
\t\t\tuint16_t metricsPort; i2p::config::GetOption("metrics.port", metricsPort);
\t\t\tLogPrint(eLogInfo, "Daemon: Starting Metrics at ", metricsAddr, ":", metricsPort);
\t\t\td.metricsServer = std::unique_ptr<i2p::metrics::MetricsServer>(new i2p::metrics::MetricsServer(metricsAddr, metricsPort));
\t\t\td.metricsServer->Start();
\t\t}
\t\treturn true;'''

# Replace the "return true;" at the end of start()
# It appears as "\t\treturn true;" - find the one in start() by context
text = text.replace(
    '\n\t\treturn true;\n\t}\n\n\tbool Daemon_Singleton::stop',
    metrics_start + '\n\t}\n\n\tbool Daemon_Singleton::stop'
)

# Add metrics stop block after m_I2PControlService = nullptr;
metrics_stop = '''\t\t\td.m_I2PControlService = nullptr;
\t\t}
\t\tif (d.metricsServer)
\t\t{
\t\t\tLogPrint(eLogInfo, "Daemon: Stopping Metrics");
\t\t\td.metricsServer->Stop();
\t\t\td.metricsServer = nullptr;
\t\t}'''

text = text.replace(
    '\t\t\td.m_I2PControlService = nullptr;\n\t\t}',
    metrics_stop
)

open(path, 'w').write(text)
print("  Daemon.cpp patched")
PYEOF

echo "==> Patching libi2pd/Config.cpp..."
python3 - "$I2PD_DIR/libi2pd/Config.cpp" <<'PYEOF'
import sys

path = sys.argv[1]
text = open(path).read()

# Add metrics options_description block before the m_OptionsDesc.add chain
metrics_block = '''
		options_description metrics("Metrics options");
		metrics.add_options()
			("metrics.enabled", value<bool>()->default_value(false),             "Enable or disable Prometheus/OpenMetrics endpoint")
			("metrics.address", value<std::string>()->default_value("127.0.0.1"), "Metrics listen address")
			("metrics.port",    value<uint16_t>()->default_value(9600),           "Metrics listen port")
		;

		m_OptionsDesc'''

text = text.replace('\n\t\tm_OptionsDesc', metrics_block, 1)

# Add .add(metrics) to the options chain, before the final ;
# Find the last .add(...) and add after it
text = text.replace(
    '\n\t\t\t.add(cpuext)',
    '\n\t\t\t.add(cpuext)\n\t\t\t.add(metrics)'
)

# Handle meshnets conditional block — add metrics after the closing #endif block
# The chain might end with meshnets inside #ifdef, so we add after the ; on same pattern
# Actually we inserted before meshnets check above, let's verify by looking at structure.
# The add chain ends differently — let's also try after the #endif close
open(path, 'w').write(text)
print("  Config.cpp patched")
PYEOF

echo "==> Patching filelist.mk..."
python3 - "$I2PD_DIR/filelist.mk" <<'PYEOF'
import sys
path = sys.argv[1]
text = open(path).read()
text = text.replace(
    'DAEMON_SRC = $(wildcard $(DAEMON_SRC_DIR)/*.cpp)',
    'DAEMON_SRC = $(wildcard $(DAEMON_SRC_DIR)/*.cpp) $(wildcard $(DAEMON_SRC_DIR)/collectors/*.cpp)'
)
open(path, 'w').write(text)
print("  filelist.mk patched")
PYEOF

echo "==> Patching Makefile (add obj/daemon/collectors dir)..."
python3 - "$I2PD_DIR/Makefile" <<'PYEOF'
import sys
path = sys.argv[1]
text = open(path).read()
text = text.replace(
    '\t@mkdir -p obj/$(DAEMON_SRC_DIR)\n',
    '\t@mkdir -p obj/$(DAEMON_SRC_DIR)\n\t@mkdir -p obj/$(DAEMON_SRC_DIR)/collectors\n'
)
open(path, 'w').write(text)
print("  Makefile patched")
PYEOF

echo "==> Adding metrics config section to contrib/i2pd.conf..."
cat >> "$I2PD_DIR/contrib/i2pd.conf" << 'EOF'


## Prometheus/OpenMetrics endpoint
[metrics]
## Enable Prometheus/OpenMetrics endpoint (default: false)
# enabled = false
## Metrics listen address (default: 127.0.0.1)
# address = 127.0.0.1
## Metrics listen port (default: 9600)
# port = 9600
EOF

echo "==> Generating patch..."
cd "$I2PD_DIR"
git add -A
git diff --cached > "$PATCH_OUTPUT"

echo "==> Patch generated: $PATCH_OUTPUT"
echo ""
echo "Stats:"
wc -l "$PATCH_OUTPUT"
echo ""
echo "Files changed:"
git apply --stat "$PATCH_OUTPUT" 2>&1 || true

echo "==> Cleaning up i2pd submodule..."
git reset --hard HEAD
git clean -fdx
