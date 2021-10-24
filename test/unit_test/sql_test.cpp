#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/hash_lib.h"
#include "klib/sql.h"

TEST_CASE("sql", "[sql]") {
  {
    klib::SqlDatabase db("test.db", klib::SqlDatabase::ReadWrite,
                         klib::sha_256("zG2nSeEfSHfvTCHy5LCcqtBbQehKNLXn"));

    if (!db.table_exists("Cars")) {
      REQUIRE_NOTHROW(db.transaction());

      REQUIRE_NOTHROW(db.exec("CREATE TABLE Cars(Name TEXT, Price INT);"));

      std::vector<std::string> names = {"Audi",   "Mercedes",  "Skoda",
                                        "Volvo",  "Bentley",   "Citroen",
                                        "Hummer", "Volkswagen"};
      std::vector<std::int32_t> prices = {52642,  57127, 9000,  29000,
                                          350000, 21000, 41400, 21600};
      klib::SqlQuery query(db);
      REQUIRE_NOTHROW(
          query.prepare("INSERT INTO Cars(Name, Price) VALUES(?, ?);"));
      for (std::size_t i = 0; i < 8; ++i) {
        REQUIRE_NOTHROW(query.bind(1, names[i]));
        REQUIRE_NOTHROW(query.bind(2, prices[i]));
        REQUIRE_NOTHROW(query.step());
      }
      REQUIRE(db.exec("UPDATE Cars SET Name='aaa' WHERE Price > 50000") == 3);

      REQUIRE_NOTHROW(db.commit());
    }

    REQUIRE(std::filesystem::exists("test.db"));
    REQUIRE(db.table_exists("Cars"));
    REQUIRE(db.table_line_count("Cars") == 8);

    klib::SqlQuery query(db);
    REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars WHERE Price > ?"));
    REQUIRE_NOTHROW(query.bind(1, 50000));
    while (query.next()) {
      REQUIRE(query.get_column_name(0) == "Name");
      REQUIRE(query.get_column_name(1) == "Price");
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_string() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }

    std::cout << '\n';
    REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars"));
    while (query.next()) {
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_string() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }

    std::cout << '\n';
    while (query.next()) {
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_string() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }
  }

  {
    klib::SqlDatabase db("test.db", klib::SqlDatabase::ReadWrite,
                         klib::sha_256("zG2nSeEfSHfvTCHy5LCcqtBbQehKNLXn"));

    REQUIRE(db.table_exists("Cars"));
    REQUIRE(db.table_line_count("Cars") == 8);

    klib::SqlQuery query(db);
    REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars"));
    std::cout << '\n';
    while (query.next()) {
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_string() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }
  }
}
