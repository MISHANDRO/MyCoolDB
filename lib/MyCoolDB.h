#pragma once

#include "Table.h"
#include "SqlQuery.h"
#include "ResultSet.h"

#include <memory>
#include <filesystem>
#include <fstream>

class MyCoolDB {
public:
    void Load(const std::filesystem::path& path) {
        std::ifstream file(path);

        char ch;
        std::string query;
        while (file.get(ch)) {
            if (ch == ';') {
                Request(query);
                continue;
            }

            query += ch;
        }

        file.close();
    }

    void Save(const std::filesystem::path& path) {

        std::ofstream file(path);

        for (auto& [name, table] : tables_) {
            file << "CREATE TABLE " + name + " (\n";

            for (auto& [_, column] : table->columns_) {
                file <<  "    " + column->GetStrStructure() + ",\n";
            }

            for (auto& [column_name, column] : table->columns_) {
                if (!column->GetStrForeignKey().empty()) {
                    file << "    FOREIGN KEY (" + name + ") REFERENCES " + column->GetStrForeignKey() + ",\n";
                }
            }

            file << ");\n\n";
        }

        for (auto& [name, table] : tables_) {
            std::string cur = "INSERT INTO " + name + "(";

            for (auto& [column_name, column] : table->columns_) {
                cur += column_name + ", ";
            }

            cur += ") VALUES (";

            for (size_t i = 0 ; i < table->Count(); ++i) {
                file << cur;

                for (auto& [_, column] : table->columns_) {
                    file << column->GetStrData(i) + ", ";
                }

                file << ");\n";
            }

            file << '\n';
        }

        file.close();
    }

    //// TODO должен будет что-то вернуть
    void Request(const std::string& request) {
        SqlQuery sql(request);

        switch (sql.Type()) {
            case SqlQuery::RequestType::Create: {
                tables_[sql.GetTableName()] = std::make_unique<Table>(sql, tables_);
                break;
            }

            case SqlQuery::RequestType::Drop:
                tables_[sql.GetTableName()].reset();
                tables_.erase(sql.GetTableName());
                break;

            case SqlQuery::RequestType::Update:
                tables_[sql.GetTableName()]->UpdateRow(sql);
                break;

            case SqlQuery::RequestType::Insert:
                tables_[sql.GetTableName()]->InsertRow(sql);
                break;

            case SqlQuery::RequestType::Delete:
                tables_[sql.GetTableName()]->DeleteRow(sql);
                break;

            default:
                return;
        }
    }

    ResultSet RequestQuery(const std::string& request) {
        SqlQuery sql(request);

        switch (sql.Type()) {
            case SqlQuery::RequestType::Select:
                return ResultSet(*tables_[sql.GetTableName()], sql);
            case SqlQuery::RequestType::Join:
                return JoinTables(sql);
            default:
                return {};
        }
 }

private:

    ResultSet JoinTables(const SqlQuery& sql) {
        if (sql.Type() != SqlQuery::RequestType::Join) {
            return {};
        }

        ResultSet first_table = RequestQuery("SELECT * FROM " + sql.GetTableName());
        ResultSet second_table = RequestQuery("SELECT * FROM " + sql.GetData()["JOIN TABLE"]);

        return ResultSet(first_table, second_table, sql);
    }

    // order is important
    std::map<std::string, std::unique_ptr<Table>> tables_;

    friend class Table;
};
