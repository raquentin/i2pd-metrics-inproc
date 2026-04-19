// Comprehensive unit tests for i2pd metrics

#include "daemon/MetricsRenderer.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace i2p::metrics;

int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running " #name "... "; \
        try { \
            test_##name(); \
            std::cout << "[pass]" << std::endl; \
            tests_passed++; \
        } catch (const std::exception& e) { \
            std::cout << "[fail] " << e.what() << std::endl; \
            tests_failed++; \
        } \
    } \
    void test_##name()

#define ASSERT_TRUE(expr) \
    if (!(expr)) { \
        throw std::runtime_error("Assertion failed: " #expr); \
    }

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { \
        throw std::runtime_error("Assertion failed: " #a " == " #b); \
    }

#define ASSERT_CONTAINS(str, substr) \
    if ((str).find(substr) == std::string::npos) { \
        throw std::runtime_error("String doesn't contain: " substr); \
    }

// Test: Empty metrics
TEST(empty_metrics) {
    std::vector<MetricFamily> families;
    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "# EOF");
    ASSERT_EQ(output, "# EOF\n");
}

// Test: Single gauge without labels
TEST(gauge_no_labels) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("test_gauge", MetricType::Gauge, "A test gauge");
    gauge.AddMetric(42.0);
    families.push_back(std::move(gauge));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "# HELP test_gauge A test gauge");
    ASSERT_CONTAINS(output, "# TYPE test_gauge gauge");
    ASSERT_CONTAINS(output, "test_gauge 42");
    ASSERT_CONTAINS(output, "# EOF");
}

// Test: Counter with labels
TEST(counter_with_labels) {
    std::vector<MetricFamily> families;

    MetricFamily counter("test_counter_total", MetricType::Counter, "A test counter");
    counter.AddMetric({{"label", "value1"}}, 100.0);
    counter.AddMetric({{"label", "value2"}}, 200.0);
    families.push_back(std::move(counter));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "# HELP test_counter_total A test counter");
    ASSERT_CONTAINS(output, "# TYPE test_counter_total counter");
    ASSERT_CONTAINS(output, "test_counter_total{label=\"value1\"} 100");
    ASSERT_CONTAINS(output, "test_counter_total{label=\"value2\"} 200");
}

// Test: Info metric with multiple labels
TEST(info_with_multiple_labels) {
    std::vector<MetricFamily> families;

    MetricFamily info("test_info", MetricType::Info, "Test info");
    info.AddMetric({{"version", "1.0"}, {"build", "debug"}}, 1.0);
    families.push_back(std::move(info));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "# TYPE test_info info");
    ASSERT_CONTAINS(output, "version=\"1.0\"");
    ASSERT_CONTAINS(output, "build=\"debug\"");
}

// Test: Label value escaping
TEST(label_value_escaping) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("test_escape", MetricType::Gauge, "Test escaping");
    gauge.AddMetric({{"label", "value with \"quotes\""}}, 1.0);
    families.push_back(std::move(gauge));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "label=\"value with \\\"quotes\\\"\"");
}

// Test: Newline escaping in labels
TEST(newline_escaping) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("test_newline", MetricType::Gauge, "Test newline");
    gauge.AddMetric({{"label", "line1\nline2"}}, 1.0);
    families.push_back(std::move(gauge));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "label=\"line1\\nline2\"");
}

// Test: Multiple metric families
TEST(multiple_families) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("metric1", MetricType::Gauge, "First metric");
    gauge.AddMetric(1.0);
    families.push_back(std::move(gauge));

    MetricFamily counter("metric2_total", MetricType::Counter, "Second metric");
    counter.AddMetric(2.0);
    families.push_back(std::move(counter));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "# HELP metric1 First metric");
    ASSERT_CONTAINS(output, "# HELP metric2_total Second metric");
    ASSERT_CONTAINS(output, "metric1 1");
    ASSERT_CONTAINS(output, "metric2_total 2");
}

// Test: Floating point values
TEST(floating_point_values) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("test_float", MetricType::Gauge, "Test float");
    gauge.AddMetric(3.14159);
    families.push_back(std::move(gauge));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "test_float 3.14159");
}

// Test: Zero values
TEST(zero_values) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("test_zero", MetricType::Gauge, "Test zero");
    gauge.AddMetric(0.0);
    families.push_back(std::move(gauge));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "test_zero 0");
}

// Test: Metric type strings
TEST(metric_type_strings) {
    std::vector<MetricFamily> families;

    MetricFamily gauge("g", MetricType::Gauge, "g");
    gauge.AddMetric(1.0);
    families.push_back(std::move(gauge));

    MetricFamily counter("c", MetricType::Counter, "c");
    counter.AddMetric(1.0);
    families.push_back(std::move(counter));

    MetricFamily info("i", MetricType::Info, "i");
    info.AddMetric(1.0);
    families.push_back(std::move(info));

    std::string output = RenderOpenMetrics(families);

    ASSERT_CONTAINS(output, "# TYPE g gauge");
    ASSERT_CONTAINS(output, "# TYPE c counter");
    ASSERT_CONTAINS(output, "# TYPE i info");
}

int main() {
    std::cout << "=== i2pd Metrics Unit Tests ===" << std::endl << std::endl;

    run_test_empty_metrics();
    run_test_gauge_no_labels();
    run_test_counter_with_labels();
    run_test_info_with_multiple_labels();
    run_test_label_value_escaping();
    run_test_newline_escaping();
    run_test_multiple_families();
    run_test_floating_point_values();
    run_test_zero_values();
    run_test_metric_type_strings();

    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "[pass] " << tests_passed << " tests passed" << std::endl;
    if (tests_failed > 0) {
        std::cout << "[fail] " << tests_failed << " tests failed" << std::endl;
        return 1;
    }
    std::cout << std::endl << "All tests passed!" << std::endl;
    return 0;
}
