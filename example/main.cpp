#include <cassert>
#include <iostream>

#include <klib/http.h>

int main() {
  klib::Request request;
  request.set_no_proxy();

  auto response = request.get("https://www.baidu.com");
  assert(response.status_code() == klib::Response::StatusCode::Ok);

  std::cout << response.text() << '\n';
}
