#pragma once

#include "ExceptionDB.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <regex>

//bool CompareWithoutRegister(const std::string& str1, const std::string& str2) {
//    return std::equal(str1.begin(), str1.end(), str2.begin(), [](char c1, char c2) {
//        return std::toupper(static_cast<unsigned char>(c1)) == std::toupper(static_cast<unsigned char>(c2));
//    });
//}


class SqlQuery {
public:
    explicit SqlQuery(const std::string& request) {
        if (SelectHandler(request)) {
            type_ = Select;
        } else if (InsertHandler(request)) {
            type_ = Insert;
        } else if (DeleteHandler(request)) {
            type_ = Delete;
        } else if (UpdateHandler(request)) {
            type_ = Update;
        } else if (CreateHandler(request)) {
            type_ = Create;
        } else if (DropHandler(request)) {
            type_ = Drop;
        }
//        else if (JoinHandler(request)) {
//            type_ = Join;
//        }

        //// TODO exc
        if (type_ == RequestType::None) {
            throw QueryException("Bad query");
        }
    }

    struct Condition {
        std::string condition;
        std::string lhs;
        std::string operation;
        std::string rhs;
    };

    enum RequestType {
        Select, Create, Drop, Insert, Update, Delete, Join, None
    };

    [[nodiscard]] RequestType Type() const {
        return type_;
    }

    [[nodiscard]] std::unordered_map<std::string, std::string> GetData() const {
        return columns_values;
    }

    [[nodiscard]] std::vector<Condition> GetConditions() const {
        return conditions;
    }

    [[nodiscard]] std::string GetTableName() const {
        return table;
    }

    friend class Table;

private:
    void ConditionHandler(const std::string& condition_str);
    static void ColumnsValuesHandler(const std::string& str, std::vector<std::string>& mas);

    bool SelectHandler(const std::string& request);
    bool DeleteHandler(const std::string& request);
    bool InsertHandler(const std::string& request);
    bool UpdateHandler(const std::string& request);
    bool CreateHandler(const std::string& request);
    bool DropHandler(const std::string& request);


    RequestType type_ = RequestType::None;
    std::string table;

    std::unordered_map<std::string, std::string> columns_values;
//    std::vector<std::string> values;

    std::vector<Condition> conditions;
};
