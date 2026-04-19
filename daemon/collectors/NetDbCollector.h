#ifndef NETDB_COLLECTOR_H__
#define NETDB_COLLECTOR_H__

#include <vector>

namespace i2p {
namespace metrics {
struct MetricFamily;

void CollectNetDbMetrics(std::vector<MetricFamily> &families);
} // namespace metrics
} // namespace i2p

#endif // NETDB_COLLECTOR_H__
