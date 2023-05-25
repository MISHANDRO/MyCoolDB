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

}

TEST(DBTestSuite, ForeignKeyTest) {

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

}

TEST(DBTestSuite, UglySqlQueryTest) {

}
