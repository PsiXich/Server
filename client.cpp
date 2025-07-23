#include "client.hpp"

#include <cassert>
#include <boost/algorithm/string/trim.hpp>

Client::Client(const std::string_view prefix,const std::string_view name) 
    : m_file(openFile(prefix, name)) {
        m_file << m_sequence << "---=== Client \"" << name << "\" ===---"
        << std::endl;
}

Client::~Client() {
    try {
        m_file << m_sequence << "^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^" 
              << std::endl;
    } catch (...) {
        assert(false);
    }
}

void Client::putRecord(const std::string_view record) {
    m_file << m_sequence << boost::algorithm::trim_copy(record) << std::endl;
}
