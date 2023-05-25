#include "MyCoolDB.h"

#include <iostream>
#include <string>

int main() {

    MyCoolDB db;

    db.Request("CREATE TABLE Customers ("
               "    id   INT"
               "    bre  INT,"
               "    name VARCHAR,"
               ");");


    db.Request("CREATE TABLE Orders ("
               "    id          INT"
               "    customer_id INT,"
               "    product     VARCHAR,"
//               "    FOREIGN KEY (customer_id) REFERENCES Customers(id)"
               ");");


    db.Request("INSERT INTO Customers(id, name) VALUES (2, 'MICKLE');");
    db.Request("INSERT INTO Customers(id, name) VALUES (2, 'SKOtt Mi');");
    db.Request("INSERT INTO Customers(id, name, bre) VALUES (3, 'JOHN dewwed', 34);");
    db.Request("INSERT INTO Customers(id, name, bre) VALUES (4, 'MISHA', 7);");
    db.Request("INSERT INTO Customers(id, name, bre) VALUES (5, 'misha dewe', 12);");

    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (1, 2, 'coffee')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (2, 2, 'milk')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (3, 3, 'eggs')");

    db.Request("UPDATE Customers SET name=NULL WHERE id=5");

//    auto res = db.RequestQuery("SELECT * FROM Customers WHERE bre IS NOT NULL AND id > 3 AND bre > 8;");
    auto res = db.RequestQuery("SELECT * FROM Customers RIGHT JOIN Orders ON Orders.customer_id = Customers.id WHERE Customers.name='SKOtt Mi' OR Orders.product='milk'");

    db.Save("dads.mcdb");
    std::cout << res;
//    while (res.Next()) {
//        std::cout << std::endl;
//
//        std::string ew = res.Get<std::string>("name").Value();
//        std::cout << ew;
//    }
//
//
//    MyCoolDB db1;
//    db1.Load("dads.mcdb");
//
//    res = db1.RequestQuery("SELECT * FROM Customers WHERE bre IS NOT NULL AND id > 3 AND bre > 8;");
//
//    while (res.Next()) {
//        std::cout << res.Get<int>("id") << " "
//                  << res.Get<std::string>("name") << " "
//                  << res.Get<int>("bre") <<  std::endl;
//    }

//    SqlQuery sql("INSERT INTO Orders(id, customer_id, product) VALUES (1, 2, 'coffee hui pomelo'   )");
//    SqlQuery sql("SELECT * FROM Customers WHERE bre IS NOT NULL AND id > 3 AND chlen = 'dede uhuh';");
//    SqlQuery sql("UPDATE Customers SET name='base cdjncsd' WHERE id=5");

    return 0;
}
