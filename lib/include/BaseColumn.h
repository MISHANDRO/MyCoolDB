#pragma once

#include "SqlQuery.h"

#include <string>

class BaseColumn {
public:
    enum DataType {
        Int, Varchar, Bool, Double, Float
    };

    explicit BaseColumn(std::string name, DataType type)
            : column_name_(std::move(name))
            , type_(type)
    {}

    virtual ~BaseColumn() = default;

    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] virtual std::string GetStrData(size_t index) const { return ""; };
    [[nodiscard]] std::string GetStrStructure() const;

    [[nodiscard]] virtual size_t Size() const;
    [[nodiscard]] DataType Type() const;

    virtual void AddData(const std::string& data) {}
    virtual void AddDefault() {};
    virtual void SetData(const std::string& data, size_t index) {}

    virtual void DeleteData(size_t index) {}
    virtual void CopyDataFrom(BaseColumn* data, size_t index) {}

    virtual bool Compare(const BaseColumn& other, const SqlQuery::Condition& operation,
                         size_t index_1, size_t index_2 = SIZE_MAX) { return false; }
    virtual bool Compare(const std::string& other, const SqlQuery::Condition& operation, size_t index) { return false; }


    virtual void SetForeignKey(const std::string& table, BaseColumn* val) {};
    [[nodiscard]] virtual std::string GetStrForeignKey() const { return ""; };
    void SetPrimaryKeyFlag(bool val);
    void SetAutoIncrementFlag(bool val);
    void SetNotNullFlag(bool val);
    virtual void SetDefault(const std::string& data) {};


protected:
    std::string column_name_;
    DataType type_;

    bool primary_key_ = false;
    bool auto_increment_ = false;
    bool not_null_ = false;

    friend class ResultSet;
};
