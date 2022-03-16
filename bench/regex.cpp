#include <regex>
#include <string>

#include <re2/re2.h>
#include <catch2/catch.hpp>

TEST_CASE("Regex", "[regex]") {
  const std::string str = "https://www.abc.com";
  auto pattern = R"(([a-zA-Z][a-zA-Z0-9]*)://([^ /]+)(/[^ ]*)?)";

  const std::regex std_regex(pattern, std::regex_constants::optimize);
  const re2::RE2 re2_regex(pattern);

  BENCHMARK("std match") { return std::regex_match(str, std_regex); };
  BENCHMARK("re2 match") { return re2::RE2::FullMatch(str, re2_regex); };
}
