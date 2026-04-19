#include "NetDbCollector.h"
#include "../../submodules/i2pd/libi2pd/NetDb.hpp"
#include "../MetricsRenderer.h"

namespace i2p {
namespace metrics {
void CollectNetDbMetrics(std::vector<MetricFamily> &families) {
  // i2pd_netdb_routers
  MetricFamily routers("i2pd_netdb_routers", MetricType::Gauge,
                       "Number of known routers in NetDb");
  routers.AddMetric(static_cast<double>(i2p::data::netdb.GetNumRouters()));
  families.push_back(std::move(routers));

  // i2pd_netdb_floodfills
  MetricFamily floodfills("i2pd_netdb_floodfills", MetricType::Gauge,
                          "Number of known floodfill routers");
  floodfills.AddMetric(
      static_cast<double>(i2p::data::netdb.GetNumFloodfills()));
  families.push_back(std::move(floodfills));

  // i2pd_netdb_leasesets
  MetricFamily leasesets("i2pd_netdb_leasesets", MetricType::Gauge,
                         "Number of known LeaseSets");
  leasesets.AddMetric(static_cast<double>(i2p::data::netdb.GetNumLeaseSets()));
  families.push_back(std::move(leasesets));
}
} // namespace metrics
} // namespace i2p
