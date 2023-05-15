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
            case SqlQuery::RequestType::Create:
                tables_["table_name"] = std::make_unique<Table>(sql);
                break;

            case SqlQuery::RequestType::Drop:
                tables_["table_name"].reset();
                tables_.erase("table_name");
                break;

            case SqlQuery::RequestType::Select:
                //// TODO возвращаем ResultSet
                break;

            case SqlQuery::RequestType::Update:
                //// TODO
                break;

            case SqlQuery::RequestType::Insert:
                //// TODO
                break;

            case SqlQuery::RequestType::Delete:
                //// TODO
                break;

            case SqlQuery::RequestType::Join:
                //// TODO
                break;

            case SqlQuery::RequestType::None:
        }
    }

private:
//    std::vector<std::unique_ptr<Table>> tables_;
    std::unordered_map<std::string, std::unique_ptr<Table>> tables_;

    friend class Table;
};
