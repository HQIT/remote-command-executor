#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/format.hpp>

namespace b = boost;

typedef class command_executor {
public:
    int operator () (const std::string& command_line) {
        FILE* stream = popen(command_line.data(), "r");
        if(!stream) {
            std::cerr << "failed to execute command-line \n";
            return -1;
        }

        std::string response;
        char buffer[1024] = {0};
        while((fgets(buffer, 1024, stream)) != nullptr) {
            response.append(buffer);
        }

        int code = pclose(stream);

        if(on_response_ready) on_response_ready(code, response);

        return code;
    }

    void on(std::function<void (int, const std::string&)> callback) {
        on_response_ready = callback;
    }

private:
    std::function<void (int, const std::string&)> on_response_ready = nullptr;
} command_executor_t;


class cli_adapter {
    static cli_adapter* instance;

    volatile bool quit;
    b::thread runner;

protected:
    void on_command(b::asio::ip::tcp::socket& socket, const std::string& cmdl) {
        command_executor executor;
        executor.on([&](int code, const std::string& response) {
            b::system::error_code error;

            std::string data(response);
            if(code) {
                data = (b::format("bad:%d") % code).str();
            }

            size_t count = socket.write_some(b::asio::buffer(data), error);
            if(error) std::cerr << "on_command:" << count << "," << error.message() << "\n";
        });
        executor(cmdl);
    }

    void on_connected(b::shared_ptr<b::asio::ip::tcp::socket> socket) {
        std::string cmdl;

        b::array<char, 128> buffer;
        b::system::error_code error;
        do {
            size_t length = socket->read_some(b::asio::buffer(buffer), error);
            cmdl.append(buffer.data(), length);

            if(std::string::npos != cmdl.find_last_of("\r\n\r\n")) {
                on_command(*socket, cmdl.substr(0, cmdl.length() - 4));
                cmdl.clear();
            }
        } while(!error);

        if(error) std::cerr << "on_connected:" << error.message() << "\n";
    }

public:

    static cli_adapter* global() {
        return instance ? instance : instance = new cli_adapter();
    }

    void loop(int port) {
        b::asio::io_service io_service;
        b::asio::ip::tcp::acceptor acceptor(io_service,
                                            b::asio::ip::tcp::endpoint(
                                                    b::asio::ip::tcp::v4(), port));
        while(!quit) {
            b::shared_ptr<b::asio::ip::tcp::socket> socket(new b::asio::ip::tcp::socket(io_service));
            acceptor.accept(*socket.get());

            b::thread(b::bind(&cli_adapter::on_connected, this, socket));
        }
    }

    int start(int port = 12315) {
        runner = b::thread(b::bind(&cli_adapter::loop, this, port));
        return 0;
    }

    void stop() {
        quit = true;
    }

    void join() {
        runner.join();
    }
};

cli_adapter* cli_adapter::instance = nullptr;

int main(int argc, char* argv[]) {
    if(!argc > 1) {
        std::cerr << "missing command-line \n";
        return 1;
    }

    std::unique_ptr<cli_adapter> cli(cli_adapter::global());
    cli->start(12315);
    cli->join();

    return 0;
}