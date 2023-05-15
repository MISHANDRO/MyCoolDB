#pragma once

#include <exception>
#include <string>
#include <utility>

class QueryException : public std::exception {
private:
    std::string errorMessage;

public:
    explicit QueryException(std::string message)
        : errorMessage(std::move(message))
    {}

    [[nodiscard]] const char* what() const noexcept override {
        return errorMessage.c_str();
    }
};