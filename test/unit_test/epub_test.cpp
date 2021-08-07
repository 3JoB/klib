#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/epub.h"
#include "klib/util.h"

TEST_CASE("generate") {
  klib::epub::Epub epub;
  epub.set_book_name("test");
  epub.generate();

  REQUIRE(std::filesystem::exists("test/META-INF/container.xml"));
  REQUIRE(klib::util::read_file("test/META-INF/container.xml", false) ==
          R"(<?xml version="1.0" encoding="UTF-8"?>
<container version="1.0" xmlns="urn:oasis:names:tc:opendocument:xmlns:container">
    <rootfiles>
        <rootfile full-path="OEBPS/content.opf" media-type="application/oebps-package+xml" />
    </rootfiles>
</container>
)");

  REQUIRE(std::filesystem::exists("test/mimetype"));
  REQUIRE(klib::util::read_file("test/mimetype", false) ==
          "application/epub+zip");

  REQUIRE(std::filesystem::exists("test/OEBPS/content.opf"));
  REQUIRE(klib::util::read_file("test/OEBPS/content.opf", false) ==
          "application/epub+zip");

  std::filesystem::remove_all("test");
}
