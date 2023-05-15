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

            conditionals.push_back(condition);
        } else {
            //// TODO
            throw QueryException("Bad condition");
        }

        start_search = matches.suffix().first;
    }

    if (conditionals.empty()) {
        //// TODO
        throw QueryException("Bad condition");
    }
}

void SqlQuery::ColumnsValuesHandler(const std::string &str, bool columns_flag) {
    std::regex pattern(R"([^,\s*]+)");
    std::sregex_iterator pattern_iter(str.begin(), str.end(), pattern);
    std::sregex_iterator pattern_end;

    for (; pattern_iter != pattern_end; ++pattern_iter) {
        std::string element_str = pattern_iter->str();
        if (columns_flag) {
            columns.push_back(element_str);
        } else {
            values.push_back(element_str);
        }
    }
}

bool SqlQuery::SelectHandler(const std::string& request) {
    std::regex regex(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
    std::smatch matches;

    if (std::regex_match(request, matches, regex) && matches.size() >= 3) {
        std::string columns_str = matches[1].str();
        if (columns_str == "*") {
            columns.emplace_back("*");
        } else {
            ColumnsValuesHandler(columns_str);
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

        ColumnsValuesHandler(matches[2].str());
        ColumnsValuesHandler(matches[3].str(), false);

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

        uint16_t iter = 0;
        for (; columnsIter != columnsEnd; ++columnsIter) {
            std::string element_str = columnsIter->str();
            if (iter % 2 == 0) {
                columns.push_back(element_str);
            } else {
                values.push_back(element_str);
            }

            ++iter;
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

        std::string condition_str = matches[2].str();
        std::regex conditionRegex(R"(([^\s,]+)\s+([^\s,]+)(?:\s+(PRIMARY KEY|FOREIGN KEY|NOT\ NULL))?,)");

        std::string::const_iterator start_search(request.cbegin());
        while (std::regex_search(start_search, request.cend(), matches, conditionRegex)) {
            if (matches.size() >= 2) {
                columns.push_back(matches[1].str());
                values.push_back(matches[2].str());
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
