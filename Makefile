CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCFLAGS = -I. -Isubmodules/i2pd -Isubmodules/i2pd/libi2pd -Isubmodules/i2pd/libi2pd_client -Isubmodules/i2pd/daemon -Isubmodules/i2pd/i18n

OBJS = daemon/Metrics.o daemon/MetricsRenderer.o daemon/collectors/RouterCollector.o daemon/collectors/TunnelCollector.o daemon/collectors/TransportCollector.o daemon/collectors/NetDbCollector.o daemon/collectors/ClientCollector.o

all: $(OBJS)

daemon/Metrics.o: daemon/Metrics.cpp daemon/Metrics.h daemon/MetricsRenderer.h daemon/collectors/RouterCollector.h daemon/collectors/TunnelCollector.h daemon/collectors/TransportCollector.h daemon/collectors/NetDbCollector.h daemon/collectors/ClientCollector.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/Metrics.cpp -o daemon/Metrics.o

daemon/MetricsRenderer.o: daemon/MetricsRenderer.cpp daemon/MetricsRenderer.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/MetricsRenderer.cpp -o daemon/MetricsRenderer.o

daemon/collectors/RouterCollector.o: daemon/collectors/RouterCollector.cpp daemon/collectors/RouterCollector.h daemon/MetricsRenderer.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/collectors/RouterCollector.cpp -o daemon/collectors/RouterCollector.o

daemon/collectors/TunnelCollector.o: daemon/collectors/TunnelCollector.cpp daemon/collectors/TunnelCollector.h daemon/MetricsRenderer.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/collectors/TunnelCollector.cpp -o daemon/collectors/TunnelCollector.o

daemon/collectors/TransportCollector.o: daemon/collectors/TransportCollector.cpp daemon/collectors/TransportCollector.h daemon/MetricsRenderer.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/collectors/TransportCollector.cpp -o daemon/collectors/TransportCollector.o

daemon/collectors/NetDbCollector.o: daemon/collectors/NetDbCollector.cpp daemon/collectors/NetDbCollector.h daemon/MetricsRenderer.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/collectors/NetDbCollector.cpp -o daemon/collectors/NetDbCollector.o

daemon/collectors/ClientCollector.o: daemon/collectors/ClientCollector.cpp daemon/collectors/ClientCollector.h daemon/MetricsRenderer.h
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c daemon/collectors/ClientCollector.cpp -o daemon/collectors/ClientCollector.o

test: daemon/MetricsRenderer.o
	@echo "Building unit tests..."
	$(CXX) $(CXXFLAGS) $(INCFLAGS) tests/test_all.cpp daemon/MetricsRenderer.o -o tests/test_all
	$(CXX) $(CXXFLAGS) $(INCFLAGS) tests/test_renderer.cpp daemon/MetricsRenderer.o -o tests/test_renderer
	@echo "Running tests..."
	@./tests/test_all
	@./tests/test_renderer

clean:
	rm -f $(OBJS)
	rm -f tests/test_all tests/test_renderer tests/stubs.o

.PHONY: all clean test
