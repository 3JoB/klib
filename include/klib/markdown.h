#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <vector>

namespace klib {

std::string markdown_to_html(const std::string &markdown);

struct Heading {
  std::string heading_;
  std::int32_t level_;
};

struct Paragraph {
  std::vector<std::string> content_;
};

struct Image {
  std::string text_;
  std::string url_;
  std::string title_;
};

class Markdown;

class Item {
  friend class Markdown;

 public:
  Item(const Item &) = delete;
  Item(Item &&) = delete;
  Item &operator=(const Item &) = delete;
  Item &operator=(Item &&) = delete;

  ~Item();

  [[nodiscard]] std::string to_html() const;

  [[nodiscard]] bool is_heading() const;
  [[nodiscard]] bool is_paragraph() const;
  [[nodiscard]] bool is_image() const;

  [[nodiscard]] Heading as_heading() const;
  [[nodiscard]] Paragraph as_paragraph() const;
  [[nodiscard]] Image as_image() const;

 private:
  explicit Item(const Markdown &markdown);

  class ItemImpl;
  std::experimental::propagate_const<std::unique_ptr<ItemImpl>> impl_;
};

class Markdown {
  friend class Item::ItemImpl;

 public:
  explicit Markdown(const std::string &mark_down);

  Markdown(const Markdown &) = delete;
  Markdown(Markdown &&) = delete;
  Markdown &operator=(const Markdown &) = delete;
  Markdown &operator=(Markdown &&) = delete;

  ~Markdown();

  [[nodiscard]] bool has_next() const;
  [[nodiscard]] Item next();

 private:
  class MarkdownImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownImpl>> impl_;
};

}  // namespace klib
