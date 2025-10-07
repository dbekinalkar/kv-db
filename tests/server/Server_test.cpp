#include <gtest/gtest.h>
#include "Server.h"
#include "Codec.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>

using namespace server;
using namespace codec;

namespace {
    int connectToServer(int port, int maxRetries = 20) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            return -1;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

        for (int i = 0; i < maxRetries; ++i) {
            if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
                return sock;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        close(sock);
        return -1;
    }
}

TEST(SimpleServerTest, TwoCommands) {
    constexpr int TEST_PORT = 9999;

    Server server(TEST_PORT);
    ASSERT_TRUE(server.start());

    std::thread serverThread([&server]() {
        server.run();
    });
    serverThread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int sock = connectToServer(TEST_PORT);
    ASSERT_NE(sock, -1) << "Failed to connect to server";

    // Command 1: SET key value
    CodecValue setCmd = array({bulk("SET"), bulk("mykey"), bulk("myvalue")});
    std::string setEncoded = Codec::encode(setCmd);

    ssize_t sent = send(sock, setEncoded.data(), setEncoded.size(), 0);
    ASSERT_EQ(sent, static_cast<ssize_t>(setEncoded.size())) << "Failed to send SET command";

    char buffer[4096];
    ssize_t received = recv(sock, buffer, sizeof(buffer), 0);
    ASSERT_GT(received, 0) << "Failed to receive SET response";

    std::string setResponse(buffer, received);
    CodecValue setResult = Codec::decode(setResponse);
    ASSERT_TRUE(std::holds_alternative<SimpleString>(setResult.data));
    EXPECT_EQ(std::get<SimpleString>(setResult.data).value, "OK");

    // Command 2: GET key
    CodecValue getCmd = array({bulk("GET"), bulk("mykey")});
    std::string getEncoded = Codec::encode(getCmd);

    sent = send(sock, getEncoded.data(), getEncoded.size(), 0);
    ASSERT_EQ(sent, static_cast<ssize_t>(getEncoded.size())) << "Failed to send GET command";

    received = recv(sock, buffer, sizeof(buffer), 0);
    ASSERT_GT(received, 0) << "Failed to receive GET response";

    std::string getResponse(buffer, received);
    CodecValue getResult = Codec::decode(getResponse);
    ASSERT_TRUE(std::holds_alternative<BulkString>(getResult.data));
    EXPECT_EQ(std::get<BulkString>(getResult.data).value, "myvalue");

    // Cleanup
    close(sock);
    server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
