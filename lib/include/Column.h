#pragma once

#include "SqlException.h"
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

    void SetDefault(const std::string& data) {
        T value = GetValType(data);
        default_ = Element<T>(value);
    }

    [[nodiscard]] std::string GetStrData(size_t index) const override {
        std::stringstream ss;
        ss << values[index];
        return ss.str();
    }

    T GetValType(const std::string& data) const;

    void AddData(const std::string& data) override {
        if (data == "NULL") {
            if (not_null_) {
                throw SqlException("The value of  cannot be NULL");
            }

            values.emplace_back();
            return;
        }

        T value = GetValType(data);
        CheckAvailable(value);
        values.emplace_back(value);
    }

    void AddDefault() override {
        if (auto_increment_) {
            AddData(std::to_string(values.size() + 1));
            return;
        }

        if (not_null_ && default_.IsNull()) {
            throw SqlException("The value of " + column_name_ +"cannot be NULL");
        }

        values.push_back(default_);
    }

    void SetData(const std::string& data, size_t index) override {
        if (data == "NULL") {
            values[index] = Element<T>();
            return;
        }


        T value = GetValType(data);
        CheckAvailable(value);
        values[index] = Element<T>(value);
    }


    void DeleteData(size_t index) override {
        values.erase(values.begin() + index);
    }


    void CopyDataFrom(BaseColumn* data, size_t index) override {
        values.push_back(static_cast<Column<T>*>(data)->values[index]);
    }


    [[nodiscard]] Element<T> At(size_t index) const {
        return values[index];
    }

    [[nodiscard]] size_t Size() const override {
        return values.size();
    }


    void CheckAvailable(const T& val) {
        bool check_foreign = CheckForeignKey(Element<T>(val));
        if (!check_foreign || !CheckPrimaryKey(val)) {
            throw SqlException((!check_foreign) ? "Unavailable for FOREIGN KEY" :
                               "Unavailable for PRIMARY KEY");
        }
    }

    [[nodiscard]] bool CheckForeignKey(const Element<T>& val) const {
        if (foreign_key.second == nullptr) {
            return true;
        }

        auto same_element = std::find_if(foreign_key.second->values.begin(), foreign_key.second->values.end(),
                     [val](const Element<T>& el) {
                         return val == el;
                     }
        );

        return same_element != foreign_key.second->values.end();
    }


    [[nodiscard]] bool CheckPrimaryKey(const T& val) const {
        if (!primary_key_) {
            return true;
        }

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

        return foreign_key.first + '(' + foreign_key.second->column_name_ + ')';
    }


    bool Compare(const BaseColumn& other, const SqlQuery::Condition& operation,
                 size_t index1, size_t index2) override {
        if (index2 == SIZE_MAX) {
            index2 = index1;
        }

        return operation.Compare<T>(values[index1].Value(), static_cast<const Column<T>*>(&other)->values[index2].Value());
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

        if (element.IsNull()) {
            return false;
        }

        return operation.Compare<T>(element.Value(), GetValType(other));
    }

private:
    std::vector<Element<T>> values;

    std::pair<std::string, Column<T>*> foreign_key = {"", nullptr};
    Element<T> default_;
};
