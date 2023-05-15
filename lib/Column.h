#pragma once

#include <string>
#include <vector>
#include <memory>

class BaseColumn {
public:
    explicit BaseColumn(std::string name)
        : column_name(std::move(name))
    {}

    virtual ~BaseColumn() = default;

    [[nodiscard]] const std::string& GetName() const {
        return column_name;
    };

    [[nodiscard]] virtual size_t Size() const {
        return 0;
    };

private:
    std::string column_name;
};


template<typename T>
class Column : public BaseColumn {
public:
    explicit Column(const std::string& name)
        : BaseColumn(name)
    {}

    void AddData(const T& data) {
        values.push_back(std::make_unique<T>(data));
    }

    [[nodiscard]] size_t Size() const override {
        return values.size();
    }

    [[nodiscard]] T operator[](size_t n) const {
        return *values[n];
    }

private:
    std::vector<std::unique_ptr<T>> values;

    std::unique_ptr<T> default_ = nullptr;
    bool not_null = false;
    bool primary_key = false;
    bool auto_increment = false;
};
