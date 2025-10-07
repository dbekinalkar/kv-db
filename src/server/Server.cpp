#include "Server.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

namespace server {

constexpr int MAX_EVENTS = 64;
constexpr int BUFFER_SIZE = 4096;

Server::Server(int port)
    : port_(port), epollFd_(-1), socketFd_(-1) {
    kvStore_ = std::make_unique<storage::KeyValueStore>();
    processor_ = std::make_unique<command::CommandProcessor>(*kvStore_);
}

Server::~Server() {
    stop();
}

bool Server::start() {
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    int opt = 1;
    if (setsockopt(socketFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
        close(socketFd_);
        return false;
    }

    if (!setNonBlocking(socketFd_)) {
        close(socketFd_);
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(socketFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(socketFd_);
        return false;
    }

    if (listen(socketFd_, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen: " << strerror(errno) << std::endl;
        close(socketFd_);
        return false;
    }

    epollFd_ = epoll_create1(0);
    if (epollFd_ == -1) {
        std::cerr << "Failed to create epoll: " << strerror(errno) << std::endl;
        close(socketFd_);
        return false;
    }

    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = socketFd_;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, socketFd_, &ev) == -1) {
        std::cerr << "Failed to add listen socket to epoll: " << strerror(errno) << std::endl;
        close(socketFd_);
        close(epollFd_);
        return false;
    }

    std::cout << "Server listening on port " << port_ << std::endl;
    return true;
}

void Server::run() {
    epoll_event events[MAX_EVENTS];

    while (true) {
        int nfds = epoll_wait(epollFd_, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR) continue;
            std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == socketFd_) {
                handleAccept();
            } else {
                handleClient(events[i].data.fd);
            }
        }
    }
}

void Server::stop() {
    for (const auto& [fd, _] : clientBuffers_) {
        close(fd);
    }
    clientBuffers_.clear();

    if (epollFd_ != -1) {
        close(epollFd_);
        epollFd_ = -1;
    }

    if (socketFd_ != -1) {
        close(socketFd_);
        socketFd_ = -1;
    }
}

bool Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags: " << strerror(errno) << std::endl;
        return false;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void Server::handleAccept() {
    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(socketFd_, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);

        if (clientFd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                break;
            }
        }

        std::cout << "New connection: fd=" << clientFd << std::endl;

        if (!setNonBlocking(clientFd)) {
            close(clientFd);
            continue;
        }

        epoll_event ev{};
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = clientFd;
        if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
            std::cerr << "Failed to add client to epoll: " << strerror(errno) << std::endl;
            close(clientFd);
            continue;
        }

        clientBuffers_[clientFd] = "";
    }
}

void Server::handleClient(int fd) {
    char buffer[BUFFER_SIZE];

    while (true) {
        ssize_t n = read(fd, buffer, sizeof(buffer));

        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                std::cerr << "Read error: " << strerror(errno) << std::endl;
                closeClient(fd);
                return;
            }
        } else if (n == 0) {
            std::cout << "Client disconnected: fd=" << fd << std::endl;
            closeClient(fd);
            return;
        }

        clientBuffers_[fd].append(buffer, n);
    }

    processMessages(fd);
}

void Server::processMessages(int fd) {
    std::string& buffer = clientBuffers_[fd];

    codec::CodecValue request = codec::Codec::decode(buffer);

    codec::CodecValue response = processor_->process(request);

    std::string encoded = codec::Codec::encode(response);
    sendResponse(fd, encoded);
    buffer = "";
}

void Server::sendResponse(int fd, const std::string& data) {
    size_t totalSent = 0;
    size_t remaining = data.size();

    while (totalSent < data.size()) {
        ssize_t n = write(fd, data.data() + totalSent, remaining);

        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                std::cerr << "Write error: " << strerror(errno) << std::endl;
                closeClient(fd);
                return;
            }
        }

        totalSent += n;
        remaining -= n;
    }
}

void Server::closeClient(int fd) {
    epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
    clientBuffers_.erase(fd);
}

} // namespace server