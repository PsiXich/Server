
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

namespace {

    boost::asio::awaitable<void> listen() { 
        const auto executor = co_await boost::asio::this_coro::executor;

    }

    void runServer() {
        std::cout << std::this_thread::get_id() << "Running server..." << std::endl;

        boost::asio::io_context ioContext;

        //auto x = boost::asio::co_spawn(ioContext, listen, boost::asio::detached);

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