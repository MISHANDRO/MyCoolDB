#include "ResultSet.h"


bool ResultSet::Next() {
    return ++cur_ < Count();
}

void ResultSet::Reset() {
    cur_ = 0;
}

std::ostream& operator<<(std::ostream& os, const ResultSet& result_set) {

    for (auto& [name, _] : result_set.columns_) {
        os << name << ' ';
    }
    os << '\n';

    for (size_t i = 0; i < result_set.Count(); ++i) {
        for (auto& [_, column] : result_set.columns_) {
            os << column->GetStrData(i) << ' ';
        }
        os << '\n';
    }

    return os;
}

ResultSet ResultSet::JoinTables(Table& table1, Table& table2, SqlQuery& sql) {
    auto rows = sql.GetData();
    std::string table1_name = sql.GetTableName() + '.';
    std::string table2_name = rows["JOIN TABLE"] + '.';

    Table joined_table;

    for (auto& [name, column] : table1.columns_) {
        std::string table_column = table1_name + name;
        if (rows.contains(table_column) || rows.contains("*")) {
            joined_table.AddColumnByType(table_column, column->Type());
        }
    }

    for (auto& [name, column] : table2.columns_) {
        std::string table_column = table2_name + name;
        if (rows.contains(table_column) || rows.contains("*")) {
            joined_table.AddColumnByType(table_column, column->Type());
        }
    }

    Table* left_table = &table1;
    Table* right_table = &table2;
    if (rows["JOIN TYPE"] == "RIGHT") {
        std::swap(left_table, right_table);
        std::swap(table1_name, table2_name);
    }

    const bool kInnerMode = rows["JOIN TYPE"] == "INNER";

    std::vector<std::pair<BaseColumn*, BaseColumn*>> ON_conditions;
    ON_conditions.reserve(sql.GetConditions().size());
    for (auto& condition : sql.GetConditions()) {
        if (condition.GetCondition() == SqlQuery::Condition::Type::WHERE) {
            break;
        }

        std::string left_column = split(condition.GetLhs(), '.')[1];
        std::string right_column = split(condition.GetRhs(), '.')[1];

        if (left_table->columns_.contains(left_column) && right_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[left_column].get(),
                                       right_table->columns_[right_column].get());
        } else if (right_table->columns_.contains(left_column) && left_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[right_column].get(),
                                       right_table->columns_[left_column].get());
        } else {
            throw SqlException("Bad JOIN condition");
        }
    }


    SqlQuery::Condition equals_obj{};
    equals_obj.SetOperation("=");
    for (size_t i = 0; i < left_table->Count(); ++i) {
        bool joined_flag = false;
        for (auto& [column1, column2] : ON_conditions) {

            for (size_t j = 0; j < right_table->Count(); ++j) {
                bool equals = true;
                for (auto& [lhs, rhs] : ON_conditions) {
                    if (!lhs->Compare(*rhs, equals_obj, i, j)) {
                        equals = false;
                        break;
                    }
                }

                if (equals) {
                    joined_flag = true;

                    for (auto& [name, column]: left_table->columns_) {
                        if (joined_table.columns_.contains(table1_name + name)) {
                            joined_table.columns_[table1_name + name]->CopyDataFrom(column.get(), i);
                        }
                    }

                    for (auto& [name, column]: right_table->columns_) {
                        if (joined_table.columns_.contains(table2_name + name)) {
                            joined_table.columns_[table2_name + name]->CopyDataFrom(column.get(), j);
                        }
                    }
                }
            }
        }

        if (!joined_flag && !kInnerMode) {
            for (auto& [name, column]: left_table->columns_) {
                if (joined_table.columns_.contains(table1_name + name)) {
                    joined_table.columns_[table1_name + name]->CopyDataFrom(column.get(), i);
                }
            }

            for (auto& [name, _]: right_table->columns_) {
                joined_table.columns_[table2_name + name]->AddData("NULL");
            }
        }
    }


    return ResultSet(joined_table, sql);
}
