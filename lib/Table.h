#pragma once

#include "Column.h"
#include "SqlQuery.h"

#include <vector>
#include <memory>

std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string cur;

    while (std::getline(ss, cur, delimiter)) {
        words.push_back(cur);
    }

    return words;
}

class Table {
public:
    using table_list = std::map<std::string, std::unique_ptr<Table>>;

    explicit Table(const SqlQuery& sql,
                   const table_list& other_tables) {
        if (sql.type_ != SqlQuery::RequestType::Create) {
            return;
        }

        for (auto& [column, type] : sql.GetData()) {
            //// TODO всякие дефолты, примари, но нулы

            auto parameters = split(type);
            std::string col_type = parameters[0];

            if (col_type == "INT") {
                AddColumnByType(column, BaseColumn::DataType::Int);
            } else if (col_type == "VARCHAR") {
                AddColumnByType(column, BaseColumn::DataType::Varchar);
            } else if (col_type == "BOOL") {
                AddColumnByType(column, BaseColumn::DataType::Bool);
            } else if (col_type == "DOUBLE") {
                AddColumnByType(column, BaseColumn::DataType::Double);
            } else if (col_type == "FLOAT") {
                AddColumnByType(column, BaseColumn::DataType::Float);
            }

            if (std::find(parameters.begin(), parameters.end(), "PRIMARY") != parameters.end()) {
                columns_[column]->SetPrimaryKeyFlag(true);
            }
        }

        for (auto& [column, type] : sql.GetData()) {
            if (column[0] == 'F' && column[1] == ' ') {
                CreateForeignKey(column.substr(2), type, other_tables);
                continue;
            }
        }
    }

    [[nodiscard]] size_t Count() const {
        return columns_.begin()->second->Size();
    }


    void InsertRow(const SqlQuery& sql) {
        if (sql.type_ != SqlQuery::RequestType::Insert) {
            return;
        }

        auto row = sql.GetData();
        for (const auto& [_, column] : columns_) {
            if (row.contains(column->GetName())) {
                column->AddData(row[column->GetName()]);
            } else {
                column->AddDefault();
            }

            //// TODO нужен ли try, чтоб удалить уже добавленное при ошибке?
            //// TODO Или сделать сначала проверку на primary и foreign
        }
    }

    void UpdateRow(const SqlQuery& sql) {
        if (sql.type_ != SqlQuery::RequestType::Update) {
            return;
        }

        auto conditions = sql.GetConditions();
        for (size_t i = 0; i < Count(); ++i) {
            if (CheckConditions(conditions, i)) {

                for (auto& [column, value]: sql.columns_values) {
                    if (!columns_.contains(column)) {
                        //// TODO
                        throw QueryException("cdddc");
                    }

                    columns_[column]->SetData(value, i);
                }

            }
        }
    }

    void DeleteRow(const SqlQuery& sql) {
        if (sql.type_ != SqlQuery::RequestType::Delete) {
            return;
        }

        auto conditions = sql.GetConditions();
        for (long long i = Count() - 1; i >= 0; --i) {
            if (CheckConditions(conditions, i)) {

                for (auto& [name, column]: columns_) {
                    column->DeleteData(i);
                }

            }
        }
    }

protected:

    Table() = default;

    Table(Table& table, const SqlQuery& sql) {
        if (sql.Type() != SqlQuery::RequestType::Select) {
            return;
        }

        auto row = sql.GetData();
        for (const auto& [_, column] : table.columns_) {
            if (row.contains(column->GetName()) || row.contains("*")) {
                AddColumnByType(column->GetName(), column->Type());
            }
        }

        auto conditions = sql.GetConditions();
        for (size_t i = 0; i < table.Count(); ++i) {
            if (table.CheckConditions(conditions, i)) {

                for (auto& [name, column]: columns_) {

                    column->CopyDataAt(table.columns_[name].get(), i);
                }

            }
        }
    }

    std::unordered_map<std::string, std::unique_ptr<BaseColumn>> columns_;

private:

    bool CheckConditions(const std::vector<SqlQuery::Condition>& conditions,
                         size_t index) {
        if (conditions.empty()) {
            return true;
        }

        bool res = CheckOneCondition(conditions[0], index);
        for (size_t i = 1; i < conditions.size(); ++i) {
            bool cur = CheckOneCondition(conditions[i], index);
            switch (conditions[i].GetCondition()) {
                case SqlQuery::Condition::AND:
                    res = res && cur;
                    break;
                case SqlQuery::Condition::OR:
                    res = res || cur;
                    break;
                case SqlQuery::Condition::WHERE:
                case SqlQuery::Condition::ON:
                    //// TODO сообщение
                    throw QueryException("fwefwfe");
            }
        }

        return res;
    }

    bool CheckOneCondition(const SqlQuery::Condition& condition, size_t index) {
        if (columns_.contains(condition.GetLhs()) && columns_.contains(condition.GetRhs())) {
            return columns_[condition.GetRhs()]->Compare(*columns_[condition.GetLhs()], condition, index);
        }

        if (columns_.contains(condition.GetLhs())) {
            return columns_[condition.GetLhs()]->Compare(condition.GetRhs(), condition, index);
        }

        if (columns_.contains(condition.GetRhs())) {
            return columns_[condition.GetRhs()]->Compare(condition.GetLhs(), condition, index);
        }

        return true;
    }

    void CreateForeignKey(const std::string& column, const std::string& foreign,
                          const table_list& other_tables) {
        auto parameters = split(foreign);
        columns_[column].get()->SetForeignKey(parameters[0],
                                              other_tables.at(parameters[0])->columns_.at(parameters[1]).get());
    }

    template<typename T>
    void AddColumn(const std::string& name, const BaseColumn::DataType& type) {
        columns_[name] = std::make_unique<Column<T>>(name, type);
    }

    void AddColumnByType(const std::string& name, const BaseColumn::DataType& type) {
        switch (type) {
            case BaseColumn::DataType::Int:
                AddColumn<int>(name, type);
                break;
            case BaseColumn::DataType::Varchar:
                AddColumn<std::string>(name, type);
                break;
            case BaseColumn::DataType::Bool:
                AddColumn<bool>(name, type);
                break;
            case BaseColumn::DataType::Double:
                AddColumn<double>(name, type);
                break;
            case BaseColumn::DataType::Float:
                AddColumn<float>(name, type);
                break;
        }
    }

    friend class MyCoolDB;
    friend class ResultSet;
};
