#include <cassert>
#include <iostream>

#include <klib/exception.h>
#include <klib/http.h>
#include <klib/log.h>

int main() try {
  klib::Request request;
  request.set_no_proxy();

  auto response = request.get("https://www.baidu.com");
  assert(response.ok());

  std::cout << response.text() << '\n';

  throw klib::RuntimeError("error");
} catch (const klib::RuntimeError &err) {
  klib::warn(err.what());
}
