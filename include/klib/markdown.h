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

class MarkdownParser;

class MarkdownElement {
  friend class MarkdownParser;

 public:
  MarkdownElement(const MarkdownElement &) = delete;
  MarkdownElement(MarkdownElement &&) = delete;
  MarkdownElement &operator=(const MarkdownElement &) = delete;
  MarkdownElement &operator=(MarkdownElement &&) = delete;

  ~MarkdownElement();

  [[nodiscard]] std::string to_html() const;

  [[nodiscard]] bool is_heading() const;
  [[nodiscard]] bool is_paragraph() const;
  [[nodiscard]] bool is_image() const;

  [[nodiscard]] Heading as_heading() const;
  [[nodiscard]] Paragraph as_paragraph() const;
  [[nodiscard]] Image as_image() const;

 private:
  explicit MarkdownElement(const MarkdownParser &markdown);

  class MarkdownElementImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownElementImpl>>
      impl_;
};

class MarkdownParser {
  friend class MarkdownElement::MarkdownElementImpl;

 public:
  explicit MarkdownParser(const std::string &mark_down);

  MarkdownParser(const MarkdownParser &) = delete;
  MarkdownParser(MarkdownParser &&) = delete;
  MarkdownParser &operator=(const MarkdownParser &) = delete;
  MarkdownParser &operator=(MarkdownParser &&) = delete;

  ~MarkdownParser();

  [[nodiscard]] bool has_next() const;
  [[nodiscard]] MarkdownElement next();

 private:
  class MarkdownParserImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownParserImpl>> impl_;
};

class MarkdownBuilder {
 public:
  MarkdownBuilder();

  MarkdownBuilder(const MarkdownBuilder &) = delete;
  MarkdownBuilder(MarkdownBuilder &&) = delete;
  MarkdownBuilder &operator=(const MarkdownBuilder &) = delete;
  MarkdownBuilder &operator=(MarkdownBuilder &&) = delete;

  ~MarkdownBuilder();

  void add_heading(const Heading &heading);
  void add_paragraph(const Paragraph &paragraph);
  void add_image(const Image &image);

  [[nodiscard]] std::string to_string() const;

 private:
  class MarkdownBuilderImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownBuilderImpl>>
      impl_;
};

}  // namespace klib
