#include <cstdint>
#include <iostream>

#include <klib/add.h>
#include <klib/bar.h>
#include <klib/vec.h>

int main() {
  std::cout << klib::add(1, 2) << '\n';
  std::cout << klib::Bar(42).get_val() << '\n';
  std::cout << *klib::Vec<std::int32_t>{42}.begin() << '\n';
}
