#include "klib/url.h"

#include <charconv>
#include <vector>

#include <curl/curl.h>
#include <boost/algorithm/string.hpp>
#include <scope_guard.hpp>

#include "klib/exception.h"

#define CHECK_CURL_URL(rc)                         \
  do {                                             \
    if (rc != CURLUcode::CURLUE_OK) [[unlikely]] { \
      throw RuntimeError(curl_url_strerror(rc));   \
    }                                              \
  } while (0)

namespace klib {

std::string url_encode(const std::string& str) {
  auto ptr = curl_easy_escape(nullptr, str.c_str(), 0);
  SCOPE_EXIT { curl_free(ptr); };
  if (!ptr) [[unlikely]] {
    throw RuntimeError("curl_easy_escape() failed");
  }

  return ptr;
}

std::string url_decode(const std::string& str) {
  std::int32_t length;
  auto ptr = curl_easy_unescape(nullptr, str.c_str(), 0, &length);
  SCOPE_EXIT { curl_free(ptr); };
  if (!ptr) [[unlikely]] {
    throw RuntimeError("curl_easy_unescape() failed");
  }

  return std::string(ptr, length);
}

URL::URL(const std::string& url) {
  auto c_url = curl_url();
  SCOPE_EXIT { curl_url_cleanup(c_url); };
  if (!c_url) {
    throw RuntimeError("curl_url() failed");
  }

  auto rc = curl_url_set(c_url, CURLUPART_URL, url.c_str(), 0);
  CHECK_CURL_URL(rc);

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_SCHEME, &str, 0);
    if (rc != CURLUcode::CURLUE_NO_SCHEME) {
      CHECK_CURL_URL(rc);
      schema_ = str;
    }
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_HOST, &str, 0);
    if (rc != CURLUcode::CURLUE_NO_HOST) {
      CHECK_CURL_URL(rc);
      host_ = str;
    }
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_PORT, &str, 0);
    if (rc != CURLUcode::CURLUE_NO_PORT) {
      CHECK_CURL_URL(rc);
      std::from_chars(str, str + std::strlen(str), port_);
    }
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_PATH, &str, CURLU_URLDECODE);
    CHECK_CURL_URL(rc);
    path_ = str;
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_QUERY, &str, CURLU_URLDECODE);
    if (rc != CURLUcode::CURLUE_NO_QUERY) {
      CHECK_CURL_URL(rc);

      std::string query_str = str;
      std::vector<std::string> query;
      for (const auto& item :
           boost::split(query, query_str, boost::is_any_of("&"),
                        boost::token_compress_on)) {
        auto index = item.find('=');
        query_.emplace(item.substr(0, index), item.substr(index + 1));
      }
    }
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_FRAGMENT, &str, 0);
    if (rc != CURLUcode::CURLUE_NO_FRAGMENT) {
      CHECK_CURL_URL(rc);
      fragment_ = str;
    }
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_USER, &str, 0);
    if (rc != CURLUcode::CURLUE_NO_USER) {
      CHECK_CURL_URL(rc);
      user_ = str;
    }
  }

  {
    char* str;
    SCOPE_EXIT { curl_free(str); };
    rc = curl_url_get(c_url, CURLUPART_PASSWORD, &str, 0);
    if (rc != CURLUcode::CURLUE_NO_PASSWORD) {
      CHECK_CURL_URL(rc);
      password_ = str;
    }
  }
}

std::string URL::to_string() const {
  auto c_url = curl_url();
  SCOPE_EXIT { curl_url_cleanup(c_url); };
  if (!c_url) {
    throw RuntimeError("curl_url() failed");
  }

  if (!std::empty(schema_)) {
    auto rc = curl_url_set(c_url, CURLUPART_SCHEME, schema_.c_str(), 0);
    CHECK_CURL_URL(rc);
  }

  if (!std::empty(host_)) {
    auto rc = curl_url_set(c_url, CURLUPART_HOST, host_.c_str(), 0);
    CHECK_CURL_URL(rc);
  }

  if (port_ != 0) {
    auto port_str = std::to_string(port_);
    auto rc = curl_url_set(c_url, CURLUPART_PORT, port_str.c_str(), 0);
    CHECK_CURL_URL(rc);
  }

  if (!std::empty(path_)) {
    auto rc =
        curl_url_set(c_url, CURLUPART_PATH, path_.c_str(), CURLU_URLENCODE);
    CHECK_CURL_URL(rc);
  }

  for (const auto& [key, value] : query_) {
    std::string query;
    query.append(key).append("=").append(value);

    auto rc = curl_url_set(c_url, CURLUPART_QUERY, query.c_str(),
                           CURLU_APPENDQUERY | CURLU_URLENCODE);
    CHECK_CURL_URL(rc);
  }

  if (!std::empty(fragment_)) {
    auto rc = curl_url_set(c_url, CURLUPART_FRAGMENT, fragment_.c_str(), 0);
    CHECK_CURL_URL(rc);
  }

  if (!std::empty(user_)) {
    auto rc = curl_url_set(c_url, CURLUPART_USER, user_.c_str(), 0);
    CHECK_CURL_URL(rc);
  }

  if (!std::empty(password_)) {
    auto rc = curl_url_set(c_url, CURLUPART_PASSWORD, password_.c_str(), 0);
    CHECK_CURL_URL(rc);
  }

  char* str;
  SCOPE_EXIT { curl_free(str); };
  auto rc = curl_url_get(c_url, CURLUPART_URL, &str, 0);
  CHECK_CURL_URL(rc);

  return str;
}

}  // namespace klib
