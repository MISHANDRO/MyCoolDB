#include "BaseColumn.h"

const std::string& BaseColumn::GetName() const {
    return column_name_;
}

size_t BaseColumn::Size() const {
    return 0;
}

BaseColumn::DataType BaseColumn::Type() const {
    return type_;
}

void BaseColumn::SetPrimaryKeyFlag(bool val) {
    primary_key_ = val;
}

void BaseColumn::SetAutoIncrementFlag(bool val) {
    auto_increment_ = val;
}

void BaseColumn::SetNotNullFlag(bool val) {
    not_null_ = val;
}

std::string BaseColumn::GetStrStructure() const {
    std::string res = column_name_ + " ";
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

    if (primary_key_) {
        res += " PRIMARY KEY";
    }

    return res;
}
