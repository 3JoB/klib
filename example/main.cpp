#include <iostream>

#include <klib/http.h>

int main() {
  klib::http::Request request;
  request.set_no_proxy();

  auto response = request.get("https://www.baidu.com");

  std::cout << response.text() << '\n';
}
