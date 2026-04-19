#ifndef METRICS_H__
#define METRICS_H__

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace i2p {
namespace metrics {
struct MetricFamily;

class MetricsServer {
public:
  MetricsServer(const std::string &address, uint16_t port);
  ~MetricsServer();

  void Start();
  void Stop();

private:
  void Run();
  void Accept();
  void HandleAccept(const boost::system::error_code &ecode,
                    std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void HandleRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  std::vector<MetricFamily> CollectMetrics();

private:
  std::string m_Address;
  uint16_t m_Port;
  bool m_IsRunning;
  std::unique_ptr<std::thread> m_Thread;
  boost::asio::io_context m_Service;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      m_Work;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> m_Acceptor;
};
} // namespace metrics
} // namespace i2p

#endif // METRICS_H__
