#ifndef TRANSPORT_COLLECTOR_H__
#define TRANSPORT_COLLECTOR_H__

#include <vector>

namespace i2p {
namespace metrics {
struct MetricFamily;

void CollectTransportMetrics(std::vector<MetricFamily> &families);
} // namespace metrics
} // namespace i2p

#endif // TRANSPORT_COLLECTOR_H__
