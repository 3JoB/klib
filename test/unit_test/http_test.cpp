#include <filesystem>
#include <string>

#include <boost/json.hpp>
#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

#ifdef KLIB_TEST_USE_PROXY
const char* const proxy = "socks5://127.0.0.1:1080";
#endif

TEST_CASE("headers", "[http]") {
  klib::Request request;
  request.allow_redirects(false);
  request.set_connect_timeout(5);
  request.set_timeout(30);
  request.set_curl_user_agent();

#ifndef NDEBUG
  request.verbose(true);
#endif
#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy(proxy);
#endif

  const std::string header_key = "Authorization";
  const std::string header_value = "123456";

  auto response = request.get("http://httpbin.org/headers", {},
                              {{header_key, header_value}});
  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);
  auto headers = boost::json::parse(response.text()).at("headers");
  REQUIRE(headers.at(header_key).as_string() == header_value);

  response = request.get("http://httpbin.org/headers");
  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);
  headers = boost::json::parse(response.text()).at("headers");
  REQUIRE_FALSE(headers.as_object().contains(header_key));
}

TEST_CASE("GET", "[http]") {
  klib::Request request;
  request.use_http_2();
  request.set_connect_timeout(5);
  request.set_timeout(30);
  request.set_browser_user_agent();

#ifndef NDEBUG
  request.verbose(true);
#endif
#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy(proxy);
#endif

  const std::string key1 = "a";
  const std::string value1 = "111";
  const std::string key2 = "b";
  const std::string value2 = "你好世界";

  auto response =
      request.get("http://httpbin.org/get", {{key1, value1}, {key2, value2}});
  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);

  auto args = boost::json::parse(response.text()).at("args");
  REQUIRE(args.at(key1).as_string() == value1);
  REQUIRE(args.at(key2).as_string() == value2);
}

TEST_CASE("POST", "[http]") {
  klib::Request request;
  request.use_http_1_1();
  request.set_connect_timeout(5);
  request.set_timeout(30);
  request.set_browser_user_agent();

#ifndef NDEBUG
  request.verbose(true);
#endif
#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy(proxy);
#endif

  const std::string user_name = "kaiser";
  const std::string password = "123456";

  const std::string file_a = "a.txt";
  const std::string content_a = "aaa";
  const std::string file_b = "b.txt";
  const std::string content_b = "bbb";
  klib::write_file(file_a, false, content_a);
  klib::write_file(file_b, false, content_b);
  REQUIRE(std::filesystem::exists(file_a));
  REQUIRE(std::filesystem::exists(file_b));

  auto response =
      request.post("http://httpbin.org/post",
                   {{"user_name", user_name}, {"password", password}},
                   {{file_a, file_a}, {file_b, file_b}});
  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);

  auto map = response.headers_map();
  REQUIRE(map.at("content-TYPE") == "application/json");

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
#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy(proxy);
#endif

  const std::string user_name = "kaiser";
  const std::string password = "123456";

  boost::json::object obj;
  obj["user_name"] = user_name;
  obj["password"] = password;

  auto response =
      request.post("http://httpbin.org/post", boost::json::serialize(obj),
                   {{"Content-Type", "application/json"}});
  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);

  auto jv = boost::json::parse(response.text());

  REQUIRE(jv.at("data").as_string() == boost::json::serialize(obj));
}
