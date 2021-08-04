#include <iostream>

#include <klib/http.h>
#include <klib/util.h>

int main() {
  klib::http::Request request;
  request.verbose(true);

#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy();
#endif

  auto response =
      request.get("https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");
  response.save_to_file("8.0.1.zip", true);

  std::cout << klib::util::sha3_512("8.0.1.zip") << '\n';
}
