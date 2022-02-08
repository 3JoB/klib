#include "klib/markdown.h"

#include <cmark.h>
#include <scope_guard.hpp>

namespace klib {

std::string markdown_to_html(const std::string& markdown) {
  auto ptr = cmark_markdown_to_html(markdown.c_str(), std::size(markdown),
                                    CMARK_OPT_DEFAULT);
  SCOPE_EXIT { std::free(ptr); };
  return ptr;
}

}  // namespace klib
