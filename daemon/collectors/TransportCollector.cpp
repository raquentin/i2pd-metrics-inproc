#include "TransportCollector.h"
#include "../../submodules/i2pd/libi2pd/Transports.h"
#include "../MetricsRenderer.h"
#include <map>
#include <string>

namespace i2p {
namespace metrics {
void CollectTransportMetrics(std::vector<MetricFamily> &families) {
  // i2pd_transport_peers_total
  MetricFamily peersTotal("i2pd_transport_peers_total", MetricType::Gauge,
                          "Total number of connected peers");
  peersTotal.AddMetric(
      static_cast<double>(i2p::transport::transports.GetNumPeers()));
  families.push_back(std::move(peersTotal));

  // i2pd_transport_peers by transport type
  MetricFamily peers("i2pd_transport_peers", MetricType::Gauge,
                     "Number of peers by transport type");

  // Count NTCP2 peers
  auto ntcp2Server = i2p::transport::transports.GetNTCP2Server();
  if (ntcp2Server) {
    std::map<std::string, std::string> ntcp2Labels;
    ntcp2Labels["transport"] = "ntcp2";
    peers.AddMetric(ntcp2Labels, static_cast<double>(
                                     ntcp2Server->GetNTCP2Sessions().size()));
  }

  // Count SSU2 peers
  auto ssu2Server = i2p::transport::transports.GetSSU2Server();
  if (ssu2Server) {
    std::map<std::string, std::string> ssu2Labels;
    ssu2Labels["transport"] = "ssu2";
    peers.AddMetric(ssu2Labels,
                    static_cast<double>(ssu2Server->GetSSU2Sessions().size()));
  }
  families.push_back(std::move(peers));

  // i2pd_transport_bytes_total
  MetricFamily bytesTotal("i2pd_transport_bytes_total", MetricType::Counter,
                          "Total bytes transferred");
  std::map<std::string, std::string> sentLabels;
  sentLabels["direction"] = "sent";
  bytesTotal.AddMetric(
      sentLabels,
      static_cast<double>(i2p::transport::transports.GetTotalSentBytes()));

  std::map<std::string, std::string> receivedLabels;
  receivedLabels["direction"] = "received";
  bytesTotal.AddMetric(
      receivedLabels,
      static_cast<double>(i2p::transport::transports.GetTotalReceivedBytes()));
  families.push_back(std::move(bytesTotal));

  // i2pd_transport_transit_bytes_total
  MetricFamily transitBytes("i2pd_transport_transit_bytes_total",
                            MetricType::Counter,
                            "Total transit bytes transmitted");
  transitBytes.AddMetric(static_cast<double>(
      i2p::transport::transports.GetTotalTransitTransmittedBytes()));
  families.push_back(std::move(transitBytes));

  // i2pd_transport_bandwidth_bytes_per_second
  MetricFamily bandwidth("i2pd_transport_bandwidth_bytes_per_second",
                         MetricType::Gauge,
                         "Current bandwidth in bytes per second");
  std::map<std::string, std::string> inLabels;
  inLabels["direction"] = "in";
  bandwidth.AddMetric(
      inLabels,
      static_cast<double>(i2p::transport::transports.GetInBandwidth()));

  std::map<std::string, std::string> outLabels;
  outLabels["direction"] = "out";
  bandwidth.AddMetric(
      outLabels,
      static_cast<double>(i2p::transport::transports.GetOutBandwidth()));

  std::map<std::string, std::string> transitLabels;
  transitLabels["direction"] = "transit";
  bandwidth.AddMetric(
      transitLabels,
      static_cast<double>(i2p::transport::transports.GetTransitBandwidth()));
  families.push_back(std::move(bandwidth));

  // i2pd_transport_bandwidth_bytes_per_second_15s (short-term bandwidth)
  MetricFamily bandwidth15s(
      "i2pd_transport_bandwidth_bytes_per_second_15s", MetricType::Gauge,
      "Bandwidth over last 15 seconds in bytes per second");
  std::map<std::string, std::string> in15sLabels;
  in15sLabels["direction"] = "in";
  bandwidth15s.AddMetric(
      in15sLabels,
      static_cast<double>(i2p::transport::transports.GetInBandwidth15s()));

  std::map<std::string, std::string> out15sLabels;
  out15sLabels["direction"] = "out";
  bandwidth15s.AddMetric(
      out15sLabels,
      static_cast<double>(i2p::transport::transports.GetOutBandwidth15s()));

  std::map<std::string, std::string> transit15sLabels;
  transit15sLabels["direction"] = "transit";
  bandwidth15s.AddMetric(
      transit15sLabels,
      static_cast<double>(i2p::transport::transports.GetTransitBandwidth15s()));
  families.push_back(std::move(bandwidth15s));
}
} // namespace metrics
} // namespace i2p
