#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/markdown.h"

TEST_CASE("markdown_to_html", "[markdown]") {
  REQUIRE(klib::markdown_to_html(R"(
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

TEST_CASE("markdown", "[markdown]") {
  klib::Markdown markdown(R"(
## title
111
222

你好世界

![Tux, the Linux mascot](/assets/images/tux.png "title")
)");

  auto item = markdown.next();
  REQUIRE(item.is_heading());
  REQUIRE(item.to_html() == "<h2>title</h2>\n");
  auto heading = item.as_heading();
  REQUIRE(heading.level_ == 2);
  REQUIRE(heading.heading_ == "title");

  auto item2 = markdown.next();
  REQUIRE(item2.is_paragraph());
  REQUIRE(item2.to_html() == "<p>111\n222</p>\n");
  auto paragraph = item2.as_paragraph();
  REQUIRE(paragraph.content_ == std::vector<std::string>{"111", "222"});

  auto item3 = markdown.next();
  REQUIRE(item3.is_paragraph());
  REQUIRE(item3.to_html() == "<p>你好世界</p>\n");
  paragraph = item3.as_paragraph();
  REQUIRE(paragraph.content_ == std::vector<std::string>{"你好世界"});

  auto item4 = markdown.next();
  REQUIRE(item4.is_image());
  REQUIRE(item4.to_html() ==
          "<p><img src=\"/assets/images/tux.png\" alt=\"Tux, the Linux "
          "mascot\" title=\"title\" /></p>\n");
  auto image = item4.as_image();
  REQUIRE(image.text_ == "Tux, the Linux mascot");
  REQUIRE(image.url_ == "/assets/images/tux.png");
  REQUIRE(image.title_ == "title");

  REQUIRE_FALSE(markdown.has_next());
}
