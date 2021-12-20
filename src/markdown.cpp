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

class Item::ItemImpl {
 public:
  explicit ItemImpl(const Markdown& markdown);

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

class Markdown::MarkdownImpl {
  friend class Item::ItemImpl;

 public:
  explicit MarkdownImpl(const std::string& markdown);
  ~MarkdownImpl();

  [[nodiscard]] bool has_next() const;
  [[nodiscard]] Item next(Markdown& markdown);

 private:
  cmark_node* doc_;
  cmark_node* curr_node_ = nullptr;
};

Item::ItemImpl::ItemImpl(const Markdown& markdown)
    : node_(markdown.impl_->curr_node_) {}

std::string Item::ItemImpl::to_html() const {
  auto ptr = cmark_render_html(node_, CMARK_OPT_DEFAULT);
  SCOPE_EXIT { std::free(ptr); };
  return ptr;
}

bool Item::ItemImpl::is_heading() const {
  return cmark_node_get_type(node_) == CMARK_NODE_HEADING;
}

bool Item::ItemImpl::is_paragraph() const {
  return cmark_node_get_type(node_) == CMARK_NODE_PARAGRAPH;
}

bool Item::ItemImpl::is_image() const {
  return cmark_node_get_type(node_) == CMARK_NODE_PARAGRAPH &&
         cmark_node_get_type(cmark_node_first_child(node_)) == CMARK_NODE_IMAGE;
}

Heading Item::ItemImpl::as_heading() const {
  Expects(is_heading());

  auto level = cmark_node_get_heading_level(node_);

  auto child = cmark_node_first_child(node_);
  Ensures(cmark_node_get_type(child) == CMARK_NODE_TEXT);
  auto heading = cmark_node_get_literal(child);

  return {heading, level};
}

Paragraph Item::ItemImpl::as_paragraph() const {
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

Image Item::ItemImpl::as_image() const {
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

Markdown::MarkdownImpl::MarkdownImpl(const std::string& markdown)
    : doc_(cmark_parse_document(markdown.c_str(), std::size(markdown),
                                CMARK_OPT_DEFAULT)) {
  if (!doc_) {
    throw RuntimeError("cmark_parse_document failed");
  }
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

std::string Item::to_html() const { return impl_->to_html(); }

bool Item::is_heading() const { return impl_->is_heading(); }

bool Item::is_paragraph() const { return impl_->is_paragraph(); }

bool Item::is_image() const { return impl_->is_image(); }

Heading Item::as_heading() const { return impl_->as_heading(); }

Paragraph Item::as_paragraph() const { return impl_->as_paragraph(); }

Image Item::as_image() const { return impl_->as_image(); }

Item::Item(const Markdown& markdown)
    : impl_(std::make_unique<ItemImpl>(markdown)) {}

Markdown::Markdown(const std::string& markdown)
    : impl_(std::make_unique<MarkdownImpl>(markdown)) {}

Markdown::~Markdown() = default;

bool Markdown::has_next() const { return impl_->has_next(); }

Item Markdown::next() { return impl_->next(*this); }

}  // namespace klib
