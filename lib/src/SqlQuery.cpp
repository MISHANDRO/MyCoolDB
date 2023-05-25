#include "SqlQuery.h"


SqlQuery::SqlQuery(const std::string& request) {
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
    } else if (JoinHandler(request)) {
        type_ = Join;
    }

    if (type_ == RequestType::None) {
        throw SqlException("Bad query");
    }
}

SqlQuery::RequestType SqlQuery::Type() const {
    return type_;
}

std::unordered_map<std::string, std::string> SqlQuery::GetData() const {
    return columns_values;
}

std::vector<SqlQuery::Condition> SqlQuery::GetConditions() const {
    return conditions;
}

std::string SqlQuery::GetTableName() const {
    return table;
}


void SqlQuery::ConditionHandler(const std::string& condition_str)  {
    if (condition_str.empty()){
        return;
    }

    std::regex conditionRegex(R"((WHERE|AND|OR|ON)\s+([^\s=]+)\s*([(?:IS)<>!=]+)\s*((?:NOT NULL|NULL)|(?:'[^']*')|(?:[^,\s]+)+))");
    std::smatch matches;

    std::string::const_iterator start_search(condition_str.cbegin());
    while (std::regex_search(start_search, condition_str.cend(), matches, conditionRegex)) {
        if (matches.size() >= 5) {
            Condition condition;
            condition.SetCondition(matches[1].str());
            condition.SetLhs(matches[2].str());
            condition.SetOperation(matches[3].str());
            condition.SetRhs(matches[4].str());

            conditions.push_back(condition);
        } else {
            throw SqlException("Bad condition");
        }

        start_search = matches.suffix().first;
    }

    if (conditions.empty()) {
        throw SqlException("Empty condition");
    }
}

void SqlQuery::ColumnsValuesHandler(const std::string& str, std::vector<std::string>& mas) {
    std::regex pattern(R"(('[^']*')|([^,\s]+))");
    std::sregex_iterator pattern_iter(str.begin(), str.end(), pattern);
    std::sregex_iterator pattern_end;

    for (; pattern_iter != pattern_end; ++pattern_iter) {
        mas.push_back(pattern_iter->str());
    }
}

bool SqlQuery::SelectHandler(const std::string& request) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 3) {
        std::string columns_str = matches[1].str();
        if (columns_str == "*") {
            columns_values["*"] = "";
        } else {
            std::vector<std::string> columns;
            ColumnsValuesHandler(columns_str, columns);

            for (auto& column : columns) {
                columns_values[column] = "";
            }
        }

        table = matches[2].str();

        if (matches.size() >= 4) {
            ConditionHandler(matches[3].str());
        }

        return true;
    }

    return false;
}

bool SqlQuery::DeleteHandler(const std::string& request)  {
    std::regex regex(R"(DELETE\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 2) {
        table = matches[1].str();

        if (matches.size() >= 3) {
            ConditionHandler(matches[2].str());
        }

        return true;
    }

    return false;
}

bool SqlQuery::InsertHandler(const std::string &request) {
    std::regex regex(R"(INSERT\s+INTO\s+([^\s]+)\s*\(([A-Za-z0-9-_,\s]+)\)\s*VALUES\s*\((['"A-Za-z0-9-_,\s\.]+)\)[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 4) {
        table = matches[1].str();

        std::vector<std::string> columns;
        std::vector<std::string> values;

        ColumnsValuesHandler(matches[2].str(), columns);
        ColumnsValuesHandler(matches[3].str(), values);

        if (columns.size() != values.size()) {
            throw SqlException("Bad insert construction");
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            columns_values[columns[i]] = values[i];
        }

        return true;
    }

    return false;
}

bool SqlQuery::UpdateHandler(const std::string &request)  {
    std::regex regex(R"(UPDATE\s+([^\s]+)\s+SET\s+([^(?:WHERE)]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 3) {
        table = matches[1].str();

        std::string columns_str = matches[2].str();
        std::regex columnsRegex(R"(('[^']*')|[^\s=,]+)");
        std::sregex_iterator columnsIter(columns_str.begin(), columns_str.end(), columnsRegex);
        std::sregex_iterator columnsEnd;

        std::string column;
        for (; columnsIter != columnsEnd; ++columnsIter) {
            std::string element_str = columnsIter->str();
            if (column.empty()) {
                column = element_str;
            } else {
                columns_values[column] = element_str;
                column = "";
            }
        }

        if (matches.size() >= 4) {
            ConditionHandler(matches[3].str());
        }

        return true;
    }

    return false;
}

bool SqlQuery::CreateHandler(const std::string &request) {
    std::regex regex(R"(CREATE\s+TABLE\s+([^\s]+)\s+\(([\sA-Za-z0-9-_,\(\)]*)\)[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 2) {
        table = matches[1].str();

        std::string table_struct = matches[2].str();
        std::regex table_struct_regex(R"(([^\s,\(]+)\s+([^\s,]+(?:\s+(?:PRIMARY KEY))*))");

        std::string::const_iterator start_search(table_struct.cbegin());
        while (std::regex_search(start_search, table_struct.cend(), matches, table_struct_regex)) {
            if (matches.size() >= 3 && matches[2].str() != "KEY" && matches[2].str() != "REFERENCES") {
                columns_values[matches[1].str()] = matches[2].str();
            }

            start_search = matches.suffix().first;
        }

        if (columns_values.empty()) {
            return false;
        }

        std::regex foreign_key_regex(R"(FOREIGN\s+KEY\s+\(([^\s]+)\)\s+REFERENCES\s+([^\s\)]+)\(([^\s,]+)\))");

        start_search = table_struct.cbegin();
        while (std::regex_search(start_search, table_struct.cend(), matches, foreign_key_regex)) {
            if (matches.size() >= 4) {
                columns_values["F " + matches[1].str()] = matches[2].str() + " " + matches[3].str();
                if (!columns_values.contains(matches[1].str())) {
                    return false;
                }
            }

            start_search = matches.suffix().first;
        }

        return true;
    }

    return false;
}

bool SqlQuery::DropHandler(const std::string &request) {
    std::regex regex(R"(DROP\s+TABLE\s+([^\s;]+)[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 2) {
        table = matches[1].str();
        return true;
    }

    return false;
}

bool SqlQuery::JoinHandler(const std::string& request) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s+(LEFT|RIGHT|INNER)?\s+JOIN\s+([^\s]+)\s+(ON\s+[^(?:WHERE)]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 6) {
        std::string columns_str = matches[1].str();
        if (columns_str == "*") {
            columns_values["*"] = "";
        } else {
            std::vector<std::string> columns;
            ColumnsValuesHandler(columns_str, columns);

            for (auto& column : columns) {
                columns_values[column] = "";
            }
        }

        table = matches[2].str();

        columns_values["JOIN TYPE"]  = matches[3].str();
        columns_values["JOIN TABLE"] = matches[4].str();

        ConditionHandler(matches[5].str());


        if (matches.size() >= 7) {
            ConditionHandler(matches[6].str());
        }

        return true;
    }

    return false;
}

void SqlQuery::Condition::SetCondition(const std::string& condition) {
    if (condition == "AND") {
        condition_ = AND;
    } else if (condition == "OR") {
        condition_ = OR;
    } else if (condition == "WHERE") {
        condition_ = WHERE;
    }  else if (condition == "ON") {
        condition_ = ON;
    } else {
        throw SqlException("Bad condition operand");
    }
}

void SqlQuery::Condition::SetLhs(const std::string& lhs) {
    lhs_ = lhs;
}

void SqlQuery::Condition::SetOperation(const std::string& operation) {
    if (operation == "=" || operation == "IS") {
        operation_ = Equals;
    } else if (operation == "!=") {
        operation_ = NotEquals;
    } else if (operation == ">") {
        operation_ = More;
    } else if (operation == ">=") {
        operation_ = MoreOrEquals;
    } else if (operation == "<") {
        operation_ = Lower;
    } else if (operation == "<=") {
        operation_ = LowerOrEquals;
    }
};

void SqlQuery::Condition::SetRhs(const std::string& rhs) {
    if (rhs == "NULL" || rhs == "NOT NULL") {
        if (operation_ != Equals && operation_ != IsNull && operation_ != IsNotNull) {
            throw SqlException("Bad condition");
        }

        operation_ = (rhs == "NULL") ? IsNull : IsNotNull;
    }

    rhs_ = rhs;
}

SqlQuery::Condition::Type SqlQuery::Condition::GetCondition() const {
    return condition_;
}

std::string SqlQuery::Condition::GetLhs() const {
    return lhs_;
}

SqlQuery::Condition::Operator SqlQuery::Condition::GetOperation() const {
    return operation_;
}

std::string SqlQuery::Condition::GetRhs() const {
    return rhs_;
}
