#pragma once

#include "Column.h"
#include "SqlQuery.h"

#include <vector>
#include <memory>

class Table {
public:
    using table_list = std::map<std::string, std::unique_ptr<Table>>;

    explicit Table(const SqlQuery& sql, const table_list& other_tables);

    [[nodiscard]] size_t Count() const;

    void InsertRow(const SqlQuery& sql);
    void UpdateRow(const SqlQuery& sql);
    void DeleteRow(const SqlQuery& sql);

protected:

    Table() = default;
    Table(Table& table, const SqlQuery& sql);

    static std::vector<std::string> split(const std::string& str, char delimiter = ' ');

    std::unordered_map<std::string, std::unique_ptr<BaseColumn>> columns_;

private:

    bool CheckConditions(const std::vector<SqlQuery::Condition>& conditions, size_t index_row);
    bool CheckOneCondition(const SqlQuery::Condition& condition, size_t index_row);

    void CreateForeignKey(const std::string& column, const std::string& foreign,
                          const table_list& other_tables);

    template<typename T>
    void AddColumn(const std::string& name, const BaseColumn::DataType& type) {
        columns_[name] = std::make_unique<Column<T>>(name, type);
    }

    void AddColumnByType(const std::string& name, const BaseColumn::DataType& type);

    friend class MyCoolDB;
    friend class ResultSet;
};
