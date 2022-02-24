#include <filesystem>
#include <string>

#include <simdjson.h>
#include <boost/json.hpp>
#include <catch2/catch.hpp>

#include "klib/util.h"

TEST_CASE("json", "[json]") {
  const std::string file_name = "twitter.json";
  REQUIRE(std::filesystem::exists(file_name));

  auto json = klib::read_file(file_name, false);
  json.reserve(std::size(json) + simdjson::SIMDJSON_PADDING);

  std::string s1, s2;

  BENCHMARK("Boost.json") {
    boost::json::error_code error;
    boost::json::monotonic_resource mr;
    auto doc = boost::json::parse(json, error, &mr);
    s1 = doc.at("statuses").at(60).at("user").at("url").as_string();
  };

  BENCHMARK("simdjson") {
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json);
    s2 = doc["statuses"].at(60)["user"]["url"].get_string().value();
  };

  REQUIRE(s1 == s2);
}
