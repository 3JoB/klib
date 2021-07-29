#include <fstream>
#include <iostream>

#include "klib/http.h"

int main() {
  klib::http::Request request;
  request.verbose(true);
  request.allow_redirects(true);
  request.set_proxy("socks5://127.0.0.1:1080");

  auto response =
      request.get("https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");

  response.save("8.0.1.zip");
}
