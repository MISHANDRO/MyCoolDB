cmake_minimum_required(VERSION 3.25)
project(MyCoolDB)

set(CMAKE_CXX_STANDARD 23)

#add_library(MyCoolDB library.cpp)
include_directories(lib)

add_executable(MyCoolDB bin/main.cpp lib/Table.h lib/Column.h lib/ResultSet.h
                                     lib/MyCoolDB.h
                                     lib/ExceptionDB.h lib/SqlQuery.cpp lib/BaseColumn.h lib/Element.h lib/BaseColumn.cpp)