#include "TunnelCollector.h"
#include "../../submodules/i2pd/libi2pd/Tunnel.h"
#include "../MetricsRenderer.h"

namespace i2p {
namespace metrics {
void CollectTunnelMetrics(std::vector<MetricFamily> &families) {
  // i2pd_tunnels_participating
  MetricFamily participating(
      "i2pd_tunnels_participating", MetricType::Gauge,
      "Number of tunnels this router is participating in");
  participating.AddMetric(
      static_cast<double>(i2p::tunnel::tunnels.CountTransitTunnels()));
  families.push_back(std::move(participating));

  // i2pd_tunnels_inbound
  MetricFamily inbound("i2pd_tunnels_inbound", MetricType::Gauge,
                       "Number of inbound tunnels");
  inbound.AddMetric(
      static_cast<double>(i2p::tunnel::tunnels.CountInboundTunnels()));
  families.push_back(std::move(inbound));

  // i2pd_tunnels_outbound
  MetricFamily outbound("i2pd_tunnels_outbound", MetricType::Gauge,
                        "Number of outbound tunnels");
  outbound.AddMetric(
      static_cast<double>(i2p::tunnel::tunnels.CountOutboundTunnels()));
  families.push_back(std::move(outbound));

  // i2pd_tunnels_queue_size
  MetricFamily queue("i2pd_tunnels_queue_size", MetricType::Gauge,
                     "Number of messages in tunnel processing queue");
  queue.AddMetric(static_cast<double>(i2p::tunnel::tunnels.GetQueueSize()));
  families.push_back(std::move(queue));

  // i2pd_tunnels_build_queue_size
  MetricFamily buildQueue("i2pd_tunnels_build_queue_size", MetricType::Gauge,
                          "Number of messages in tunnel build queue");
  buildQueue.AddMetric(
      static_cast<double>(i2p::tunnel::tunnels.GetTBMQueueSize()));
  families.push_back(std::move(buildQueue));

  // i2pd_tunnels_creation_success_rate
  MetricFamily successRate("i2pd_tunnels_creation_success_rate",
                           MetricType::Gauge,
                           "Tunnel creation success rate (0-100)");
  successRate.AddMetric(
      i2p::tunnel::tunnels.GetPreciseTunnelCreationSuccessRate());
  families.push_back(std::move(successRate));
}
} // namespace metrics
} // namespace i2p
