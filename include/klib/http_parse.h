/**
 * @file url.h
 * @brief Contains HTTP parse module
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

/**
 * @brief HTTP Status
 * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status
 */
#define HTTP_STATUS(XX)                                                     \
  XX(100, CONTINUE, Continue)                                               \
  XX(101, SWITCHING_PROTOCOLS, Switching Protocols)                         \
  XX(102, PROCESSING, Processing)                                           \
  XX(200, OK, OK)                                                           \
  XX(201, CREATED, Created)                                                 \
  XX(202, ACCEPTED, Accepted)                                               \
  XX(203, NON_AUTHORITATIVE_INFORMATION, Non - Authoritative Information)   \
  XX(204, NO_CONTENT, No Content)                                           \
  XX(205, RESET_CONTENT, Reset Content)                                     \
  XX(206, PARTIAL_CONTENT, Partial Content)                                 \
  XX(207, MULTI_STATUS, Multi - Status)                                     \
  XX(208, ALREADY_REPORTED, Already Reported)                               \
  XX(226, IM_USED, IM Used)                                                 \
  XX(300, MULTIPLE_CHOICES, Multiple Choices)                               \
  XX(301, MOVED_PERMANENTLY, Moved Permanently)                             \
  XX(302, FOUND, Found)                                                     \
  XX(303, SEE_OTHER, See Other)                                             \
  XX(304, NOT_MODIFIED, Not Modified)                                       \
  XX(305, USE_PROXY, Use Proxy)                                             \
  XX(307, TEMPORARY_REDIRECT, Temporary Redirect)                           \
  XX(308, PERMANENT_REDIRECT, Permanent Redirect)                           \
  XX(400, BAD_REQUEST, Bad Request)                                         \
  XX(401, UNAUTHORIZED, Unauthorized)                                       \
  XX(402, PAYMENT_REQUIRED, Payment Required)                               \
  XX(403, FORBIDDEN, Forbidden)                                             \
  XX(404, NOT_FOUND, Not Found)                                             \
  XX(405, METHOD_NOT_ALLOWED, Method Not Allowed)                           \
  XX(406, NOT_ACCEPTABLE, Not Acceptable)                                   \
  XX(407, PROXY_AUTHENTICATION_REQUIRED, Proxy Authentication Required)     \
  XX(408, REQUEST_TIMEOUT, Request Timeout)                                 \
  XX(409, CONFLICT, Conflict)                                               \
  XX(410, GONE, Gone)                                                       \
  XX(411, LENGTH_REQUIRED, Length Required)                                 \
  XX(412, PRECONDITION_FAILED, Precondition Failed)                         \
  XX(413, PAYLOAD_TOO_LARGE, Payload Too Large)                             \
  XX(414, URI_TOO_LONG, URI Too Long)                                       \
  XX(415, UNSUPPORTED_MEDIA_TYPE, Unsupported Media Type)                   \
  XX(416, RANGE_NOT_SATISFIABLE, Range Not Satisfiable)                     \
  XX(417, EXPECTATION_FAILED, Expectation Failed)                           \
  XX(421, MISDIRECTED_REQUEST, Misdirected Request)                         \
  XX(422, UNPROCESSABLE_ENTITY, Unprocessable Entity)                       \
  XX(423, LOCKED, Locked)                                                   \
  XX(424, FAILED_DEPENDENCY, Failed Dependency)                             \
  XX(426, UPGRADE_REQUIRED, Upgrade Required)                               \
  XX(428, PRECONDITION_REQUIRED, Precondition Required)                     \
  XX(429, TOO_MANY_REQUESTS, Too Many Requests)                             \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS, Unavailable For Legal Reasons)     \
  XX(500, INTERNAL_SERVER_ERROR, Internal Server Error)                     \
  XX(501, NOT_IMPLEMENTED, Not Implemented)                                 \
  XX(502, BAD_GATEWAY, Bad Gateway)                                         \
  XX(503, SERVICE_UNAVAILABLE, Service Unavailable)                         \
  XX(504, GATEWAY_TIMEOUT, Gateway Timeout)                                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED, HTTP Version Not Supported)           \
  XX(506, VARIANT_ALSO_NEGOTIATES, Variant Also Negotiates)                 \
  XX(507, INSUFFICIENT_STORAGE, Insufficient Storage)                       \
  XX(508, LOOP_DETECTED, Loop Detected)                                     \
  XX(510, NOT_EXTENDED, Not Extended)                                       \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required)

/**
 *
 */
#define HTTP_METHOD(XX)            \
  XX(0, DELETE, DELETE)            \
  XX(1, GET, GET)                  \
  XX(2, HEAD, HEAD)                \
  XX(3, POST, POST)                \
  XX(4, PUT, PUT)                  \
  XX(5, CONNECT, CONNECT)          \
  XX(6, OPTIONS, OPTIONS)          \
  XX(7, TRACE, TRACE)              \
  XX(8, COPY, COPY)                \
  XX(9, LOCK, LOCK)                \
  XX(10, MKCOL, MKCOL)             \
  XX(11, MOVE, MOVE)               \
  XX(12, PROPFIND, PROPFIND)       \
  XX(13, PROPPATCH, PROPPATCH)     \
  XX(14, SEARCH, SEARCH)           \
  XX(15, UNLOCK, UNLOCK)           \
  XX(16, BIND, BIND)               \
  XX(17, REBIND, REBIND)           \
  XX(18, UNBIND, UNBIND)           \
  XX(19, ACL, ACL)                 \
  XX(20, REPORT, REPORT)           \
  XX(21, MKACTIVITY, MKACTIVITY)   \
  XX(22, CHECKOUT, CHECKOUT)       \
  XX(23, MERGE, MERGE)             \
  XX(24, MSEARCH, M - SEARCH)      \
  XX(25, NOTIFY, NOTIFY)           \
  XX(26, SUBSCRIBE, SUBSCRIBE)     \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE) \
  XX(28, PATCH, PATCH)             \
  XX(29, PURGE, PURGE)             \
  XX(30, MKCALENDAR, MKCALENDAR)   \
  XX(31, LINK, LINK)               \
  XX(32, UNLINK, UNLINK)           \
  XX(33, SOURCE, SOURCE)

namespace klib {

/**
 *
 */
enum class HttpStatus {
#define XX(num, name, string) HTTP_STATUS_##name = num,
  HTTP_STATUS(XX)
#undef XX
};

/**
 *
 * @param http_status
 * @return
 */
constexpr const char* http_status_str(HttpStatus http_status) {
  switch (http_status) {
#define XX(num, name, string)          \
  case HttpStatus::HTTP_STATUS_##name: \
    return #string;
    HTTP_STATUS(XX)
#undef XX
    default:
      return "<unknown>";
  }
}

/**
 *
 */
enum class HttpMethod {
#define XX(num, name, string) HTTP_METHOD_##name = num,
  HTTP_METHOD(XX)
#undef XX
};

/**
 *
 * @param http_method
 * @return
 */
constexpr const char* http_method_str(HttpMethod http_method) {
  switch (http_method) {
#define XX(num, name, string)          \
  case HttpMethod::HTTP_METHOD_##name: \
    return #string;
    HTTP_METHOD(XX)
#undef XX
    default:
      return "<unknown>";
  }
}

/**
 * @brief Parse URL
 */
class URL {
 public:
  URL() = default;
  /**
   * @brief Constructor
   * @param url: URL to be parsed
   */
  explicit URL(std::string url);

  /**
   * @brief Get parts of the URL
   * @return parts of the URL
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view schema() const { return schema_; }
  [[nodiscard]] std::string_view host() const { return host_; }
  [[nodiscard]] std::int32_t port() const { return port_; }
  [[nodiscard]] std::string_view path() const { return path_; }
  [[nodiscard]] std::string_view query() const { return query_; }
  [[nodiscard]] std::string_view fragment() const { return fragment_; }
  [[nodiscard]] std::string_view user_info() const { return user_info_; }

  /**
   * @brief Parse query parameters
   * @return Query key-value pairs
   */
  [[nodiscard]] std::unordered_map<std::string, std::string> query_map() const;

 private:
  std::string url_;

  std::string_view schema_;
  std::string_view host_;
  std::int32_t port_ = 0;
  std::string_view path_;
  std::string_view query_;
  std::string_view fragment_;
  std::string_view user_info_;
};

/**
 * @brief HTTP header
 * @see https://developer.mozilla.org/zh-CN/docs/Glossary/HTTP_header
 * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Messages
 */
class HTTPHeader {
 public:
  /**
   * @brief Constructor
   * @param header: HTTP header to be parsed
   */
  explicit HTTPHeader(std::string header);

  /**
   * @brief Get parts of the header
   * @return parts of the header
   */
  [[nodiscard]] HttpStatus status() const { return status_; }
  [[nodiscard]] HttpMethod method() const { return method_; }
  [[nodiscard]] std::int32_t http_major() const { return http_major_; }
  [[nodiscard]] std::int32_t http_minor() const { return http_minor_; }
  [[nodiscard]] const URL& url() const { return url_; }
  [[nodiscard]] const std::string& body() const { return body_; }

  /**
   * @brief Query the value corresponding to the field
   * @param field: Field name
   * @return Corresponding value
   */
  [[nodiscard]] const std::string& value(const std::string& field) const;

  /**
   * @brief Check if this field is included
   * @param field: Field name
   * @return Return true if exists
   */
  [[nodiscard]] bool contains(const std::string& field) const;

 private:
  std::string header_;

  HttpStatus status_;
  HttpMethod method_;
  std::int32_t http_major_;
  std::int32_t http_minor_;

  URL url_;
  std::unordered_map<std::string, std::string> field_value_;
  std::string body_;
};

}  // namespace klib
