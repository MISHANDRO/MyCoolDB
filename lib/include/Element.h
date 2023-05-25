#pragma once

#include <ostream>

template<typename T>
class Element {
public:
    Element()
        : value_(T())
        , null_(true)
    {}

    explicit Element(T value)
            : value_(value)
    {}

    [[nodiscard]] T Value() const {
        return value_;
    }

    [[nodiscard]] bool IsNull() const {
        return null_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Element<T>& element) {
        if (element.IsNull()) {
            os << "NULL";
        } else if (std::is_same<T, std::string>::value) {
            os << '\'' << element.value_ << '\'';
        } else {
            os << element.value_;
        }

        return os;
    }

    bool operator==(const Element& other) const {
        return null_ == other.null_ && value_ == other.value_;
    }

private:
    T value_;
    bool null_ = false;
};
