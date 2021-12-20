#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/markdown.h"

TEST_CASE("markdown", "[markdown]") {
  klib::Markdown markdown(R"(
## title
111
222

你好世界

![Tux, the Linux mascot](/assets/images/tux.png "title")
)");

  auto heading = markdown.next().as_heading();
  REQUIRE(heading.level_ == 2);
  REQUIRE(heading.heading_ == "title");

  auto paragraph = markdown.next().as_paragraph();
  REQUIRE(paragraph.content_ == std::vector<std::string>{"111", "222"});

  paragraph = markdown.next().as_paragraph();
  REQUIRE(paragraph.content_ == std::vector<std::string>{"你好世界"});

  auto image = markdown.next().as_image();
  REQUIRE(image.text_ == "Tux, the Linux mascot");
  REQUIRE(image.url_ == "/assets/images/tux.png");
  REQUIRE(image.title_ == "title");

  REQUIRE_FALSE(markdown.has_next());
}
