#pragma once

#include "Table.h"
#include "SqlQuery.h"
#include "ResultSet.h"

#include <memory>
#include <filesystem>
#include <fstream>

class MyCoolDB {
public:
    void Load(const std::filesystem::path& path);
    void Save(const std::filesystem::path& path);

    void Request(const std::string& request);
    ResultSet RequestQuery(const std::string& request);

private:

    ResultSet JoinTables(SqlQuery& sql);

    // order is important
    std::map<std::string, std::unique_ptr<Table>> tables_;

    friend class Table;
};
