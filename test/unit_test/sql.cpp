#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/sql.h"
#include "klib/util.h"

TEST_CASE("sql", "[sql]") {
  {
    klib::SqlDatabase db("test.db", klib::SqlDatabase::ReadWrite,
                         "6K4VpQY5&b*WRR^Y");

    REQUIRE_NOTHROW(db.transaction());
    REQUIRE_NOTHROW(db.drop_table_if_exists("Cars"));
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
      REQUIRE(query.exec() == 1);
    }
    REQUIRE(db.exec("UPDATE Cars SET Name='aaa' WHERE Price > 50000") == 3);

    REQUIRE_NOTHROW(db.commit());

    REQUIRE_NOTHROW(db.transaction());
    for (std::size_t i = 0; i < 8; ++i) {
      REQUIRE_NOTHROW(query.bind(1, names[i]));
      REQUIRE_NOTHROW(query.bind(2, prices[i]));
      REQUIRE(query.exec() == 1);
    }
    REQUIRE_NOTHROW(db.rollback());

    REQUIRE_NOTHROW(db.vacuum());

    REQUIRE(std::filesystem::exists("test.db"));
    REQUIRE(db.table_exists("Cars"));
    REQUIRE(db.table_line_count("Cars") == 8);

    REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars WHERE Price > ?"));
    REQUIRE_NOTHROW(query.bind(1, 50000));
    while (query.next()) {
      REQUIRE(query.get_column_name(0) == "Name");
      REQUIRE(query.get_column_name(1) == "Price");
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_text() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }

    std::cout << '\n';
    REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars"));
    while (query.next()) {
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_text() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }

    std::cout << '\n';
    while (query.next()) {
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_text() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }
  }

  {
    klib::SqlDatabase db("test.db", klib::SqlDatabase::ReadWrite,
                         "6K4VpQY5&b*WRR^Y");

    REQUIRE(db.table_exists("Cars"));
    REQUIRE(db.table_line_count("Cars") == 8);

    klib::SqlQuery query(db);
    REQUIRE_NOTHROW(query.prepare("SELECT * FROM Cars"));
    std::cout << '\n';
    while (query.next()) {
      REQUIRE_NOTHROW(std::cout << query.get_column(0).as_text() << ' ');
      REQUIRE_NOTHROW(std::cout << query.get_column(1).as_int32() << '\n');
    }
  }
}

TEST_CASE("blob", "[sql]") {
  klib::SqlDatabase db("test2.db", klib::SqlDatabase::ReadWrite, "");

  REQUIRE_NOTHROW(db.transaction());
  REQUIRE_NOTHROW(db.drop_table_if_exists("BlobTest"));
  REQUIRE_NOTHROW(db.exec("CREATE TABLE BlobTest(Data BLOB);"));

  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  std::string blob = klib::read_file("zlib-v1.2.11.tar.gz", true);

  klib::SqlQuery query(db);
  REQUIRE_NOTHROW(query.prepare("INSERT INTO BlobTest(Data) VALUES(?);"));
  REQUIRE_NOTHROW(query.bind(1, std::data(blob), std::size(blob)));
  REQUIRE(query.exec() == 1);

  REQUIRE_NOTHROW(db.commit());

  REQUIRE_NOTHROW(query.prepare("SELECT * FROM BlobTest"));
  REQUIRE(query.next());
  REQUIRE(query.get_column(0).as_blob() == blob);
  REQUIRE_NOTHROW(query.finalize());

  REQUIRE_NOTHROW(db.vacuum());
}
