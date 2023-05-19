#pragma once

#include "Table.h"

class ResultSet : public Table {
public:

    ResultSet() = default;

    explicit ResultSet(Table& table, const SqlQuery& sql)
        : Table(table, sql)
    {}

    bool Next() {
        return ++cur_ < Count();
    }

    template<typename T>
    Element<T> Get(const std::string& columnName) {
        if (cur_ == SIZE_MAX) {
            ++cur_;
        }

        if (cur_ >= Count()) {
            return Element<T>();
        }

        for (const auto& [name, column] : columns_) {
            if (name == columnName) {
                return static_cast<Column<T>*>(column.get())->At(cur_);
            }
        }

        return Element<T>();
    }

private:
    size_t cur_ = SIZE_MAX;
};
