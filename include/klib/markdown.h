#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <vector>

namespace klib {

struct Heading {
  std::string heading_;
  std::int32_t level_;
};

struct Paragraph {
  std::vector<std::string> content_;
};

class Markdown;

class Item {
  friend class Markdown;

 public:
  [[nodiscard]] bool is_heading() const;
  [[nodiscard]] bool is_paragraph() const;

  [[nodiscard]] Heading as_heading() const;
  [[nodiscard]] Paragraph as_paragraph() const;

 private:
  explicit Item(Markdown &markdown);

  class ItemImpl;
  std::experimental::propagate_const<std::unique_ptr<ItemImpl>> impl_;
};

class Markdown {
  friend class Item::ItemImpl;

 public:
  explicit Markdown(const std::string &mark_down);

  [[nodiscard]] bool has_next() const;
  [[nodiscard]] Item next();

 private:
  class MarkdownImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownImpl>> impl_;
};

}  // namespace klib
