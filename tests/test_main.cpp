#include "daemon/Metrics.h"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  try {
    std::cout << "Starting MetricsServer on 127.0.0.1:19600..." << std::endl;

    i2p::metrics::MetricsServer server("127.0.0.1", 19600);
    server.Start();

    std::cout << "Server started, running for 10 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Stopping server..." << std::endl;
    server.Stop();

    std::cout << "Server stopped successfully" << std::endl;
    return 0;
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
