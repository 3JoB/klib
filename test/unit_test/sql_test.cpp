#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/sql.h"

TEST_CASE("sql", "[sql]") {
  std::vector<std::string> names = {"Audi",   "Mercedes",  "Skoda",
                                    "Volvo",  "Bentley",   "Citroen",
                                    "Hummer", "Volkswagen"};
  std::vector<std::int32_t> prices = {52642,  57127, 9000,  29000,
                                      350000, 21000, 41400, 21600};

  klib::SqlDatabase db("test.db", klib::SqlDatabase::ReadWrite);
  REQUIRE_NOTHROW(db.drop_table("Cars"));

  REQUIRE_NOTHROW(db.transaction());
  klib::SqlQuery query(db);
  REQUIRE_NOTHROW(query.exec("CREATE TABLE Cars(Name TEXT, Price INT);"));
  REQUIRE_NOTHROW(query.prepare("INSERT INTO Cars(Name, Price) VALUES(?, ?);"));
  for (std::size_t i = 0; i < 8; ++i) {
    REQUIRE_NOTHROW(query.bind(1, names[i]));
    REQUIRE_NOTHROW(query.bind(2, prices[i]));
    REQUIRE_NOTHROW(query.step());
  }
  REQUIRE_NOTHROW(db.commit());

  REQUIRE(std::filesystem::exists("test.db"));
  REQUIRE(db.table_exists("Cars"));

  REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars WHERE Price > ?"));
  REQUIRE_NOTHROW(query.bind(1, 50000));
  while (query.next()) {
    REQUIRE_NOTHROW(std::cout << query.get_column(0).as_string() << ' ');
    REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
  }

  std::cout << '\n';
  REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars"));
  while (query.next()) {
    REQUIRE_NOTHROW(std::cout << query.get_column(0).as_string() << ' ');
    REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
  }
}
