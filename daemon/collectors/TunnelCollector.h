#ifndef TUNNEL_COLLECTOR_H__
#define TUNNEL_COLLECTOR_H__

#include <vector>

namespace i2p {
namespace metrics {
struct MetricFamily;

void CollectTunnelMetrics(std::vector<MetricFamily> &families);
} // namespace metrics
} // namespace i2p

#endif // TUNNEL_COLLECTOR_H__
