#include "MetricsRenderer.h"
#include <sstream>

namespace i2p {
namespace metrics {
static std::string EscapeLabelValue(const std::string &value) {
  std::string result;
  result.reserve(value.size());
  for (char c : value) {
    if (c == '\\' || c == '"' || c == '\n') {
      result += '\\';
      if (c == '\n')
        result += 'n';
      else
        result += c;
    } else
      result += c;
  }
  return result;
}

static std::string TypeToString(MetricType type) {
  switch (type) {
  case MetricType::Gauge:
    return "gauge";
  case MetricType::Counter:
    return "counter";
  case MetricType::Info:
    return "info";
  default:
    return "unknown";
  }
}

std::string RenderOpenMetrics(const std::vector<MetricFamily> &families) {
  std::ostringstream oss;

  for (const auto &family : families) {
    // HELP line
    oss << "# HELP " << family.name << " " << family.help << "\n";

    // TYPE line
    oss << "# TYPE " << family.name << " " << TypeToString(family.type) << "\n";

    // Metrics
    for (const auto &metric : family.metrics) {
      oss << family.name;

      // Labels
      if (!metric.labels.empty()) {
        oss << "{";
        bool first = true;
        for (const auto &label : metric.labels) {
          if (!first)
            oss << ",";
          first = false;
          oss << label.first << "=\"" << EscapeLabelValue(label.second) << "\"";
        }
        oss << "}";
      }

      // Value
      oss << " " << metric.value << "\n";
    }
  }

  // EOF marker
  oss << "# EOF\n";

  return oss.str();
}
} // namespace metrics
} // namespace i2p
