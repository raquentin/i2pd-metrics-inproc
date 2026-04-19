#ifndef CLIENT_COLLECTOR_H__
#define CLIENT_COLLECTOR_H__

#include <vector>

namespace i2p {
namespace metrics {
struct MetricFamily;

void CollectClientMetrics(std::vector<MetricFamily> &families);
} // namespace metrics
} // namespace i2p

#endif // CLIENT_COLLECTOR_H__
