#include "RouterCollector.h"
#include "../../submodules/i2pd/libi2pd/RouterContext.h"
#include "../../submodules/i2pd/libi2pd/version.h"
#include "../MetricsRenderer.h"
#include <map>
#include <string>

namespace i2p {
namespace metrics {
void CollectRouterMetrics(std::vector<MetricFamily> &families) {
  // i2pd_router_uptime_seconds
  MetricFamily uptime("i2pd_router_uptime_seconds", MetricType::Gauge,
                      "Router uptime in seconds");
  uptime.AddMetric(static_cast<double>(i2p::context.GetUptime()));
  families.push_back(std::move(uptime));

  // i2pd_router_info
  MetricFamily info("i2pd_router_info", MetricType::Info,
                    "Router version information");
  std::map<std::string, std::string> infoLabels;
  infoLabels["version"] = I2PD_VERSION;
  info.AddMetric(infoLabels, 1.0);
  families.push_back(std::move(info));

  // i2pd_router_bandwidth_limit_bytes
  MetricFamily bandwidth("i2pd_router_bandwidth_limit_bytes", MetricType::Gauge,
                         "Router bandwidth limit in bytes per second");
  bandwidth.AddMetric(static_cast<double>(i2p::context.GetBandwidthLimit()));
  families.push_back(std::move(bandwidth));

  // i2pd_router_status
  MetricFamily status("i2pd_router_status", MetricType::Gauge,
                      "Router network status (0=OK, 1=Firewalled, 2=Unknown, "
                      "3=Proxy, 4=Mesh, 5=Stan)");
  std::map<std::string, std::string> statusLabels;
  RouterStatus routerStatus = i2p::context.GetStatus();
  statusLabels["status"] = ROUTER_STATUS_NAMES[routerStatus];
  status.AddMetric(statusLabels, static_cast<double>(routerStatus));
  families.push_back(std::move(status));

  // i2pd_router_status_v6
  MetricFamily statusV6("i2pd_router_status_v6", MetricType::Gauge,
                        "Router IPv6 network status (0=OK, 1=Firewalled, "
                        "2=Unknown, 3=Proxy, 4=Mesh, 5=Stan)");
  std::map<std::string, std::string> statusV6Labels;
  RouterStatus routerStatusV6 = i2p::context.GetStatusV6();
  statusV6Labels["status"] = ROUTER_STATUS_NAMES[routerStatusV6];
  statusV6.AddMetric(statusV6Labels, static_cast<double>(routerStatusV6));
  families.push_back(std::move(statusV6));
}
} // namespace metrics
} // namespace i2p
