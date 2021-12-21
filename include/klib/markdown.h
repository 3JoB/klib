/**
 * @file markdown.h
 * @brief Contains Markdown module
 */

#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <vector>

namespace klib {

/**
 * @brief Convert from CommonMark Markdown to HTML,
 * @param markdown: CommonMark Markdown to be converted
 * @return Converted HTML
 */
std::string markdown_to_html(const std::string &markdown);

/*
 * # heading
 */
struct Heading {
  std::string heading_;
  std::int32_t level_;
};

/*
 * ~~~
 *
 * paragraph
 *
 * ~~~
 */
struct Paragraph {
  std::vector<std::string> content_;
};

/*
 * ![text](/url "title")
 */
struct Image {
  std::string text_;
  std::string url_;
  std::string title_;
};

class MarkdownParser;

/**
 * @brief Represents a Markdown node
 */
class MarkdownNode {
  friend class MarkdownParser;

 public:
  MarkdownNode(const MarkdownNode &) = delete;
  MarkdownNode(MarkdownNode &&) = delete;
  MarkdownNode &operator=(const MarkdownNode &) = delete;
  MarkdownNode &operator=(MarkdownNode &&) = delete;

  /**
   * @brief Destructor
   */
  ~MarkdownNode();

  /**
   * @brief Convert the node to HTML
   * @return Converted HTML
   */
  [[nodiscard]] std::string to_html() const;

  /**
   * @brief Determine whether the node is of heading
   * @return Returns true if it is of heading
   */
  [[nodiscard]] bool is_heading() const;

  /**
   * @brief Determine whether the node is of paragraph
   * @return Returns true if it is of paragraph
   */
  [[nodiscard]] bool is_paragraph() const;

  /**
   * @brief Determine whether the node is of image
   * @return Returns true if it is of image
   */
  [[nodiscard]] bool is_image() const;

  /**
   * @brief Return the Heading value of the node
   * @return Heading value
   */
  [[nodiscard]] Heading as_heading() const;

  /**
   * @brief Return the Paragraph value of the node
   * @return Paragraph value
   */
  [[nodiscard]] Paragraph as_paragraph() const;

  /**
   * @brief Return the Image value of the node
   * @return Image value
   */
  [[nodiscard]] Image as_image() const;

 private:
  explicit MarkdownNode(const MarkdownParser &markdown);

  class MarkdownNodeImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownNodeImpl>> impl_;
};

/**
 * @brief Parsing Markdown
 */
class MarkdownParser {
  friend class MarkdownNode::MarkdownNodeImpl;

 public:
  /**
   * @brief Constructor
   * @param markdown: Markdown to be parsed
   */
  explicit MarkdownParser(const std::string &markdown);

  MarkdownParser(const MarkdownParser &) = delete;
  MarkdownParser(MarkdownParser &&) = delete;
  MarkdownParser &operator=(const MarkdownParser &) = delete;
  MarkdownParser &operator=(MarkdownParser &&) = delete;

  /**
   * @brief Destructor
   */
  ~MarkdownParser();

  /**
   * @brief Determine if there is a next node
   * @return Return true if there is a next node
   */
  [[nodiscard]] bool has_next() const;

  /**
   * @brief Return to the next node
   * @return The next node
   */
  [[nodiscard]] MarkdownNode next();

 private:
  class MarkdownParserImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownParserImpl>> impl_;
};

/**
 * @brief Building Markdown
 */
class MarkdownBuilder {
 public:
  /**
   * @brief Constructor
   */
  MarkdownBuilder();

  MarkdownBuilder(const MarkdownBuilder &) = delete;
  MarkdownBuilder(MarkdownBuilder &&) = delete;
  MarkdownBuilder &operator=(const MarkdownBuilder &) = delete;
  MarkdownBuilder &operator=(MarkdownBuilder &&) = delete;

  /**
   * @brief Destructor
   */
  ~MarkdownBuilder();

  /**
   * @brief Add a heading
   * @param heading: Heading to be added
   */
  void add_heading(const Heading &heading);

  /**
   * @brief Add a paragraph
   * @param paragraph: Paragraph to be added
   */
  void add_paragraph(const Paragraph &paragraph);

  /**
   * @brief Add a image
   * @param image: Image to be added
   */
  void add_image(const Image &image);

  /**
   * @brief Get the CommonMark Markdown document
   * @return CommonMark Markdown document
   */
  [[nodiscard]] std::string to_string() const;

 private:
  class MarkdownBuilderImpl;
  std::experimental::propagate_const<std::unique_ptr<MarkdownBuilderImpl>>
      impl_;
};

}  // namespace klib
