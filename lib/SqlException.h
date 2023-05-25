#pragma once

#include <exception>
#include <string>
#include <utility>

class SqlException : public std::exception {
public:
    explicit SqlException(std::string message)
        : errorMessage(std::move(message))
    {}

    [[nodiscard]] const char* what() const noexcept override {
        return errorMessage.c_str();
    }

private:
    std::string errorMessage;
};
