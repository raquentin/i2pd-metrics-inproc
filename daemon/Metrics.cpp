#include "Metrics.h"
#include "MetricsRenderer.h"
#include "collectors/ClientCollector.h"
#include "collectors/NetDbCollector.h"
#include "collectors/RouterCollector.h"
#include "collectors/TransportCollector.h"
#include "collectors/TunnelCollector.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace i2p {
namespace metrics {
MetricsServer::MetricsServer(const std::string &address, uint16_t port)
    : m_Address(address), m_Port(port), m_IsRunning(false), m_Thread(nullptr),
      m_Work(m_Service.get_executor()) {}

MetricsServer::~MetricsServer() { Stop(); }

void MetricsServer::Start() {
  if (m_IsRunning)
    return;

  m_IsRunning = true;
  m_Acceptor = std::make_unique<tcp::acceptor>(
      m_Service, tcp::endpoint(net::ip::make_address(m_Address), m_Port));
  m_Thread.reset(new std::thread(std::bind(&MetricsServer::Run, this)));
  m_Acceptor->listen();
  Accept();
}

void MetricsServer::Stop() {
  if (!m_IsRunning)
    return;

  m_IsRunning = false;

  if (m_Acceptor) {
    boost::system::error_code ec;
    m_Acceptor->cancel(ec);
    m_Acceptor->close();
  }

  m_Service.stop();
  if (m_Thread) {
    m_Thread->join();
    m_Thread = nullptr;
  }
}

void MetricsServer::Run() {
  while (m_IsRunning) {
    try {
      m_Service.run();
    } catch (std::exception &ex) {
      std::cerr << "MetricsServer: Runtime exception: " << ex.what()
                << std::endl;
    }
  }
}

void MetricsServer::Accept() {
  auto socket = std::make_shared<tcp::socket>(m_Service);
  m_Acceptor->async_accept(*socket,
                           std::bind(&MetricsServer::HandleAccept, this,
                                     std::placeholders::_1, socket));
}

void MetricsServer::HandleAccept(const boost::system::error_code &ecode,
                                 std::shared_ptr<tcp::socket> socket) {
  if (!ecode) {
    HandleRequest(socket);
  } else {
    if (socket)
      socket->close();
    std::cerr << "MetricsServer: Accept error: " << ecode.message()
              << std::endl;
  }

  if (m_IsRunning)
    Accept();
}

std::vector<MetricFamily> MetricsServer::CollectMetrics() {
  std::vector<MetricFamily> families;

  // Collect router metrics
  CollectRouterMetrics(families);

  // Collect tunnel metrics
  CollectTunnelMetrics(families);

  // Collect transport metrics
  CollectTransportMetrics(families);

  // Collect NetDb metrics
  CollectNetDbMetrics(families);

  // Collect client metrics
  CollectClientMetrics(families);

  return families;
}

void MetricsServer::HandleRequest(std::shared_ptr<tcp::socket> socket) {
  auto buffer = std::make_shared<beast::flat_buffer>();
  auto req = std::make_shared<http::request<http::string_body>>();

  http::async_read(
      *socket, *buffer, *req,
      [this, socket, buffer, req](beast::error_code ec, std::size_t) {
        if (ec) {
          socket->close();
          return;
        }

        auto res = std::make_shared<http::response<http::string_body>>();
        res->version(req->version());
        res->keep_alive(false);

        if (req->target() == "/metrics") {
          res->result(http::status::ok);
          res->set(
              http::field::content_type,
              "application/openmetrics-text; version=1.0.0; charset=utf-8");
          auto families = CollectMetrics();
          res->body() = RenderOpenMetrics(families);
        } else {
          res->result(http::status::not_found);
          res->set(http::field::content_type, "text/plain");
          res->body() = "404 Not Found\n";
        }

        res->prepare_payload();

        http::async_write(
            *socket, *res, [socket, res](beast::error_code, std::size_t) {
              beast::error_code shutdown_ec;
              socket->shutdown(tcp::socket::shutdown_send, shutdown_ec);
              socket->close();
            });
      });
}
} // namespace metrics
} // namespace i2p
