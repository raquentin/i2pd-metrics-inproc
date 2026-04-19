#include "daemon/MetricsRenderer.h"
#include <cassert>
#include <iostream>

int main() {
  using namespace i2p::metrics;

  std::vector<MetricFamily> families;

  // [1]: simple gauge without labels
  MetricFamily upMetric("i2pd_up", MetricType::Gauge, "Router is up");
  upMetric.AddMetric(1.0);
  families.push_back(std::move(upMetric));

  // [2]: counter with labels
  MetricFamily bytesMetric("i2pd_bytes_total", MetricType::Counter,
                           "Total bytes transferred");
  bytesMetric.AddMetric({{"direction", "in"}}, 1234.0);
  bytesMetric.AddMetric({{"direction", "out"}}, 5678.0);
  families.push_back(std::move(bytesMetric));

  // [3]: info metric with multiple labels
  MetricFamily versionMetric("i2pd_version", MetricType::Info,
                             "Router version information");
  versionMetric.AddMetric({{"version", "2.59.0"}, {"commit", "abc123"}}, 1.0);
  families.push_back(std::move(versionMetric));

  std::string output = RenderOpenMetrics(families);
  std::cout << output << std::endl;

  // verify
  assert(output.find("# HELP i2pd_up Router is up") != std::string::npos);
  assert(output.find("# TYPE i2pd_up gauge") != std::string::npos);
  assert(output.find("i2pd_up 1") != std::string::npos);
  assert(output.find("# HELP i2pd_bytes_total Total bytes transferred") !=
         std::string::npos);
  assert(output.find("# TYPE i2pd_bytes_total counter") != std::string::npos);
  assert(output.find("i2pd_bytes_total{direction=\"in\"} 1234") !=
         std::string::npos);
  assert(output.find("i2pd_bytes_total{direction=\"out\"} 5678") !=
         std::string::npos);
  assert(output.find("# EOF") != std::string::npos);

  std::cout << "All renderer tests passed!" << std::endl;
  return 0;
}
