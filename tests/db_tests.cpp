#include "MyCoolDB.h"

#include "gtest/gtest.h"

MyCoolDB GetTestDB() {
    MyCoolDB db;

    db.Request("CREATE TABLE Customers ("
               "    id    INT,"
               "    name  VARCHAR,"
               "    age   DOUBLE,"
               "    male  BOOL"
               ");");

    db.Request("INSERT INTO Customers(id, name, age, male) VALUES (1, 'Michael Scott', 41.3, true)");
    db.Request("INSERT INTO Customers(id, name) VALUES (2, 'Dwight Schrute')");
    db.Request("INSERT INTO Customers(id, name, age) VALUES (3, 'Jim Halpert', 29.1)");
    db.Request("INSERT INTO Customers(id, name, male) VALUES (4, 'Pam Beesly', false)");

    return db;
}

TEST(DBTestSuite, CreateTableTest) {
    MyCoolDB db;

    db.Request("CREATE TABLE Customers ("
               "    id    INT,"
               "    name  VARCHAR,"
               "    age   DOUBLE"
               ");");
}

TEST(DBTestSuite, InsertTest) {
    MyCoolDB db;

    db.Request("CREATE TABLE Customers ("
               "    id    INT,"
               "    name  VARCHAR,"
               "    age   DOUBLE,"
               "    male  BOOL"
               ");");

    db.Request("INSERT INTO Customers(id, name, age, male) VALUES (1, 'Michael Scott', 41.3, true)");
    db.Request("INSERT INTO Customers(id, name) VALUES (2, 'Dwight Schrute')");
    db.Request("INSERT INTO Customers(id, name, age) VALUES (3, 'Jim Halpert', 29.1)");
    db.Request("INSERT INTO Customers(id, name, male) VALUES (4, 'Pam Beesly', false)");

    ResultSet resSet = db.RequestQuery("SELECT * FROM Customers");
    ASSERT_EQ(resSet.Count(), 4);

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 1);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Michael Scott");
    ASSERT_EQ(resSet.Get<double>("age").Value(), 41.3);
    ASSERT_EQ(resSet.Get<bool>("male").Value(), true);

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 2);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Dwight Schrute");
    ASSERT_TRUE(resSet.Get<double>("age").IsNull());
    ASSERT_TRUE(resSet.Get<bool>("male").IsNull());

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 3);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Jim Halpert");
    ASSERT_EQ(resSet.Get<double>("age").Value(), 29.1);
    ASSERT_TRUE(resSet.Get<bool>("male").IsNull());

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 4);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Pam Beesly");
    ASSERT_TRUE(resSet.Get<double>("age").IsNull());
    ASSERT_EQ(resSet.Get<bool>("male").Value(), false);

    ASSERT_FALSE(resSet.Next());
}

TEST(DBTestSuite, SelectTest) {
    MyCoolDB db = GetTestDB();

    ResultSet resSet1 = db.RequestQuery("SELECT id, male FROM Customers WHERE name = 'Pam Beesly'");
    ASSERT_EQ(resSet1.Count(), 1);
    ASSERT_EQ(resSet1.Get<int>("id").Value(), 4);
    ASSERT_EQ(resSet1.Get<bool>("male").Value(), false);

    ResultSet resSet2 = db.RequestQuery("SELECT name FROM Customers WHERE id <= 2");
    ASSERT_EQ(resSet2.Count(), 2);
    resSet2.Next();
    ASSERT_EQ(resSet2.Get<std::string>("name").Value(), "Michael Scott");
    resSet2.Next();
    ASSERT_EQ(resSet2.Get<std::string>("name").Value(), "Dwight Schrute");
    ASSERT_FALSE(resSet2.Next());

    ResultSet resSet3 = db.RequestQuery("SELECT name, age FROM Customers WHERE age <= 40 AND male IS NULL");
    ASSERT_EQ(resSet3.Count(), 1);
    ASSERT_EQ(resSet3.Get<std::string>("name").Value(), "Jim Halpert");
    ASSERT_EQ(resSet3.Get<double>("age").Value(), 29.1);
    ASSERT_FALSE(resSet2.Next());

    ResultSet resSet4 = db.RequestQuery("SELECT id FROM Customers WHERE id > 3 OR age IS NOT NULL");
    ASSERT_EQ(resSet4.Count(), 3);
    resSet4.Next();
    ASSERT_EQ(resSet4.Get<int>("id").Value(), 1);
    resSet4.Next();
    ASSERT_EQ(resSet4.Get<int>("id").Value(), 3);
    resSet4.Next();
    ASSERT_EQ(resSet4.Get<int>("id").Value(), 4);
    ASSERT_FALSE(resSet2.Next());

    ResultSet resSet5 = db.RequestQuery("SELECT id FROM Customers WHERE male != true");
    ASSERT_EQ(resSet5.Count(), 1);
    ASSERT_EQ(resSet5.Get<int>("id").Value(), 4);
    ASSERT_FALSE(resSet5.Next());
}

TEST(DBTestSuite, UpdateTest) {
    MyCoolDB db = GetTestDB();

    db.Request("UPDATE Customers SET age = 38.2, male = true WHERE id = 2");
    db.Request("UPDATE Customers SET age = NULL WHERE id = 3");
    ResultSet resSet = db.RequestQuery("SELECT * FROM Customers WHERE id = 2 OR id = 3");
    ASSERT_EQ(resSet.Count(), 2);

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 2);
    ASSERT_EQ(resSet.Get<double>("age").Value(), 38.2);
    ASSERT_EQ(resSet.Get<bool>("male").Value(), true);

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 3);
    ASSERT_TRUE(resSet.Get<double>("age").IsNull());
}

TEST(DBTestSuite, DeleteTest) {
    MyCoolDB db = GetTestDB();

    db.Request("DELETE FROM Customers WHERE id <= 2");
    ResultSet resSet1 = db.RequestQuery("SELECT * FROM Customers");
    ASSERT_EQ(resSet1.Count(), 2);

    db.Request("DELETE FROM Customers");
    ResultSet resSet2 = db.RequestQuery("SELECT * FROM Customers");
    ASSERT_EQ(resSet2.Count(), 0);
}

TEST(DBTestSuite, DropTableTest) {
    MyCoolDB db = GetTestDB();

    db.Request("CREATE TABLE Test_table ("
               "    test BOOL"
               ")");

    ASSERT_EQ(db.GetTableNames(), std::vector<std::string>({"Customers", "Test_table"}));
    db.Request("DROP TABLE Customers");
    ASSERT_EQ(db.GetTableNames(), std::vector<std::string>({"Test_table"}));
    db.Request("DROP TABLE Test_table");
    ASSERT_EQ(db.GetTableNames(), std::vector<std::string>());
}

TEST(DBTestSuite, PrimaryKeyTest) {
    MyCoolDB db;

    db.Request("CREATE TABLE Products ("
               "    id INT PRIMARY KEY,"
               "    name VARCHAR"
               ")");

    db.Request("INSERT INTO Products(id, name) VALUES (1, 'Milk')");
    ASSERT_THROW(db.Request("INSERT INTO Products(id, name) VALUES (1, 'Eggs')"), SqlException);
    db.Request("INSERT INTO Products(id, name) VALUES (2, 'Eggs')");
    ASSERT_THROW(db.Request("INSERT INTO Products(id, name) VALUES (2, 'Coffee')"), SqlException);
}

TEST(DBTestSuite, ForeignKeyTest) {
    MyCoolDB db = GetTestDB();

    db.Request("CREATE TABLE Orders ("
               "    id INT PRIMARY KEY,"
               "    customer_id INT,"
               "    product VARCHAR,"
               "    FOREIGN KEY (customer_id) REFERENCES Customers(id)"
               ")");

    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (1, 1, 'Milk')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (2, 1, 'Doshirak')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (3, 2, 'Coffee')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (4, 2, 'Beckon')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (5, 3, 'Adrenalin Rush')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (6, 4, 'Cocoa')");
    ASSERT_THROW(db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (7, 5, 'PC')"), SqlException);
    ASSERT_THROW(db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (8, 6, 'House')"), SqlException);
}

TEST(DBTestSuite, SaveLoadTest) {
    MyCoolDB test_db = GetTestDB();
    test_db.Save("test.mcdb"); // mcdb - My Cool Data Base

    MyCoolDB db;
    db.Load("test.mcdb");

    ResultSet resSet = db.RequestQuery("SELECT * FROM Customers");
    ASSERT_EQ(resSet.Count(), 4);

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 1);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Michael Scott");
    ASSERT_EQ(resSet.Get<double>("age").Value(), 41.3);
    ASSERT_EQ(resSet.Get<bool>("male").Value(), true);

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 2);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Dwight Schrute");
    ASSERT_TRUE(resSet.Get<double>("age").IsNull());
    ASSERT_TRUE(resSet.Get<bool>("male").IsNull());

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 3);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Jim Halpert");
    ASSERT_EQ(resSet.Get<double>("age").Value(), 29.1);
    ASSERT_TRUE(resSet.Get<bool>("male").IsNull());

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 4);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Pam Beesly");
    ASSERT_TRUE(resSet.Get<double>("age").IsNull());
    ASSERT_EQ(resSet.Get<bool>("male").Value(), false);

    ASSERT_FALSE(resSet.Next());
}

TEST(DBTestSuite, JoinTest) {
    MyCoolDB db = GetTestDB();

    db.Request("CREATE TABLE Orders ("
               "    id INT PRIMARY KEY,"
               "    customer_id INT,"
               "    product VARCHAR,"
               ")");

    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (1, 1, 'Milk')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (2, 1, 'Doshirak')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (3, 2, 'Beet')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (4, 2, 'Beckon')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (5, 4, 'Cocoa')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (6, 5, 'PC')");
    db.Request("INSERT INTO Orders(id, customer_id, product) VALUES (7, 6, 'House')");

    ResultSet resSet1 = db.RequestQuery("SELECT * FROM Customers INNER JOIN Orders ON Orders.customer_id = Customers.id");
    ASSERT_EQ(resSet1.Count(), 5);
    resSet1.Next();
    ASSERT_EQ(resSet1.Get<int>("Orders.id").Value(), 1);
    ASSERT_EQ(resSet1.Get<int>("Customers.id").Value(), 1);
    ASSERT_EQ(resSet1.Get<std::string>("Customers.name").Value(), "Michael Scott");
    ASSERT_EQ(resSet1.Get<std::string>("Orders.product").Value(), "Milk");

    resSet1.Next();
    ASSERT_EQ(resSet1.Get<int>("Orders.id").Value(), 2);
    ASSERT_EQ(resSet1.Get<int>("Customers.id").Value(), 1);
    ASSERT_EQ(resSet1.Get<std::string>("Customers.name").Value(), "Michael Scott");
    ASSERT_EQ(resSet1.Get<std::string>("Orders.product").Value(), "Doshirak");

    resSet1.Next();
    ASSERT_EQ(resSet1.Get<int>("Orders.id").Value(), 3);
    ASSERT_EQ(resSet1.Get<int>("Customers.id").Value(), 2);
    ASSERT_EQ(resSet1.Get<std::string>("Customers.name").Value(), "Dwight Schrute");
    ASSERT_EQ(resSet1.Get<std::string>("Orders.product").Value(), "Beet");

    resSet1.Next();
    ASSERT_EQ(resSet1.Get<int>("Orders.id").Value(), 4);
    ASSERT_EQ(resSet1.Get<int>("Customers.id").Value(), 2);
    ASSERT_EQ(resSet1.Get<std::string>("Customers.name").Value(), "Dwight Schrute");
    ASSERT_EQ(resSet1.Get<std::string>("Orders.product").Value(), "Beckon");

    resSet1.Next();
    ASSERT_EQ(resSet1.Get<int>("Orders.id").Value(), 5);
    ASSERT_EQ(resSet1.Get<int>("Customers.id").Value(), 4);
    ASSERT_EQ(resSet1.Get<std::string>("Customers.name").Value(), "Pam Beesly");
    ASSERT_EQ(resSet1.Get<std::string>("Orders.product").Value(), "Cocoa");
    ASSERT_FALSE(resSet1.Next());


    ResultSet resSet2 = db.RequestQuery("SELECT * FROM Customers LEFT JOIN Orders ON Orders.customer_id = Customers.id");
    ASSERT_EQ(resSet2.Count(), 6);
    resSet2.Next();
    resSet2.Next();
    resSet2.Next();
    resSet2.Next();
    resSet2.Next();
    ASSERT_TRUE(resSet2.Get<int>("Orders.id").IsNull());
    ASSERT_TRUE(resSet2.Get<int>("Orders.customer_id").IsNull());
    ASSERT_TRUE(resSet2.Get<std::string>("Orders.product").IsNull());
    ASSERT_EQ(resSet2.Get<int>("Customers.id").Value(), 3);
    ASSERT_EQ(resSet2.Get<std::string>("Customers.name").Value(), "Jim Halpert");

    ResultSet resSet3 = db.RequestQuery("SELECT * FROM Customers RIGHT JOIN Orders ON Orders.customer_id = Customers.id");
    ASSERT_EQ(resSet3.Count(), 7);
    resSet3.Next();
    resSet3.Next();
    resSet3.Next();
    resSet3.Next();
    resSet3.Next();
    resSet3.Next();
    ASSERT_TRUE(resSet3.Get<int>("Customers.id").IsNull());
    ASSERT_TRUE(resSet3.Get<std::string>("Customers.name").IsNull());
    ASSERT_TRUE(resSet3.Get<double>("Customers.age").IsNull());
    ASSERT_EQ(resSet3.Get<int>("Orders.id").Value(), 6);
    ASSERT_EQ(resSet3.Get<std::string>("Orders.product").Value(), "PC");

    resSet3.Next();
    ASSERT_TRUE(resSet3.Get<int>("Customers.id").IsNull());
    ASSERT_TRUE(resSet3.Get<std::string>("Customers.name").IsNull());
    ASSERT_TRUE(resSet3.Get<double>("Customers.age").IsNull());
    ASSERT_EQ(resSet3.Get<int>("Orders.id").Value(), 7);
    ASSERT_EQ(resSet3.Get<std::string>("Orders.product").Value(), "House");


    ResultSet resSet4 = db.RequestQuery("SELECT * FROM Customers INNER JOIN Orders ON Orders.customer_id = Customers.id "
                                        "WHERE Customers.id >= 3 OR Orders.product = 'Doshirak'");
    ASSERT_EQ(resSet4.Count(), 2);
    resSet4.Next();
    ASSERT_EQ(resSet4.Get<int>("Orders.id").Value(), 2);
    ASSERT_EQ(resSet4.Get<int>("Customers.id").Value(), 1);
    ASSERT_EQ(resSet4.Get<std::string>("Customers.name").Value(), "Michael Scott");
    ASSERT_EQ(resSet4.Get<std::string>("Orders.product").Value(), "Doshirak");

    resSet4.Next();
    ASSERT_EQ(resSet4.Get<int>("Orders.id").Value(), 5);
    ASSERT_EQ(resSet4.Get<int>("Customers.id").Value(), 4);
    ASSERT_EQ(resSet4.Get<std::string>("Customers.name").Value(), "Pam Beesly");
    ASSERT_EQ(resSet4.Get<std::string>("Orders.product").Value(), "Cocoa");
}

TEST(DBTestSuite, UglySqlQueryTest) {
    MyCoolDB db;

    db.Request("CREATE TABLE Products ("
               "    id INT PRIMARY KEY,"
               "    name VARCHAR"
               ")");

    db.Request("INSERT   INTO    Products(id, name) VALUES   (1, 'Milk') ; ;;");
    db.Request("INSERT      INTO     Products(id  ,    name)      VALUES (2,    'Eggs', );;;;");

    ResultSet resSet = db.RequestQuery("SELECT  *  FROM    Products ; ;");
    ASSERT_EQ(resSet.Count(), 2);
    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 1);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Milk");

    resSet.Next();
    ASSERT_EQ(resSet.Get<int>("id").Value(), 2);
    ASSERT_EQ(resSet.Get<std::string>("name").Value(), "Eggs");
    ASSERT_FALSE(resSet.Next());
}
