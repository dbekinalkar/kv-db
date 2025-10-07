#pragma once
#include "CommandProcessor.h"
#include "KeyValueStore.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace server {

class Server {
public:
    explicit Server(int port);
    ~Server();

    bool start();
    void run();
    void stop();

private:
    bool setNonBlocking(int fd);
    void handleAccept();
    void handleClient(int fd);
    void processMessages(int fd);
    void sendResponse(int fd, const std::string& data);
    void closeClient(int fd);

    int port_;
    int epollFd_;
    int socketFd_;
    std::unique_ptr<storage::KeyValueStore> kvStore_;
    std::unique_ptr<command::CommandProcessor> processor_;
    std::unordered_map<int, std::string> clientBuffers_;
};

} // namespace server
