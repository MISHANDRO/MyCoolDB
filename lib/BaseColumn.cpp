#include "BaseColumn.h"

const std::string& BaseColumn::GetName() const {
    return column_name;
}

size_t BaseColumn::Size() const {
    return 0;
}

BaseColumn::DataType BaseColumn::Type() const {
    return type_;
}

void BaseColumn::SetPrimaryKeyFlag(bool val) {
    primary_key = val;
}

std::string BaseColumn::GetStrStructure() const {
    std::string res = column_name + " ";
    switch (type_) {
        case Int:
            res += "INT";
            break;
        case Bool:
            res += "BOOL";
            break;
        case Double:
            res += "DOUBLE";
            break;
        case Float:
            res += "FLOAT";
            break;
        case Varchar:
            res += "VARCHAR";
            break;
    }

    if (primary_key) {
        res += " PRIMARY KEY";
    }

    return res;
}
