#pragma once

#include "Column.h"
#include "SqlQuery.h"

#include <vector>
#include <memory>

class Table {
public:

    explicit Table(const SqlQuery& sql) {
        if (sql.type_ == SqlQuery::RequestType::Create) {
            return;
        }

        for (size_t i = 0; i < sql.columns.size(); ++i) {
            //// TODO всякие дефолты, примари, но нулы
            std::string col_type = sql.values[i];

            if (col_type == "int") {
                AddColumn<int>(sql.columns[i]);
            } else if (col_type == "varchar") {
                AddColumn<std::string>(sql.columns[i]);
            } else if (col_type == "bool") {
                AddColumn<bool>(sql.columns[i]);
            } else if (col_type == "double") {
                AddColumn<double>(sql.columns[i]);
            } else if (col_type == "float") {
                AddColumn<float>(sql.columns[i]);
            }
        }
    }

    [[nodiscard]] size_t Count() const {
        return columns_[0]->Size();
    }

    friend class MyCoolDB;

protected:

    void AddRow(const SqlQuery& sql) {
        if (sql.type_ != SqlQuery::RequestType::Select) {
            return;
        }

        for (auto& column : columns_) {
            if ((column->GetName()))
        }
    }

    template<typename T>
    void AddData(const std::string& columnName, const T& data) {
        for (const auto& column : columns_) {
            if (column->GetName() == columnName) {
                static_cast<Column<T>*>(column.get())->AddData(data);
                return;
            }
        }
    }

    template<typename T>
    void AddColumn(const std::string& name) {
        columns_.push_back(std::make_unique<Column<T>>(name));
    }

    std::vector<std::unique_ptr<BaseColumn>> columns_;
    size_t count_ = 0;

private:

};
