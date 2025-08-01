#include "client.hpp"

#include <cassert>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>

Client::Client(const std::string_view prefix,const std::string_view name, bool create_dir) 
    : m_file(openFile(prefix, name, create_dir)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file << m_sequence << "---=== Client \"" << name << "\" ===---" << std::endl;
        m_file.flush(); // Written immediatly
}

Client::~Client() {
    try {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file << m_sequence << "^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^" << std::endl;
        m_file.flush();
        if (!m_file) {
            std::cerr << "Failed to write shutdown message" << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Exception in Client destructor: " << ex.what() << std::endl;
    }
}

void Client::putRecord(std::string_view record) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_file << m_sequence << record << std::endl;
    m_file.flush(); // Written immediately
    if (!m_file) {
        std::cerr << "Failed to write to log file" << std::endl;
    }
}

// Генерация имени файла
std::string Client::generateFilename(std::string_view prefix, std::string_view name) const {
    std::ostringstream filename;
    filename << prefix << "/" << name << ".log";
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

template<typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
    std::basic_ostream<Char, Traits>& os,
    const Client::Sequence& seq) {
    os << "[" << std::setw(4) << std::setfill('0') << seq.m_sequenceNo << "] ";
    const_cast<Client::Sequence&>(seq).m_sequenceNo++;
    
    return os;
}