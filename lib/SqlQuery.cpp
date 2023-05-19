#include "SqlQuery.h"

void SqlQuery::ConditionHandler(const std::string& condition_str)  {
    if (condition_str.empty()){
        return;
    }

    std::regex conditionRegex(R"((WHERE|AND|OR)\s+([A-Za-z0-9-_]+)\s*([(?:IS)<>!=]+)\s*((?:NOT NULL)|[^\s;]+))");
    std::smatch matches;

    std::string::const_iterator start_search(condition_str.cbegin());
    while (std::regex_search(start_search, condition_str.cend(), matches, conditionRegex)) {
        if (matches.size() >= 5) {
            Condition condition;
            condition.condition = matches[1].str();
            condition.lhs = matches[2].str();
            condition.operation = matches[3].str();
            condition.rhs = matches[4].str();

            conditions.push_back(condition);
        } else {
            //// TODO
            throw QueryException("Bad condition");
        }

        start_search = matches.suffix().first;
    }

    if (conditions.empty()) {
        //// TODO
        throw QueryException("Bad condition");
    }
}

void SqlQuery::ColumnsValuesHandler(const std::string& str, std::vector<std::string>& mas) {
    std::regex pattern(R"([^,\s*]+)");
    std::sregex_iterator pattern_iter(str.begin(), str.end(), pattern);
    std::sregex_iterator pattern_end;

    for (; pattern_iter != pattern_end; ++pattern_iter) {
        mas.push_back(pattern_iter->str());
    }
}

bool SqlQuery::SelectHandler(const std::string& request) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
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
    std::regex regex(R"(INSERT\s+INTO\s+([^\s]+)\s*\(([A-Za-z0-9-_,\s]+)\)\s*VALUES\s*\(([A-Za-z0-9-_,\s]+)\)[\s;]*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 4) {
        table = matches[1].str();

        std::vector<std::string> columns;
        std::vector<std::string> values;

        ColumnsValuesHandler(matches[2].str(), columns);
        ColumnsValuesHandler(matches[3].str(), values);

        if (columns.size() != values.size()) {
            //// TODO
            throw QueryException("Bad condition");
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            columns_values[columns[i]] = values[i];
        }

        return true;
    }

    return false;
}

bool SqlQuery::UpdateHandler(const std::string &request)  {
    std::regex regex(R"(UPDATE\s+([^\s]+)\s+SET\s+([A-Za-z0-9-_=,\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 3) {
        table = matches[1].str();

        std::string columns_str = matches[2].str();
        std::regex columnsRegex(R"([^\s=,]+)");
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
        std::regex table_struct_regex(R"(([^\s,\(]+)\s+([^\s,]+(?:\s+(?:PRIMARY KEY|NOT NULL))*))");

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
