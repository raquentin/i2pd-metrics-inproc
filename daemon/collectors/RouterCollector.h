#ifndef ROUTER_COLLECTOR_H__
#define ROUTER_COLLECTOR_H__

#include <vector>

namespace i2p {
namespace metrics {
struct MetricFamily;

void CollectRouterMetrics(std::vector<MetricFamily> &families);
} // namespace metrics
} // namespace i2p

#endif // ROUTER_COLLECTOR_H__
