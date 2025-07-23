#pragma once

#include <fstream>
#include <string_view>

class Client {
    class Sequence {
        template<typename Char, typename Traits>
        friend std::basic_ostream<Char, Traits>& operator<<(
            std::basic_ostream<Char, Traits>& os,
            const Sequence&);

        std::size_t m_sequenceNo = 1;
    };

    template<typename Char, typename Traits>
    friend std::basic_ostream<Char, Traits>& operator<<(
        std::basic_ostream<Char, Traits>& os,
        const Client::Sequence&);

public:
    Client(std::string_view prefix, std::string_view name);
    ~Client();
    //запрет копирования
    Client(const Client&) = delete;
    Client(Client&&) = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = default;

    void putRecord(std::string_view record);

private:
    Sequence m_sequence;
    std::ofstream m_file;
};