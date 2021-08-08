#include "klib/epub.h"

#include <cassert>
#include <cstddef>
#include <ctime>
#include <filesystem>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <pugixml.hpp>

#include "klib/exception.h"
#include "klib/util.h"
#include "klib/version.h"

extern char style[];
extern int style_size;

namespace klib::epub {

namespace {

std::string num_to_str(std::int32_t i) {
  assert(i > 0);

  auto str = std::to_string(i);
  if (i < 10) {
    return "00" + str;
  } else if (i < 100) {
    return "0" + str;
  } else {
    return str;
  }
}

std::string num_to_chapter_name(std::int32_t i) {
  return "chapter" + num_to_str(i) + ".xhtml";
}

std::string num_to_illustration_name(std::int32_t i) {
  return "illustration" + num_to_str(i) + ".xhtml";
}

std::string get_date() {
  return fmt::format("{:%Y-%m-%d}.", fmt::localtime(std::time(nullptr)));
}

void save_file(const pugi::xml_document &doc, std::string_view path) {
  if (!doc.save_file(path.data(), "    ")) {
    throw klib::RuntimeError("can not save: {}", path);
  }
}

void append_manifest_and_spine(pugi::xml_node &manifest, const std::string &id,
                               const std::string &href) {
  auto item = manifest.append_child("item");
  item.append_attribute("id") = id.c_str();
  item.append_attribute("href") = href.c_str();

  std::string media_type;
  if (href.ends_with("xhtml")) {
    media_type = "application/xhtml+xml";
  } else if (href.ends_with("jpg")) {
    media_type = "image/jpeg";
  } else if (href.ends_with("ncx")) {
    media_type = "application/x-dtbncx+xml";
  } else if (href.ends_with("css")) {
    media_type = "text/css";
  } else if (href.ends_with("otf")) {
    // https://www.oreilly.com/library/view/epub-3-best/9781449329129/ch04.html
    media_type = "application/vnd.ms-opentype";
  } else {
    throw klib::RuntimeError("Unknown media type");
  }
  item.append_attribute("media-type") = media_type.c_str();

  if (href.ends_with("xhtml")) {
    auto package = manifest.parent();
    auto spine = package.child("spine");
    if (spine.empty()) {
      spine = package.append_child("spine");
      spine.append_attribute("toc") = "ncx";
    }

    auto itemref = spine.append_child("itemref");
    itemref.append_attribute("idref") = id.c_str();
  }
}

void append_nav_map(pugi::xml_node &nav_map, const std::string &text,
                    const std::string &src) {
  auto first = nav_map.last_child().attribute("playOrder").as_int() + 1;

  auto nav_point = nav_map.append_child("navPoint");
  nav_point.append_attribute("id") =
      ("navPoint-" + std::to_string(first)).c_str();
  nav_point.append_attribute("playOrder") = std::to_string(first).c_str();

  auto nav_label = nav_point.append_child("navLabel");
  nav_label.append_child("text").text().set(text.c_str());

  nav_point.append_child("content").append_attribute("src") = src.c_str();
}

pugi::xml_document generate_declaration() {
  pugi::xml_document doc;
  auto decl = doc.prepend_child(pugi::node_declaration);
  decl.append_attribute("version") = "1.0";
  decl.append_attribute("encoding") = "UTF-8";

  return doc;
}

pugi::xml_document generate_xhtml(const std::string &title,
                                  const std::string &div_class = "",
                                  bool has_h1 = false) {
  auto doc = generate_declaration();

  doc.append_child(pugi::node_doctype)
      .set_value(
          R"(html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd")");

  auto html = doc.append_child("html");
  html.append_attribute("xmlns") = "http://www.w3.org/1999/xhtml";
  html.append_attribute("xml:lang") = "zh";
  html.append_attribute("xmlns:epub") = "http://www.idpf.org/2007/ops";

  auto head = html.append_child("head");

  auto link = head.append_child("link");
  link.append_attribute("href") = "../Styles/style.css";
  link.append_attribute("rel") = "stylesheet";
  link.append_attribute("type") = "text/css";

  head.append_child("title").text() = title.c_str();

  auto body = html.append_child("body");
  auto div = body.append_child("div");

  if (!std::empty(div_class)) {
    div.append_attribute("class") = div_class.c_str();
  }

  if (has_h1) {
    auto h1 = div.append_child("h1");
    h1.append_attribute("class") = "bold";
    h1.text() = title.c_str();
  }

  return doc;
}

void append_texts(pugi::xml_document &doc,
                  const std::vector<std::string> &texts) {
  auto div = doc.select_node("/html/body/div").node();
  if (div.empty()) {
    throw klib::RuntimeError("no div");
  }

  for (const auto &text : texts) {
    auto p = div.append_child("p");
    p.text() = text.c_str();
  }
}

}  // namespace

Epub::Epub() { style_ = std::string_view(style, style_size); }

void Epub::set_creator(const std::string &creator) { creator_ = creator; }

void Epub::set_book_name(const std::string &book_name) {
  book_name_ = book_name;
}

void Epub::set_author(const std::string &author) { author_ = author; }

void Epub::set_introduction(const std::vector<std::string> &introduction) {
  introduction_ = introduction;
}

void Epub::set_generate_cover(bool generate_cover) {
  generate_cover_ = generate_cover;
}

void Epub::set_generate_postscript(bool generate_postscript) {
  generate_postscript_ = generate_postscript;
}

void Epub::set_illustration_num(std::int32_t illustration_num) {
  illustration_num_ = illustration_num;
}

void Epub::set_image_num(std::int32_t image_num) { image_num_ = image_num; }

void Epub::set_front(const std::string &font) {
  font_ = std::string_view(font.data(), std::size(font));
}

void Epub::set_front(std::string_view font) { font_ = font; }

void Epub::set_uuid(const std::string &uuid) { uuid_ = "urn:uuid:" + uuid; }

void Epub::set_date(const std::string &date) { date_ = date; }

void Epub::add_content(const std::string &title,
                       const std::vector<std::string> &content) {
  content_.emplace_back(title, content);
}

void Epub::generate() {
  if (std::empty(uuid_)) {
    auto uuid = boost::uuids::random_generator()();
    uuid_ = "urn:uuid:" + boost::uuids::to_string(uuid);
  }

  if (std::empty(date_)) {
    date_ = get_date();
  }

  std::filesystem::create_directory(book_name_);
  klib::util::ChangeWorkingDir change_working_dir(book_name_);

  std::filesystem::create_directory(Epub::meta_inf_dir);
  std::filesystem::create_directory(Epub::oebps_dir);
  std::filesystem::create_directory(Epub::fonts_dir);
  if (image_num_ > 0) {
    std::filesystem::create_directory(Epub::images_dir);
  }
  std::filesystem::create_directory(Epub::styles_dir);
  std::filesystem::create_directory(Epub::text_dir);

  generate_container();
  generate_font();
  generate_style();
  generate_chapter();
  generate_cover();
  generate_illustration();
  generate_introduction();
  generate_message();
  generate_postscript();
  generate_content();
  generate_toc();
  generate_mimetype();
}

void Epub::generate_container() const {
  auto doc = generate_declaration();

  auto container = doc.append_child("container");
  container.append_attribute("version") = "1.0";
  container.append_attribute("xmlns") =
      "urn:oasis:names:tc:opendocument:xmlns:container";

  auto rootfile = container.append_child("rootfiles").append_child("rootfile");
  rootfile.append_attribute("full-path") = content_path.data();
  rootfile.append_attribute("media-type") = "application/oebps-package+xml";

  save_file(doc, container_path);
}

void Epub::generate_font() const {
  if (std::empty(font_)) {
    throw klib::RuntimeError("The font is empty");
  }

  klib::util::write_file(Epub::font_path, true, font_);
}

void Epub::generate_style() const {
  if (std::empty(style_)) {
    throw klib::RuntimeError("The style is empty");
  }

  klib::util::write_file(Epub::style_path, false, style_);
}

void Epub::generate_chapter() const {
  auto size = std::size(content_);
  for (std::size_t i = 0; i < size; ++i) {
    auto [title, texts] = content_[i];
    auto doc = generate_xhtml(title, "", true);
    append_texts(doc, texts);

    auto path =
        std::filesystem::path(Epub::text_dir) / num_to_chapter_name(i + 1);
    save_file(doc, path.c_str());
  }
}

void Epub::generate_cover() const {
  auto doc = generate_xhtml("封面", "cover", false);

  auto div = doc.select_node("/html/body/div").node();
  auto img = div.append_child("img");
  img.append_attribute("alt") = "";
  img.append_attribute("src") = "../Images/cover.jpg";

  save_file(doc, Epub::cover_path);
}

void Epub::generate_illustration() const {
  for (std::int32_t i = 1; i <= illustration_num_; ++i) {
    auto num_str = num_to_str(i);
    auto doc = generate_xhtml("彩页" + num_str, "", false);
    auto file_name = num_to_illustration_name(i);

    auto div = doc.select_node("/html/body/div").node();
    div = div.append_child("div");
    div.append_attribute("class") = "center";

    auto img = div.append_child("img");
    img.append_attribute("alt") = num_str.c_str();
    img.append_attribute("src") = ("../Images/" + num_str + ".jpg").c_str();

    auto path = std::filesystem::path(Epub::text_dir) / file_name;
    save_file(doc, path.c_str());
  }
}

void Epub::generate_introduction() const {
  auto doc = generate_xhtml("简介", "", true);
  append_texts(doc, introduction_);
  save_file(doc, Epub::introduction_path);
}

void Epub::generate_message() const {
  auto doc = generate_xhtml("制作信息", "", true);

  auto div = doc.select_node("/html/body/div").node();
  div = div.append_child("div");
  div.append_attribute("class") = "cutline";

  auto p = div.append_child("p");
  p.append_attribute("class") = "makerifm";
  p.text() = ("制作者：" + creator_).c_str();

  save_file(doc, Epub::message_path);
}

void Epub::generate_postscript() const {
  auto doc = generate_xhtml("后记", "", true);
  save_file(doc, Epub::postscript_path);
}

void Epub::generate_content() const {
  auto doc = generate_declaration();

  auto package = doc.append_child("package");
  package.append_attribute("version") = "2.0";
  package.append_attribute("unique-identifier") = "BookId";
  package.append_attribute("xmlns") = "http://www.idpf.org/2007/opf";

  auto metadata = package.append_child("metadata");
  metadata.append_attribute("xmlns:dc") = "http://purl.org/dc/elements/1.1/";
  metadata.append_attribute("xmlns:opf") = "http://www.idpf.org/2007/opf";

  metadata.append_child("dc:title").text() = book_name_.c_str();

  auto dc_creator = metadata.append_child("dc:creator");
  dc_creator.append_attribute("opf:file-as") = creator_.c_str();
  dc_creator.append_attribute("opf:role") = "aut";
  dc_creator.text() = author_.c_str();

  metadata.append_child("dc:language").text() = "zh-CN";
  metadata.append_child("dc:rights").text() = creator_.c_str();

  auto meta = metadata.append_child("meta");
  meta.append_attribute("name") = "klib version";
  meta.append_attribute("content") = klib_version().c_str();

  auto dc_date = metadata.append_child("dc:date");
  dc_date.append_attribute("opf:event") = "modification";
  dc_date.append_attribute("xmlns:opf") = "http://www.idpf.org/2007/opf";
  dc_date.text() = date_.c_str();

  auto dc_identifier = metadata.append_child("dc:identifier");
  dc_identifier.append_attribute("id") = "BookId";
  dc_identifier.append_attribute("opf:scheme") = "UUID";
  dc_identifier.text() = uuid_.c_str();

  if (generate_cover_) {
    meta = metadata.append_child("meta");
    meta.append_attribute("name") = "cover";
    meta.append_attribute("content") = "cover.jpg";
  }

  auto manifest = package.append_child("manifest");
  append_manifest_and_spine(manifest, "ncx", "toc.ncx");
  append_manifest_and_spine(manifest, "style.css", "Styles/style.css");
  append_manifest_and_spine(manifest, "SourceHanSansHWSC-Bold.otf",
                            "Fonts/SourceHanSansHWSC-Bold.otf");

  for (std::int32_t i = 1; i <= image_num_; ++i) {
    append_manifest_and_spine(manifest, "x" + num_to_str(i) + ".jpg",
                              "Images/" + num_to_str(i) + ".jpg");
  }

  if (generate_cover_) {
    append_manifest_and_spine(manifest, "cover.jpg", "Images/cover.jpg");
    append_manifest_and_spine(manifest, "cover.xhtml", "Text/cover.xhtml");
  }

  append_manifest_and_spine(manifest, "message.xhtml", "Text/message.xhtml");
  append_manifest_and_spine(manifest, "introduction.xhtml",
                            "Text/introduction.xhtml");

  for (std::int32_t i = 1; i <= illustration_num_; ++i) {
    auto name = num_to_illustration_name(i);
    append_manifest_and_spine(manifest, name, "Text/" + name);
  }

  auto size = std::size(content_);
  for (std::size_t i = 1; i <= size; ++i) {
    auto name = num_to_chapter_name(i);
    append_manifest_and_spine(manifest, name, "Text/" + name);
  }

  if (generate_postscript_) {
    append_manifest_and_spine(manifest, "postscript.xhtml",
                              "Text/postscript.xhtml");
  }

  auto guide = package.append_child("guide");
  if (generate_cover_) {
    auto reference = guide.append_child("reference");
    reference.append_attribute("type") = "cover";
    reference.append_attribute("title") = "Cover";
    reference.append_attribute("href") = "Text/cover.xhtml";
  }

  save_file(doc, Epub::content_path);
}

void Epub::generate_toc() const {
  auto doc = generate_declaration();

  doc.append_child(pugi::node_doctype)
      .set_value(
          R"(ncx PUBLIC "-//NISO//DTD ncx 2005-1//EN" "http://www.daisy.org/z3986/2005/ncx-2005-1.dtd")");

  auto ncx = doc.append_child("ncx");
  ncx.append_attribute("version") = "2005-1";
  ncx.append_attribute("xmlns") = "http://www.daisy.org/z3986/2005/ncx/";

  auto head = ncx.append_child("head");

  auto meta = head.append_child("meta");
  meta.append_attribute("name") = "dtb:uid";
  meta.append_attribute("content") = uuid_.c_str();

  meta = head.append_child("meta");
  meta.append_attribute("name") = "dtb:depth";
  meta.append_attribute("content") = 1;

  meta = head.append_child("meta");
  meta.append_attribute("name") = "dtb:totalPageCount";
  meta.append_attribute("content") = 0;

  meta = head.append_child("meta");
  meta.append_attribute("name") = "dtb:maxPageNumber";
  meta.append_attribute("content") = 0;

  auto doc_title = ncx.append_child("docTitle");
  doc_title.append_child("text").text() = book_name_.c_str();

  auto doc_author = ncx.append_child("docAuthor");
  doc_author.append_child("text").text() = author_.c_str();

  auto nav_map = ncx.append_child("navMap");
  if (generate_cover_) {
    append_nav_map(nav_map, "封面", "Text/cover.xhtml");
  }
  append_nav_map(nav_map, "制作信息", "Text/message.xhtml");
  append_nav_map(nav_map, "简介", "Text/introduction.xhtml");

  if (illustration_num_ > 0) {
    append_nav_map(nav_map, "彩页", "Text/illustration001.xhtml");
  }

  auto size = std::size(content_);
  for (std::size_t i = 0; i < size; ++i) {
    append_nav_map(nav_map, content_[i].first,
                   "Text/" + num_to_chapter_name(i + 1));
  }

  if (generate_postscript_) {
    append_nav_map(nav_map, "后记", "Text/postscript.xhtml");
  }

  save_file(doc, Epub::toc_path);
}

void Epub::generate_mimetype() const {
  std::string text = "application/epub+zip\n";
  klib::util::write_file(Epub::mimetype_path, false, text);
}

}  // namespace klib::epub
