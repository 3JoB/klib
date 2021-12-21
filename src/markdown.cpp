#include "klib/markdown.h"

#include <cstdlib>

#include <cmark.h>
#include <gsl/gsl-lite.hpp>
#include <scope_guard.hpp>

#include "klib/exception.h"

namespace klib {

std::string markdown_to_html(const std::string& markdown) {
  auto ptr = cmark_markdown_to_html(markdown.c_str(), std::size(markdown),
                                    CMARK_OPT_DEFAULT);
  SCOPE_EXIT { std::free(ptr); };
  return ptr;
}

class MarkdownNode::MarkdownNodeImpl {
 public:
  explicit MarkdownNodeImpl(const MarkdownParser& markdown);

  [[nodiscard]] std::string to_html() const;

  [[nodiscard]] bool is_heading() const;
  [[nodiscard]] bool is_paragraph() const;
  [[nodiscard]] bool is_image() const;

  [[nodiscard]] Heading as_heading() const;
  [[nodiscard]] Paragraph as_paragraph() const;
  [[nodiscard]] Image as_image() const;

 private:
  cmark_node* node_;
};

class MarkdownParser::MarkdownParserImpl {
  friend class MarkdownNode::MarkdownNodeImpl;

 public:
  explicit MarkdownParserImpl(const std::string& markdown);
  ~MarkdownParserImpl();

  [[nodiscard]] bool has_next() const;
  [[nodiscard]] MarkdownNode next(MarkdownParser& markdown);

 private:
  cmark_node* doc_;
  cmark_node* curr_node_ = nullptr;
};

class MarkdownBuilder::MarkdownBuilderImpl {
 public:
  MarkdownBuilderImpl();
  ~MarkdownBuilderImpl();

  void add_heading(const Heading& heading);
  void add_paragraph(const Paragraph& paragraph);
  void add_image(const Image& image);

  [[nodiscard]] std::string to_string() const;

 private:
  cmark_node* doc_;
};

MarkdownNode::MarkdownNodeImpl::MarkdownNodeImpl(const MarkdownParser& markdown)
    : node_(markdown.impl_->curr_node_) {}

std::string MarkdownNode::MarkdownNodeImpl::to_html() const {
  auto ptr = cmark_render_html(node_, CMARK_OPT_DEFAULT);
  SCOPE_EXIT { std::free(ptr); };
  return ptr;
}

bool MarkdownNode::MarkdownNodeImpl::is_heading() const {
  return cmark_node_get_type(node_) == CMARK_NODE_HEADING;
}

bool MarkdownNode::MarkdownNodeImpl::is_paragraph() const {
  return cmark_node_get_type(node_) == CMARK_NODE_PARAGRAPH;
}

bool MarkdownNode::MarkdownNodeImpl::is_image() const {
  return cmark_node_get_type(node_) == CMARK_NODE_PARAGRAPH &&
         cmark_node_get_type(cmark_node_first_child(node_)) == CMARK_NODE_IMAGE;
}

Heading MarkdownNode::MarkdownNodeImpl::as_heading() const {
  Expects(is_heading());

  auto level = cmark_node_get_heading_level(node_);

  auto child = cmark_node_first_child(node_);
  Ensures(cmark_node_get_type(child) == CMARK_NODE_TEXT);
  auto heading = cmark_node_get_literal(child);

  return {heading, level};
}

Paragraph MarkdownNode::MarkdownNodeImpl::as_paragraph() const {
  Expects(is_paragraph());

  std::vector<std::string> content;

  auto child = cmark_node_first_child(node_);
  do {
    if (cmark_node_get_type(child) == CMARK_NODE_SOFTBREAK) {
      // do nothing
    } else {
      Ensures(cmark_node_get_type(child) == CMARK_NODE_TEXT);
      content.emplace_back(cmark_node_get_literal(child));
    }

    child = cmark_node_next(child);
  } while (child);

  return {content};
}

Image MarkdownNode::MarkdownNodeImpl::as_image() const {
  // node_'s type is paragraph
  Expects(is_paragraph());
  auto image = cmark_node_first_child(node_);
  Ensures(cmark_node_get_type(image) == CMARK_NODE_IMAGE);
  auto url = cmark_node_get_url(image);
  auto title = cmark_node_get_title(image);

  auto child = cmark_node_first_child(image);
  Ensures(cmark_node_get_type(child) == CMARK_NODE_TEXT);
  auto text = cmark_node_get_literal(child);

  return {text, url, title};
}

MarkdownParser::MarkdownParserImpl::MarkdownParserImpl(
    const std::string& markdown)
    : doc_(cmark_parse_document(markdown.c_str(), std::size(markdown),
                                CMARK_OPT_DEFAULT)) {
  if (!doc_) {
    throw RuntimeError("cmark_parse_document failed");
  }
}

MarkdownParser::MarkdownParserImpl::~MarkdownParserImpl() {
  cmark_node_free(doc_);
}

bool MarkdownParser::MarkdownParserImpl::has_next() const {
  return cmark_node_next(curr_node_) != nullptr;
}

MarkdownNode MarkdownParser::MarkdownParserImpl::next(
    MarkdownParser& markdown) {
  if (curr_node_) {
    curr_node_ = cmark_node_next(curr_node_);
  } else {
    curr_node_ = cmark_node_first_child(doc_);
  }

  return MarkdownNode(markdown);
}

MarkdownBuilder::MarkdownBuilderImpl::MarkdownBuilderImpl()
    : doc_(cmark_node_new(CMARK_NODE_DOCUMENT)) {
  if (!doc_) {
    throw RuntimeError("cmark_node_new failed");
  }
}

MarkdownBuilder::MarkdownBuilderImpl::~MarkdownBuilderImpl() {
  cmark_node_free(doc_);
}

void MarkdownBuilder::MarkdownBuilderImpl::add_heading(const Heading& heading) {
  auto heading_node = cmark_node_new(CMARK_NODE_HEADING);
  cmark_node_set_heading_level(heading_node, heading.level_);

  auto text_node = cmark_node_new(CMARK_NODE_TEXT);
  cmark_node_set_literal(text_node, heading.heading_.c_str());

  cmark_node_append_child(heading_node, text_node);
  cmark_node_append_child(doc_, heading_node);
}

void MarkdownBuilder::MarkdownBuilderImpl::add_paragraph(
    const Paragraph& paragraph) {
  auto paragraph_node = cmark_node_new(CMARK_NODE_PARAGRAPH);

  for (const auto& item : paragraph.content_) {
    auto text_node = cmark_node_new(CMARK_NODE_TEXT);
    cmark_node_set_literal(text_node, item.c_str());

    auto soft_break_node = cmark_node_new(CMARK_NODE_SOFTBREAK);

    cmark_node_append_child(paragraph_node, text_node);
    cmark_node_append_child(paragraph_node, soft_break_node);
  }

  cmark_node_append_child(doc_, paragraph_node);
}

void MarkdownBuilder::MarkdownBuilderImpl::add_image(const Image& image) {
  auto paragraph_node = cmark_node_new(CMARK_NODE_PARAGRAPH);
  auto image_node = cmark_node_new(CMARK_NODE_IMAGE);
  auto text_node = cmark_node_new(CMARK_NODE_TEXT);

  cmark_node_set_url(image_node, image.url_.c_str());
  cmark_node_set_title(image_node, image.title_.c_str());
  cmark_node_set_literal(text_node, image.text_.c_str());

  cmark_node_append_child(image_node, text_node);
  cmark_node_append_child(paragraph_node, image_node);
  cmark_node_append_child(doc_, paragraph_node);
}

std::string MarkdownBuilder::MarkdownBuilderImpl::to_string() const {
  auto ptr = cmark_render_commonmark(doc_, CMARK_OPT_DEFAULT, 0);
  SCOPE_EXIT { std::free(ptr); };
  return ptr;
}

MarkdownNode::~MarkdownNode() = default;

std::string MarkdownNode::to_html() const { return impl_->to_html(); }

bool MarkdownNode::is_heading() const { return impl_->is_heading(); }

bool MarkdownNode::is_paragraph() const { return impl_->is_paragraph(); }

bool MarkdownNode::is_image() const { return impl_->is_image(); }

Heading MarkdownNode::as_heading() const { return impl_->as_heading(); }

Paragraph MarkdownNode::as_paragraph() const { return impl_->as_paragraph(); }

Image MarkdownNode::as_image() const { return impl_->as_image(); }

MarkdownNode::MarkdownNode(const MarkdownParser& markdown)
    : impl_(std::make_unique<MarkdownNodeImpl>(markdown)) {}

MarkdownParser::MarkdownParser(const std::string& markdown)
    : impl_(std::make_unique<MarkdownParserImpl>(markdown)) {}

MarkdownParser::~MarkdownParser() = default;

bool MarkdownParser::has_next() const { return impl_->has_next(); }

MarkdownNode MarkdownParser::next() { return impl_->next(*this); }

MarkdownBuilder::MarkdownBuilder()
    : impl_(std::make_unique<MarkdownBuilderImpl>()) {}

MarkdownBuilder::~MarkdownBuilder() = default;

void MarkdownBuilder::add_heading(const Heading& heading) {
  impl_->add_heading(heading);
}

void MarkdownBuilder::add_paragraph(const Paragraph& paragraph) {
  impl_->add_paragraph(paragraph);
}

void MarkdownBuilder::add_image(const Image& image) { impl_->add_image(image); }

std::string MarkdownBuilder::to_string() const { return impl_->to_string(); }

}  // namespace klib
