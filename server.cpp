
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
#include "setting.hpp"

using tcp = boost::asio::ip::tcp;

namespace {

    boost::asio::awaitable<void> workClient(tcp::socket socket) {

    }

    boost::asio::awaitable<void> listen() {
        //object performer
        const auto executor = co_await boost::asio::this_coro::executor;

        tcp::acceptor acceptor{executor, {tcp::v4(), port}};

        for(;;) {
            //get socket
            tcp::socket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
            boost::asio::co_spawn(executor, workClient(std::move(socket)), boost::asio::detached);
        }

    }

    void runServer() {
        std::cout << std::this_thread::get_id() << "Running server..." << std::endl;
        //ProActor
        boost::asio::io_context ioContext;

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