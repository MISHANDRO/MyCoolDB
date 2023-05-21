#pragma once

#include <ostream>

template<typename T>
class Element {
public:
    Element()
            : value_(T())
            , null(true)
    {}

    explicit Element(T value)
            : value_(value)
    {}

    [[nodiscard]] T Value() const {
        return value_;
    }

    [[nodiscard]] bool IsNull() const {
        return null;
    }

    friend std::ostream& operator<<(std::ostream& os, const Element& element) {
        if (element.IsNull()) {
            os << "NULL";
        } else {
            os << element.Value();
        }

        return os;
    }

private:
    T value_;
    bool null = false;
};
