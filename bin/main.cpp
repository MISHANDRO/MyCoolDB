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

    db.Request("CREATE TABLE Customers (\n"
               "    id INT PRIMARY KEY,\n"
               "    name VARCHAR\n"
               ");");

    db.Request("CREATE TABLE Orders (\n"
               "    id INT PRIMARY KEY,\n"
               "    customer_id INT,\n"
               "    FOREIGN KEY (customer_id) REFERENCES Customers(id)\n"
               ");");


    db.Request("INSERT INTO Customers(id, name) VALUES (3, JOHN)");
    db.Request("INSERT INTO Customers(id, name) VALUES (4, MISHA)");
    db.Request("INSERT INTO Orders(id, customer_id) VALUES (1, 3)");
    db.Request("INSERT INTO Orders(id, customer_id) VALUES (2, 4)");
    db.Request("INSERT INTO Orders(id, customer_id) VALUES (3, 3)");

    db.Request("UPDATE Orders SET customer_id=4");

    db.Request("DELETE FROM Orders");
    ResultSet t1 = db.RequestQuery("SELECT * FROM Orders");

    while (t1.Next()) {
        auto e1 = t1.Get<int>("customer_id");
        std::cout << (t1.Get<int>("id")) << " " << e1 << std::endl;
    }

    return 0;
}
