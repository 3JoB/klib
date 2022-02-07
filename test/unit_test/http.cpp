#include <filesystem>
#include <string>

#include <boost/json.hpp>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/http.h"
#include "klib/util.h"

const std::string httpbin_url = "https://httpbin.org";

TEST_CASE("request headers", "[http]") {
  klib::Request request;
  request.allow_redirects(false);
  request.set_curl_user_agent();
  request.use_cookies(false);

#ifndef NDEBUG
  request.verbose(true);
#endif

  const std::string header_key = "Authorization";
  const std::string header_value = "你好123456";

  auto response = request.get(httpbin_url + "/headers", {},
                              {{header_key, request.url_encode(header_value)}});
  REQUIRE(response.ok());
  auto headers = boost::json::parse(response.text()).at("headers");
  REQUIRE(request.url_decode(headers.at(header_key).as_string().c_str()) ==
          header_value);

  response = request.get(httpbin_url + "/headers");
  REQUIRE(response.ok());
  headers = boost::json::parse(response.text()).at("headers");
  REQUIRE_FALSE(headers.as_object().contains(header_key));
}

TEST_CASE("response headers", "[http]") {
  klib::Request request;
  request.allow_redirects(true);
  request.set_curl_user_agent();

#ifndef NDEBUG
  request.verbose(true);
#endif

  const std::string cookie1 = "a";
  const std::string value1 = "111";
  const std::string cookie2 = "b";
  const std::string value2 = "222";

  auto response = request.get(httpbin_url + "/cookies/set",
                              {{cookie1, value1}, {cookie2, value2}});
  REQUIRE(response.ok());
}

TEST_CASE("GET", "[http]") {
  klib::Request request;
  request.set_browser_user_agent();
  request.set_accept_encoding("gzip, deflate, br");

#ifndef NDEBUG
  request.verbose(true);
#endif
  const std::string key1 = "a";
  const std::string value1 = "111";
  const std::string key2 = "b";
  const std::string value2 = "你好世界";

  auto response =
      request.get(httpbin_url + "/get", {{key1, value1}, {key2, value2}});
  REQUIRE(response.ok());

  auto args = boost::json::parse(response.text()).at("args");
  REQUIRE(args.at(key1).as_string() == value1);
  REQUIRE(args.at(key2).as_string() == value2);
}

TEST_CASE("POST", "[http]") {
  klib::Request request;

#ifndef NDEBUG
  request.verbose(true);
#endif

  const std::string user_name = "你好kaiser";
  const std::string password = "123456";

  auto response = request.post(httpbin_url + "/post", {{"user_name", user_name},
                                                       {"password", password}});
  REQUIRE(response.ok());

  auto jv = boost::json::parse(response.text());

  const auto& form = jv.at("form");
  REQUIRE(form.at("user_name").as_string() == user_name);
  REQUIRE(form.at("password").as_string() == password);
}

TEST_CASE("POST mime", "[http]") {
  klib::Request request;
  request.set_browser_user_agent();

#ifndef NDEBUG
  request.verbose(true);
#endif

  const std::string user_name = "kaiser";
  const std::string password = "123456";

  const std::string file_a = "你好.txt";
  const std::string content_a = "你好你好";
  const std::string file_b = "b.txt";
  const std::string content_b = "bbb";
  klib::write_file(file_a, false, content_a);
  klib::write_file(file_b, false, content_b);
  REQUIRE(std::filesystem::exists(file_a));
  REQUIRE(std::filesystem::exists(file_b));

  auto response = request.post_mime(
      httpbin_url + "/post", {{"user_name", user_name}, {"password", password}},
      {{file_a, file_a}, {file_b, file_b}});
  REQUIRE(response.ok());

  auto jv = boost::json::parse(response.text());

  const auto& form = jv.at("form");
  REQUIRE(form.at("user_name").as_string() == user_name);
  REQUIRE(form.at("password").as_string() == password);

  const auto& files = jv.at("files");
  REQUIRE(files.at(file_a).as_string() == klib::read_file(file_a, false));
  REQUIRE(files.at(file_b).as_string() == klib::read_file(file_b, false));

  REQUIRE(std::filesystem::remove(file_a));
  REQUIRE(std::filesystem::remove(file_b));
}

TEST_CASE("POST json", "[http]") {
  klib::Request request;

#ifndef NDEBUG
  request.verbose(true);
#endif

  const std::string user_name = "你好kaiser";
  const std::string password = "123456";

  boost::json::object obj;
  obj["user_name"] = user_name;
  obj["password"] = password;

  auto response =
      request.post(httpbin_url + "/post", boost::json::serialize(obj));
  REQUIRE(response.ok());

  auto jv = boost::json::parse(response.text());

  REQUIRE(jv.at("data").as_string() == boost::json::serialize(obj));
}

TEST_CASE("download", "[http]") {
  klib::Request request;
  request.set_browser_user_agent();
  request.set_doh_url("https://dns.google/dns-query");

#ifndef NDEBUG
  request.verbose(true);
#endif
#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy("http://127.0.0.1:1080");
#endif

  auto response = request.get(
      "https://github.com/facebook/zstd/archive/refs/tags/v1.5.0.tar.gz");
  REQUIRE(response.ok());
  response.save_to_file("zstd-1.5.0.tar.gz", true);

  REQUIRE(std::filesystem::is_regular_file("zstd-1.5.0.tar.gz"));
  REQUIRE(klib::sha256_hex(klib::read_file("zstd-1.5.0.tar.gz", true)) ==
          "0d9ade222c64e912d6957b11c923e214e2e010a18f39bec102f572e693ba2867");
  std::filesystem::remove("zstd-1.5.0.tar.gz");
}
