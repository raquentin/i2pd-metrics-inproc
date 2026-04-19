#include "ClientCollector.h"
#include "../../submodules/i2pd/libi2pd_client/ClientContext.h"
#include "../MetricsRenderer.h"
#include <map>
#include <string>

namespace i2p {
namespace metrics {
void CollectClientMetrics(std::vector<MetricFamily> &families) {
  // i2pd_client_destinations
  MetricFamily destinations("i2pd_client_destinations", MetricType::Gauge,
                            "Number of local client destinations");
  destinations.AddMetric(
      static_cast<double>(i2p::client::context.GetDestinations().size()));
  families.push_back(std::move(destinations));

  // i2pd_client_tunnels
  MetricFamily tunnels("i2pd_client_tunnels", MetricType::Gauge,
                       "Number of client tunnels by type");

  std::map<std::string, std::string> clientLabels;
  clientLabels["type"] = "client";
  tunnels.AddMetric(
      clientLabels,
      static_cast<double>(i2p::client::context.GetClientTunnels().size()));

  std::map<std::string, std::string> serverLabels;
  serverLabels["type"] = "server";
  tunnels.AddMetric(
      serverLabels,
      static_cast<double>(i2p::client::context.GetServerTunnels().size()));
  families.push_back(std::move(tunnels));

  // i2pd_client_udp_tunnels
  MetricFamily udpTunnels("i2pd_client_udp_tunnels", MetricType::Gauge,
                          "Number of UDP tunnels by type");

  std::map<std::string, std::string> udpClientLabels;
  udpClientLabels["type"] = "client";
  udpTunnels.AddMetric(
      udpClientLabels,
      static_cast<double>(i2p::client::context.GetClientForwards().size()));

  std::map<std::string, std::string> udpServerLabels;
  udpServerLabels["type"] = "server";
  udpTunnels.AddMetric(
      udpServerLabels,
      static_cast<double>(i2p::client::context.GetServerForwards().size()));
  families.push_back(std::move(udpTunnels));

  // i2pd_client_sam_sessions
  MetricFamily samSessions("i2pd_client_sam_sessions", MetricType::Gauge,
                           "Number of active SAM sessions");
  auto samBridge = i2p::client::context.GetSAMBridge();
  if (samBridge) {
    samSessions.AddMetric(static_cast<double>(samBridge->GetSessions().size()));
  } else {
    samSessions.AddMetric(0.0);
  }
  families.push_back(std::move(samSessions));
}
} // namespace metrics
} // namespace i2p
