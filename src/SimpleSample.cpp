#include <chrono>
#include <iostream>

#define ASIO_STANDLONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

using namespace std::chrono_literals;

const char* PAUSE = "read -n 1 -s -p \"Press any key to continue...\"";

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket);


int main(void) {

    asio::error_code err;

    // 建立新的 "context" - 平台溝通的基礎環境 
    // Create a "context" - essentially the platform specific interface
    asio::io_context context;

    // Give some fake tasks to asio to the context doesnt finish
    asio::io_context::work idleWork(context);

    // Start thread
    std::thread thrContext = std::thread([&]() { context.run(); });

    // 取得想要連線的 IP 位址
    // Get the address of somewhere we wish to connect to
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49"), 80);

    // 根據我們的 Context 建立 socket
    // Create a socket, the context will deliver the implementation
    asio::ip::tcp::socket socket(context);

    // Tell socket to try and connect
    socket.connect(endpoint, err);

    if (!err) {
        std::cout << "連線成功！" << std::endl;
    } else {
        std::cout << "連線 "  << err.message() << " 失敗" << std::endl;
    }

    if (socket.is_open())
    {
        GrabSomeData(socket);

        std::string sRequest =
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";

        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), err);
        
        // socket.wait(socket.wait_read);

        std::this_thread::sleep_for(2000ms);

        context.stop();
        if (thrContext.joinable())
            thrContext.join();
    }

    system(PAUSE);
};

void GrabSomeData(asio::ip::tcp::socket& socket) {
    socket.async_read_some(
        asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code err, std::size_t length)
        {
            if (!err)
            {
                std::cout << "\n\nRead " << length << "bytes\n\n";

                for (int i = 0; i < length; i ++)
                    std::cout << vBuffer[i];

                GrabSomeData(socket);
            }
        });
};