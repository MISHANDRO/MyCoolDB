#pragma once

#include "SqlException.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <regex>

class SqlQuery {
public:
    explicit SqlQuery(const std::string& request);
    struct Condition {

        enum Type {
            AND, OR, WHERE, ON
        };

        enum Operator {
            Equals, NotEquals, More, MoreOrEquals, Lower, LowerOrEquals, IsNull, IsNotNull
        };

        template<typename T>
        bool Compare(const T& lhs, const T& rhs) const {
            switch (operation_) {
                case Equals:
                    return lhs == rhs;
                case NotEquals:
                    return lhs != rhs;
                case More:
                    return lhs > rhs;
                case MoreOrEquals:
                    return lhs >= rhs;
                case Lower:
                    return lhs < rhs;
                case LowerOrEquals:
                    return lhs <= rhs;
                case IsNull:
                    return false;
                case IsNotNull:
                    return true;
            }

            return false;
        }

        void SetCondition(const std::string& condition);
        void SetLhs(const std::string& lhs);
        void SetOperation(const std::string& operation);
        void SetRhs (const std::string& rhs);

        [[nodiscard]] Type GetCondition() const;
        [[nodiscard]] std::string GetLhs() const;
        [[nodiscard]] Operator GetOperation() const;
        [[nodiscard]] std::string GetRhs() const;

    private:
        Type condition_ = ON;
        std::string lhs_;
        Operator operation_ = Equals;
        std::string rhs_;
    };

    enum RequestType {
        Select, Create, Drop, Insert, Update, Delete, Join, None
    };

    [[nodiscard]] RequestType Type() const;
    [[nodiscard]] std::unordered_map<std::string, std::string> GetData() const;
    [[nodiscard]] std::vector<Condition> GetConditions() const;
    [[nodiscard]] std::string GetTableName() const;

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
    bool JoinHandler(const std::string& request);


    RequestType type_ = RequestType::None;
    std::string table;

    std::unordered_map<std::string, std::string> columns_values;
    std::vector<Condition> conditions;
};
