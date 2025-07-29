#pragma once

#include <fstream>
#include <string_view>
#include <filesystem>
#include <memory>
#include <mutex>

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
    Client(std::string_view prefix, std::string_view name, bool create_dir = true);
    ~Client();
    //запрет копирования
    Client(const Client&) = delete;
    Client(Client&&) = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = default;

    void putRecord(std::string_view record);

private:
    std::ofstream openFile(std::string_view prefix, std::string_view name, bool create_dir);
    std::string generateFilename(std::string_view prefix, std::string_view name) const;

    Sequence m_sequence;
    std::ofstream m_file;
    std::mutex m_mutex;
};