#include <iostream>
#include <asio.hpp>
#include <asio/ssl.hpp>

using asio::ip::tcp;

int main()
{
    try
    {
        asio::io_context io_context;
        asio::ssl::context ssl_ctx(asio::ssl::context::tlsv12_client);
        ssl_ctx.set_verify_mode(asio::ssl::verify_none);
        // 서버 IP와 포트 (원하는대로 수정)
        asio::ssl::stream<tcp::socket> ssl_stream(io_context,ssl_ctx);

        std::string server_ip = "210.119.12.82";
        int server_port = 9000;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(server_ip, std::to_string(server_port));
        asio::connect(ssl_stream.lowest_layer(), endpoints);
        std::cout << "서버 연결됨.\n";

        ssl_stream.handshake(asio::ssl::stream_base::client);
        std::cout << "TLS 핸드셰이크 완료\n";

        std::thread([&]() {
            while (true) {
                asio::streambuf buf;
                asio::read_until(ssl_stream, buf, "\n");

                std::istream is(&buf);
                std::string received_msg;
                std::getline(is, received_msg);

                std::cout << "수신: " << received_msg << "\n";
            }
            }).detach();
        std::string ID;
        std::cout << "ID > " << std::flush;
        std::cin >> ID;
        while (true)
        {
            std::string msg;
            std::cout << std::flush;
            std::cout << "> ";
            std::getline(std::cin, msg);

            msg = "CHAT " + ID + " " + msg + "\n";
            asio::write(ssl_stream, asio::buffer(msg));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "에러: " << e.what() << "\n";
    }

    return 0;
}
