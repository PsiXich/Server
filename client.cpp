#include "client.hpp"

#include <cassert>
#include <boost/algorithm/string/trim.hpp>

Client::Client(const std::string_view prefix,const std::string_view name, bool create_dir) 
    : m_file(openFile(prefix, name, create_dir)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file << m_sequence << "---=== Client \"" << name << "\" ===---"
        << std::endl;
}

Client::~Client() {
    try {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file << m_sequence << "^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^" 
              << std::endl;
    } catch (...) {
        assert(false && "Failed to write shutdown message");
    }
}

void Client::putRecord(const std::string_view record) {
    m_file << m_sequence << boost::algorithm::trim_copy(record) << std::endl;
}

// Генерация имени файла
std::string Client::generateFilename(std::string_view prefix, std::string_view name) const {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    
    std::ostringstream filename;
    filename << prefix << "/"
             << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S_")
             << name << ".log";
    return filename.str();
}

// Открытие файла с созданием директории
std::ofstream Client::openFile(std::string_view prefix, std::string_view name, bool create_dir) {
    std::string filename = generateFilename(prefix, name);
    
    if (create_dir) {
        std::filesystem::path dir_path(prefix);
        if (!std::filesystem::exists(dir_path)) {
            std::filesystem::create_directories(dir_path);
        }
    }
    
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
    
    return file;
}