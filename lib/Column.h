#pragma once

#include "ExceptionDB.h"
#include "BaseColumn.h"
#include "Element.h"

#include <string>
#include <vector>
#include <memory>

template<typename T>
class Column : public BaseColumn {
public:
    explicit Column(const std::string& name, DataType type)
        : BaseColumn(name, type)
    {
        default_ = Element<T>();
    }

    [[nodiscard]] std::string GetStrData(size_t index) const override {
        std::stringstream ss;
        ss << values[index];
        return ss.str();
    }

    T GetValType(const std::string& data) {
        return T(data);
    }

    void AddData(const std::string& data) override {
        if (data == "NULL") {
            values.emplace_back();
        }

        T value = GetValType(data);
        CheckAvailable(value);
        values.emplace_back(value);
    }

    void AddDefault() override {
        values.push_back(default_);
    }

    void SetData(const std::string& data, size_t index) override {
        T value = GetValType(data);
        CheckAvailable(value);
        values[index] = Element<T>(value);
    }


    void DeleteData(size_t index) override {
        values.erase(values.begin() + index);
    }


    void CopyDataAt(BaseColumn* data, size_t index) override {
        values.push_back(static_cast<Column<T>*>(data)->values[index]);
    }


    [[nodiscard]] Element<T> At(size_t index) const {
        return values[index];
    }

    [[nodiscard]] size_t Size() const override {
        return values.size();
    }


    void CheckAvailable(const T& val) {
        bool check_foreign = CheckForeignKey(val);
        if (!check_foreign || !CheckPrimaryKey(val)) {
            //// TODO
            throw QueryException((!check_foreign) ? "Unavailable for FOREIGN KEY" :
                                 "Unavailable for PRIMARY KEY");
        }
    }

    [[nodiscard]] bool CheckForeignKey(const T& val) const {
        if (foreign_key.second == nullptr) {
            return true;
        }

        auto same_element = std::find_if(foreign_key.second->values.begin(), foreign_key.second->values.end(),
                     [val](const Element<T>& el) {
                         return val == el.Value();
                     }
        );

        return same_element != foreign_key.second->values.end();
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

    void SetForeignKey(const std::string& table, BaseColumn* val) override {
        foreign_key = {table, static_cast<Column<T>*>(val)};
    };

    [[nodiscard]] std::string GetStrForeignKey() const override {
        if (foreign_key.second == nullptr) {
            return "";
        }

        return foreign_key.first + '(' + foreign_key.second->column_name + ')';
    }


    bool Compare(const BaseColumn& other, const SqlQuery::Condition& operation,
                 size_t index) override {
        return operation.Compare<T>(values[index].Value(), static_cast<const Column<T>*>(&other)->values[index].Value());
    }

    bool Compare(const std::string& other, const SqlQuery::Condition& operation,
                 size_t index) override {
        Element<T> element = values[index];
        if (operation.GetOperation() == SqlQuery::Condition::IsNull) {
            return element.IsNull();
        }

        if (operation.GetOperation() == SqlQuery::Condition::IsNotNull) {
            return !element.IsNull();
        }

        return operation.Compare<T>(element.Value(), GetValType(other));
    }

private:
    std::vector<Element<T>> values;

    std::pair<std::string, Column<T>*> foreign_key = {"", nullptr};
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
