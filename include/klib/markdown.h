#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace klib {

struct Heading {
  std::string title_;
  std::int32_t level_;
};

struct Paragraph {
  std::vector<std::string> content_;
};

class Item {
 public:
 private:
};

class Markdown {
 public:
  explicit Markdown(const std::string &mark_down);

 private:
};

}  // namespace klib
