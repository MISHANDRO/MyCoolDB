#include <iostream>
#include <regex>
#include <string>
#include <vector>

//#include <Column.h>
//#include <SqlQuery.h>
//#include <Table.h>
//#include "ResultSet.h"
#include "MyCoolDB.h"


int main() {

    MyCoolDB db;

    db.Request("CREATE TABLE Customers (id INT PRIMARY KEY,bre INT,\n"
               "    name VARCHAR,\n"
               ");");


    db.Request("CREATE TABLE Orders (\n"
               "    id INT PRIMARY KEY,\n"
               "    customer_id INT,\n"
               "    product VARCHAR,\n"
               "    FOREIGN KEY (customer_id) REFERENCES Customers(id)\n"
               ");");


    db.Request("INSERT INTO Customers(id, name,) VALUES (2, HUI);          ");
    db.Request("INSERT INTO Customers(id, name, bre, ) VALUES (3, JOHN, 34, );");
    db.Request("INSERT INTO Customers(id, name, bre) VALUES (4, MISHA, 7); ");
    db.Request("INSERT INTO Customers(id, name, bre) VALUES (5, MISHA, 12);");

    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (1, 2, SDFDFSD)");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (2, 2, TMN)");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (3, 3, FEWEFEW)");

    db.Request("UPDATE Customers SET name=hui WHERE id = 5");

//    auto res = db.RequestQuery("SELECT id, name FROM Customers WHERE bre IS NOT NULL AND id>3;");
    auto res = db.RequestQuery("SELECT * FROM Customers INNER JOIN Orders ON Customers.id = Orders.customer_id"
                               " WHERE bre IS NOT NULL AND id>3;");

    db.Save("dads.mcdb");
//    while (res.Next()) {
//        std::cout << res.Get<int>("id") << " " << res.Get<std::string>("name") << std::endl;
//    }


//    db.Request("INSERT INTO Orders(id, customer_id) VALUES (1, 3)");
//    db.Request("INSERT INTO Orders(id, customer_id) VALUES (2, 4)");
//    db.Request("INSERT INTO Orders(id, customer_id) VALUES (3, 3)");
//
//    db.Request("UPDATE Orders SET customer_id=4");
//
//    db.Request("DELETE FROM Orders");
//    ResultSet t1 = db.RequestQuery("SELECT * FROM Orders");
//
//    while (t1.Next()) {
//        auto e1 = t1.Get<int>("customer_id");
//        std::cout << (t1.Get<int>("id")) << " " << e1 << std::endl;
//    }


//    SqlQuery sql("SELECT table1.column1, table1.column2\n"
//                 "FROM table1 \n"
//                 "INNER JOIN table2  ON table1.id = table1.table1_id AND table1.category = 'Category1'\n"
//                 "WHERE t2.status = 'Active';");

    return 0;
}
