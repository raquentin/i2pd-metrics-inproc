#ifndef METRICS_RENDERER_H__
#define METRICS_RENDERER_H__

#include <map>
#include <string>
#include <vector>

namespace i2p {
namespace metrics {
enum class MetricType { Gauge, Counter, Info };

struct Metric {
  std::map<std::string, std::string> labels;
  double value;

  Metric(double val) : value(val) {}
  Metric(const std::map<std::string, std::string> &lbls, double val)
      : labels(lbls), value(val) {}
};

struct MetricFamily {
  std::string name;
  MetricType type;
  std::string help;
  std::vector<Metric> metrics;

  MetricFamily(const std::string &n, MetricType t, const std::string &h)
      : name(n), type(t), help(h) {}

  void AddMetric(double value) { metrics.emplace_back(value); }

  void AddMetric(const std::map<std::string, std::string> &labels,
                 double value) {
    metrics.emplace_back(labels, value);
  }
};

std::string RenderOpenMetrics(const std::vector<MetricFamily> &families);
} // namespace metrics
} // namespace i2p

#endif // METRICS_RENDERER_H__
