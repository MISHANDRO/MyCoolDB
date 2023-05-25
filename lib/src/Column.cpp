#include "Column.h"


template<>
std::string Column<std::string>::GetValType(const std::string& data) const {
    if (data.front() != '\'' || data.back() != '\'') {
        throw SqlException("VARCHAR must be enclosed in quotation marks '");
    }

    return data.substr(1, data.size() - 2);
}

template<>
int Column<int>::GetValType(const std::string& data) const {
    return std::strtol(data.c_str(), nullptr, 10);
}

template<>
bool Column<bool>::GetValType(const std::string& data) const {
    if (data == "1" || data == "true") {
        return true;
    }
    if (data == "0" || data == "false") {
        return false;
    }

    throw SqlException("Bad boolean value");
}

template<>
double Column<double>::GetValType(const std::string& data) const {
    return std::strtod(data.c_str(), nullptr);
}

template<>
float Column<float>::GetValType(const std::string& data) const {
    return std::strtof(data.c_str(), nullptr);
}
