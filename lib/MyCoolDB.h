#pragma once

#include "Table.h"
#include "SqlQuery.h"
#include "ResultSet.h"

#include <memory>
#include <filesystem>

class MyCoolDB {
public:
//    void Load(const std::filesystem::path& path);
//    void Save(const std::filesystem::path& path);

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
                tables_[sql.GetTableName()]->AddRow(sql);
                break;

            case SqlQuery::RequestType::Delete:
                tables_[sql.GetTableName()]->DeleteRow(sql);
                break;

//            case SqlQuery::RequestType::None:
//            default:
        }
    }

    ResultSet RequestQuery(const std::string& request) {
        SqlQuery sql(request);

        switch (sql.Type()) {
            case SqlQuery::RequestType::Select:
                return ResultSet(*tables_[sql.GetTableName()], sql);
            case SqlQuery::RequestType::Join:
                //// TODO
                break;
//            default:
        }

        return {};
    }

private:
//    std::vector<std::unique_ptr<Table>> tables_;
    std::unordered_map<std::string, std::unique_ptr<Table>> tables_;

    friend class Table;
};
