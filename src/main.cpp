#include "server/Server.h"
#include <iostream>
#include <csignal>
#include <atomic>

constexpr int DEFAULT_PORT = 6379;

std::atomic<server::Server*> g_server(nullptr);

void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nReceived interrupt signal, shutting down server..." << std::endl;
        server::Server* srv = g_server.load();
        if (srv) {
            srv->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
            return 1;
        }
    }

    server::Server server(port);
    g_server.store(&server);

    std::signal(SIGINT, signalHandler);

    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    server.run();

    g_server.store(nullptr);
    return 0;
}
