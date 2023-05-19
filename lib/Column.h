#pragma once

#include "ExceptionDB.h"

#include <string>
#include <vector>
#include <memory>

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

class BaseColumn {
public:
    enum DataType {
        Int, Varchar, Bool, Double, Float
    };

    explicit BaseColumn(std::string name, DataType type)
        : column_name(std::move(name))
        , type_(type)
    {}

    virtual ~BaseColumn() = default;

    [[nodiscard]] const std::string& GetName() const {
        return column_name;
    };

    [[nodiscard]] virtual size_t Size() const {
        return 0;
    };

    [[nodiscard]] DataType Type() const {
        return type_;
    };

    virtual void AddData(const std::string& data) {}
    virtual void SetData(const std::string& data, size_t index) {}
    virtual void DeleteData(size_t index) {}
    virtual void CopyDataAt(BaseColumn* data, size_t index) {}

    virtual void AddDefault() {};
    virtual void SetForeignKey(BaseColumn* val) {};

    void SetNotNullFlag(bool val) {
        not_null = val;
    }

    void SetPrimaryKeyFlag(bool val) {
        primary_key = val;
    }

    void SetAutoIncrementFlag(bool val) {
        auto_increment = val;
    }

protected:
    std::string column_name;
    DataType type_;

    bool not_null = false;
    bool primary_key = false;
    bool auto_increment = false;
};


template<typename T>
class Column : public BaseColumn {
public:
    explicit Column(const std::string& name, DataType type)
        : BaseColumn(name, type)
    {
        default_ = Element<T>();
    }

    T GetValType(const std::string& data) {
        return T(data);
    }

    void CheckAvailable(const T& val) {
        bool check_foreign = CheckForeignKey(val);
        if (!check_foreign || !CheckPrimaryKey(val)) {
            //// TODO
            throw QueryException((!check_foreign) ? "Unavailable for FOREIGN KEY" :
                                 "Unavailable for PRIMARY KEY");
        }
    }

    void AddData(const std::string& data) override {
        T value = GetValType(data);
        CheckAvailable(value);
        values.emplace_back(value);
    }

    void SetData(const std::string& data, size_t index) override {
        T value = GetValType(data);
        CheckAvailable(value);
        values[index] = Element<T>(value);
    }

    void DeleteData(size_t index) override {
        values.erase(values.begin() + index);
    }

    [[nodiscard]] size_t Size() const override {
        return values.size();
    }

    void CopyDataAt(BaseColumn* data, size_t index) override {
        values.push_back(static_cast<Column<T>*>(data)->values[index]);
    }

    void AddDefault() override {
        if (default_.IsNull() && not_null) {
            //// TODO
            throw QueryException("ed");
        }

        values.push_back(default_);
    }

    [[nodiscard]] Element<T> At(size_t index) const {
        return values[index];
    }

    void SetForeignKey(BaseColumn* val) override {
        foreign_key = static_cast<Column<T>*>(val);
    };

    [[nodiscard]] bool CheckForeignKey(const T& val) const {
        if (foreign_key == nullptr) {
            return true;
        }

        auto same_element = std::find_if(foreign_key->values.begin(), foreign_key->values.end(),
                     [val](const Element<T>& el) {
                         return val == el.Value();
                     }
        );

        return same_element != foreign_key->values.end();
    }

    [[nodiscard]] bool CheckPrimaryKey(const T& val) const {
        if (!primary_key) {
            return true;
        }
        //// TODO СТРОГГИЙ ЧЕЕЕЕЕЕК
        auto same_element = std::find_if(values.begin(), values.end(),
                     [val](const Element<T>& el) {
                         return val == el.Value();
                     }
        );

        return same_element == values.end();
    }

private:
    std::vector<Element<T>> values;

    Column<T>* foreign_key = nullptr;
    Element<T> default_;
};

template<>
int Column<int>::GetValType(const std::string& data) {
    return std::strtol(data.c_str(), nullptr, 10);
}

template<>
bool Column<bool>::GetValType(const std::string& data) {
    if (data == "1" || data == "true") {
        return true;
    }
    if (data == "0" || data == "false") {
        return false;
    }

    //// TODO сообщения
    throw QueryException("");
}

template<>
double Column<double>::GetValType(const std::string& data) {
    return std::strtod(data.c_str(), nullptr);
}

template<>
float Column<float>::GetValType(const std::string& data) {
    return std::strtof(data.c_str(), nullptr);
}
