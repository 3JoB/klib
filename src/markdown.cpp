#include "klib/markdown.h"

#include <cmark.h>

namespace klib {

class Item::ItemImpl {
 public:
  explicit ItemImpl(Markdown& markdown);

  [[nodiscard]] bool is_heading() const;
  [[nodiscard]] bool is_paragraph() const;

  [[nodiscard]] Heading as_heading() const;
  [[nodiscard]] Paragraph as_paragraph() const;

 private:
  cmark_node* node_ = nullptr;
};

class Markdown::MarkdownImpl {
  friend class Item::ItemImpl;

 public:
  explicit MarkdownImpl(const std::string& markdown);
  ~MarkdownImpl();

  [[nodiscard]] bool has_next() const;
  [[nodiscard]] Item next(Markdown& markdown);

 private:
  cmark_node* doc_ = nullptr;
  cmark_node* curr_node_ = nullptr;
};

Item::ItemImpl::ItemImpl(Markdown& markdown) {
  node_ = markdown.impl_->curr_node_;
}

bool Item::ItemImpl::is_heading() const {
  return cmark_node_get_type(node_) == CMARK_NODE_HEADING;
}

bool Item::ItemImpl::is_paragraph() const {
  return cmark_node_get_type(node_) == CMARK_NODE_PARAGRAPH;
}

Heading Item::ItemImpl::as_heading() const {
  auto level = cmark_node_get_heading_level(node_);

  auto child = cmark_node_first_child(node_);
  auto heading = cmark_node_get_literal(child);

  return {heading, level};
}

Paragraph Item::ItemImpl::as_paragraph() const {
  std::vector<std::string> content;

  auto child = cmark_node_first_child(node_);
  do {
    if (cmark_node_get_type(child) == CMARK_NODE_SOFTBREAK) {
      // do nothing
    } else {
      content.emplace_back(cmark_node_get_literal(child));
    }

    child = cmark_node_next(child);
  } while (child);

  return {content};
}

Markdown::MarkdownImpl::MarkdownImpl(const std::string& markdown) {
  doc_ = cmark_parse_document(markdown.c_str(), std::size(markdown),
                              CMARK_OPT_DEFAULT);
}

Markdown::MarkdownImpl::~MarkdownImpl() { cmark_node_free(doc_); }

bool Markdown::MarkdownImpl::has_next() const {
  return cmark_node_next(curr_node_) != nullptr;
}

Item Markdown::MarkdownImpl::next(Markdown& markdown) {
  if (curr_node_) {
    curr_node_ = cmark_node_next(curr_node_);
  } else {
    curr_node_ = cmark_node_first_child(doc_);
  }

  return Item(markdown);
}

Item::~Item() = default;

bool Item::is_heading() const { return impl_->is_heading(); }

bool Item::is_paragraph() const { return impl_->is_paragraph(); }

Heading Item::as_heading() const { return impl_->as_heading(); }

Paragraph Item::as_paragraph() const { return impl_->as_paragraph(); }

Item::Item(Markdown& markdown) : impl_(std::make_unique<ItemImpl>(markdown)) {}

Markdown::Markdown(const std::string& markdown)
    : impl_(std::make_unique<MarkdownImpl>(markdown)) {}

Markdown::~Markdown() = default;

bool Markdown::has_next() const { return impl_->has_next(); }

Item Markdown::next() { return impl_->next(*this); }

}  // namespace klib
