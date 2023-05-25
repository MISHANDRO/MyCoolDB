include_directories(include)

add_library(MyCoolDB
        include/Table.h            src/Table.cpp
        include/Column.h           src/Column.cpp
        include/MyCoolDB.h         src/MyCoolDB.cpp
        include/ResultSet.h        src/ResultSet.cpp
        include/SqlQuery.h         src/SqlQuery.cpp
        include/BaseColumn.h       src/BaseColumn.cpp
        include/SqlException.h
        include/Element.h
)
