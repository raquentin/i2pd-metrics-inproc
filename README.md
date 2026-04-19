# i2pd-metrics-inproc

a 200 line patch to add prometheus exports to [https://github.com/purplei2p/i2pd](i2pd)

## exports

- `i2pd_router_*` - router status, uptime, version, bandwidth limits
- `i2pd_tunnels_*` - tunnel counts, queue sizes, success rates
- `i2pd_transport_*` - peer counts, bandwidth, bytes transferred
- `i2pd_netdb_*` - known routers, floodfills, leasesets
- `i2pd_client_*` - client destinations, tunnels, SAM sessions

## usage

### nix

```nix
{
  inputs.i2pd-metrics.url = "github:raquentin/i2pd-metrics-inproc";

  outputs = { nixpkgs, i2pd-metrics, ... }: {
    nixosConfigurations.yourhost = nixpkgs.lib.nixosSystem {
      modules = [
        i2pd-metrics.nixosModules.default
        {
          services.i2pd.metrics = {
            enable = true;
            address = "127.0.0.1";
            port = 9600;
          };
        }
      ];
    };
  };
}
```

### manual build

```bash
git clone --recursive https://github.com/raquentin/i2pd-metrics-inproc
cd i2pd-metrics-inproc

# generate patch
./generate-patch.sh

# apply patch to i2pd
cd /path/to/i2pd
git apply /path/to/i2pd-metrics.patch

# build i2pd
make
```


### i2pd config

add to `i2pd.conf`:

```ini
[metrics]
enabled = true
address = 127.0.0.1
port = 9600
```

or via command-line arguments:

```bash
i2pd --metrics.enabled=1 --metrics.address=127.0.0.1 --metrics.port=9600
```

### prometheus config

add to `prometheus.yml`:

```yaml
scrape_configs:
  - job_name: 'i2pd'
    static_configs:
      - targets: ['localhost:9600']
    scrape_interval: 15s
```

## development

```bash
nix develop

# or install dependencies manually
# - g++
# - Boost (system, asio)
# - OpenSSL
# - zlib

make test

./generate-patch.sh
```

## testing

```bash
make test
```

## target i2pd Version

- **version:** 2.59.0
- **commit:** `896f548175aa605efd15ecbfb744588e0c14f64f`

## license

BSD-3-Clause (matches i2pd license)
