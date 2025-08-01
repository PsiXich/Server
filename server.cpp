#include "setting.hpp"
#include "client.hpp"

#define BOOST_ASIO_DISABLE_IOCP
#include <boost/asio/io_context.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio.hpp>


#include <iostream>
#include <thread>

using tcp = boost::asio::ip::tcp;

namespace {

    boost::asio::awaitable<void> workClient(tcp::socket socket) {
        try {
            boost::asio::streambuf buffer;

            // Read the client
            auto bytes_read = co_await boost::asio::async_read_until(socket, buffer, "\n", boost::asio::use_awaitable);
            auto bufs = buffer.data();
            std::string name(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + bytes_read);
            boost::algorithm::trim_all(name); // Trim whitespace and newlines

            if (name.empty() || name.find("..") != std::string::npos || name.find('/') != std::string::npos) {
                std::cerr << "Invalid client name: " << name << std::endl;
                co_return;
            }

            Client client("test", name);

            buffer.consume(bytes_read);

            // Process subsequent log records
            for (;;) {
                bytes_read = co_await boost::asio::async_read_until(socket, buffer, "\n", boost::asio::use_awaitable);
                auto const_buf = buffer.data();
                std::string logRecord(static_cast<const char*>(const_buf.data()), bytes_read);
                boost::algorithm::trim(logRecord);
                if (!logRecord.empty()) {
                    client.putRecord(logRecord);
                }
                buffer.consume(bytes_read);
            }
        } catch (const boost::system::system_error& ex) {
            // Client disconnection
            if (ex.code() != boost::asio::error::eof &&
                ex.code() != boost::asio::error::connection_reset) {
                std::cerr << "Client error: " << ex.what() << std::endl;
            }
        } catch (const std::exception& ex) {
            std::cerr << "Client exception: " << ex.what() << std::endl;
        }
    }

    boost::asio::awaitable<void> listen() {
        // Object performer
        const auto executor = co_await boost::asio::this_coro::executor;

        tcp::acceptor acceptor{executor, {tcp::v4(), port}};

        for(;;) {
            // Get socket
            tcp::socket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
            boost::asio::co_spawn(executor, workClient(std::move(socket)), boost::asio::detached);
        }

    }

    void runServer() {
        std::cout << std::this_thread::get_id() << "Running server..." << std::endl;
        // ProActor
        boost::asio::io_context ioContext;
        // For shutdown
        boost::asio::signal_set signals(ioContext, SIGINT, SIGTERM);
        signals.async_wait([&](const boost::system::error_code&, int) {
            ioContext.stop();
        });

        boost::asio::co_spawn(ioContext, listen, boost::asio::detached);

        ioContext.run();

        std::cout << std::this_thread::get_id() << "Server stopped" << std::endl;
    }

} // namespace

auto main() -> int {
    try {
        runServer();
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << "\"." << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }

    return EXIT_FAILURE;
}