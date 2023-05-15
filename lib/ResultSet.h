#pragma once

#include "Table.h"

class ResultSet : public Table {
public:
    explicit ResultSet(const std::vector<std::pair<std::string, std::string>>& columns)
        : Table(columns)
    {}

    bool Next() {
        return ++cur_ < Count();
    }

    template<typename T>
    T Get(const std::string& columnName) {
        if (cur_ == SIZE_MAX) {
            ++cur_;
        }

        if (cur_ >= Count()) {
            return T();
        }

        for (const auto& column : columns_) {
            if (column->GetName() == columnName) {
                return static_cast<Column<T>*>(column.get())[cur_];
            }
        }
    }

private:
    size_t cur_ = SIZE_MAX;
};
