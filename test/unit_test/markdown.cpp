#include <catch2/catch_test_macros.hpp>

#include "klib/markdown.h"

TEST_CASE("markdown_to_html", "[markdown]") {
  CHECK(klib::markdown_to_html(R"(
## title
111
222

你好世界

![Tux, the Linux mascot](/assets/images/tux.png "title")
)") == R"(<h2>title</h2>
<p>111
222</p>
<p>你好世界</p>
<p><img src="/assets/images/tux.png" alt="Tux, the Linux mascot" title="title" /></p>
)");
}
