#include <iostream>

#include <klib/hash_lib.h>
#include <klib/http.h>
#include <klib/util.h>

int main() {
  klib::Request request;
  request.verbose(true);

  auto response =
      request.get("https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");
  response.save_to_file("8.0.1.zip", true);

  std::cout << klib::sha3_512_hex(klib::read_file("8.0.1.zip", true)) << '\n';
}
