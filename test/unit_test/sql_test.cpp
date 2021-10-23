#include <filesystem>

#include <catch2/catch.hpp>

#include <iostream>
#include "klib/sql.h"

TEST_CASE("sql", "[sql]") {
  klib::SqlDatabase db("test.db", klib::SqlDatabase::ReadWrite);
  db.drop_table("Cars");

  db.transaction();
  std::string_view sql =
      "CREATE TABLE Cars(Id INTEGER PRIMARY KEY, Name TEXT, Price INT);"
      "INSERT INTO Cars(Name, Price) VALUES('Audi', 52642);"
      "INSERT INTO Cars(Name, Price) VALUES('Mercedes', 57127);"
      "INSERT INTO Cars(Name, Price) VALUES('Skoda', 9000);"
      "INSERT INTO Cars(Name, Price) VALUES('Volvo', 29000);"
      "INSERT INTO Cars(Name, Price) VALUES('Bentley', 350000);"
      "INSERT INTO Cars(Name, Price) VALUES('Citroen', 21000);"
      "INSERT INTO Cars(Name, Price) VALUES('Hummer', 41400);"
      "INSERT INTO Cars(Name, Price) VALUES('Volkswagen', 21600);";
  db.exec(sql);
  db.commit();

  REQUIRE(std::filesystem::exists("test.db"));
  REQUIRE(db.table_exists("Cars"));

  sql = "SELECT * FROM Cars";
  klib::SqlQuery query(db, sql);

  while (query.next()) {
    std::cout << query.get_column(0).as_int32() << ' ';
    std::cout << query.get_column(1).as_string() << ' ';
    std::cout << query.get_column(2).as_int32() << '\n';
  }
}
