cmake_minimum_required(VERSION 3.25)
project(MyCoolDB)

set(CMAKE_CXX_STANDARD 23)

#add_library(MyCoolDB library.cpp)
include_directories(lib)

add_executable(MyCoolDB
        bin/main.cpp
        lib/Table.h          lib/Table.cpp
        lib/Column.h         lib/Column.cpp
        lib/MyCoolDB.h       lib/MyCoolDB.cpp
        lib/ResultSet.h      lib/ResultSet.cpp
        lib/SqlQuery.h       lib/SqlQuery.cpp
        lib/BaseColumn.h     lib/BaseColumn.cpp
        lib/SqlException.h
        lib/Element.h
        )