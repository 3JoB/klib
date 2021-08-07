#include "klib/epub.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <pugixml.hpp>

#include "klib/exception.h"
#include "klib/icu.h"
#include "klib/util.h"
#include "klib/version.h"

namespace klib::epub {

namespace {

void append_manifest(pugi::xml_node &manifest, const std::string &id,
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
}

void append_spine(pugi::xml_node &spine, const std::string &idref) {
  auto itemref = spine.append_child("itemref");
  itemref.append_attribute("idref") = idref.c_str();
}

void append_nav_map(pugi::xml_node &nav_map, const std::string &text,
                    const std::string &src) {
  auto back = nav_map.last_child().attribute("playOrder").as_int();

  auto nav_point = nav_map.append_child("navPoint");
  nav_point.attribute("id") = ("navPoint-" + std::to_string(back)).c_str();
  nav_point.attribute("playOrder") = std::to_string(back).c_str();

  auto nav_label = nav_point.append_child("navLabel");
  nav_label.append_child("text").text().set(text.c_str());
  nav_label.append_child("content").attribute("src") = src.c_str();
}

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

}  // namespace

void Epub::generate_container() const {
  pugi::xml_document doc;
  auto decl = doc.prepend_child(pugi::node_declaration);
  decl.append_attribute("version") = "1.0";
  decl.append_attribute("encoding") = "UTF-8";

  auto container = doc.append_child("container");
  container.append_attribute("version") = "1.0";
  container.append_attribute("xmlns") =
      "urn:oasis:names:tc:opendocument:xmlns:container";

  auto rootfiles = container.append_child("rootfiles");
  auto rootfile = rootfiles.append_child("rootfile");
  rootfile.append_attribute("full-path") = content_path_;
  rootfile.append_attribute("media-type") = "application/oebps-package+xml";

  if (!doc.save_file(container_path_, ident_)) {
    throw klib::RuntimeError("can not save: {}", container_path_);
  }
}

void Epub::set_book_name(const std::string &book_name) {
  book_name_ = book_name;
  root_ = book_name_;
}

void Epub::generate() {
  auto uuid = boost::uuids::random_generator()();
  uuid_ += boost::uuids::to_string(uuid);

  std::filesystem::create_directory(book_name_);
  klib::util::ChangeWorkingDir change_working_dir(book_name_);

  std::filesystem::create_directory("META-INF");
  std::filesystem::create_directory("OEBPS");
  std::filesystem::create_directory("OEBPS/Fonts");

  if (image_num_ > 0) {
    std::filesystem::create_directory("OEBPS/Images");
  }

  std::filesystem::create_directory("OEBPS/Styles");
  std::filesystem::create_directory("OEBPS/Text");

  generate_container();
  generate_mimetype();
  generate_content();
  generate_toc();
}

void Epub::generate_mimetype() const {
  klib::util::write_file(mimetype_path_, false, "application/epub+zip");
}

void Epub::generate_content() const {
  pugi::xml_document doc;
  auto decl = doc.prepend_child(pugi::node_declaration);
  decl.append_attribute("version") = "1.0";
  decl.append_attribute("encoding") = "UTF-8";

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
  dc_date.text() = get_date().c_str();

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
  append_manifest(manifest, "ncx", "toc.ncx");
  append_manifest(manifest, "style.css", "Styles/style.css");
  append_manifest(manifest, "SourceHanSansHWSC-Bold.otf",
                  "Fonts/SourceHanSansHWSC-Bold.otf");
  for (std::int32_t i = 0; i < image_num_; ++i) {
    append_manifest(manifest, "x" + num_to_str(i) + ".jpg",
                    "Images/" + num_to_str(i) + ".jpg");
  }
  if (generate_cover_) {
    append_manifest(manifest, "cover.jpg", "Images/cover.jpg");
    append_manifest(manifest, "cover.xhtml", "Text/cover.xhtml");
  }
  append_manifest(manifest, "message.xhtml", "Text/message.xhtml");
  append_manifest(manifest, "introduction.xhtml", "Text/introduction.xhtml");

  for (std::int32_t i = 0; i < illustration_num_; ++i) {
    append_manifest(manifest, "illustration" + num_to_str(i) + ".xhtml",
                    "Text/illustration" + num_to_str(i) + ".xhtml");
  }

  auto size = std::size(content_);
  for (std::size_t i = 0; i < size; ++i) {
    append_manifest(manifest, "chapter" + num_to_str(i) + ".xhtml",
                    "Text/chapter" + num_to_str(i) + ".xhtml");
  }

  if (generate_postscript_) {
    append_manifest(manifest, "postscript.xhtml", "Text/postscript.xhtml");
  }

  auto spine = package.append_child("spine");
  spine.append_attribute("toc") = "ncx";
  if (generate_cover_) {
    append_spine(spine, "cover.xhtml");
  }
  append_spine(spine, "cover.xhtml");
  append_spine(spine, "message.xhtml");
  append_spine(spine, "introduction.xhtml");

  for (std::int32_t i = 0; i < illustration_num_; ++i) {
    append_spine(spine, "illustration" + num_to_str(i) + ".xhtml");
  }
  for (std::size_t i = 0; i < size; ++i) {
    append_spine(spine, "chapter" + num_to_str(i) + ".xhtml");
  }
  if (generate_postscript_) {
    append_spine(spine, "postscript.xhtml");
  }

  auto guide = package.append_child("guide");
  if (generate_cover_) {
    auto reference = guide.append_child("reference");
    reference.append_attribute("type") = "cover";
    reference.append_attribute("title") = "Cover";
    reference.append_attribute("href") = "Text/cover.xhtml";
  }

  if (!doc.save_file(content_path_, ident_)) {
    throw klib::RuntimeError("can not save: {}", content_path_);
  }
}

void Epub::generate_toc() const {
  pugi::xml_document doc;
  auto decl = doc.prepend_child(pugi::node_declaration);
  decl.append_attribute("version") = "1.0";
  decl.append_attribute("encoding") = "UTF-8";

  auto ncx = doc.append_child("ncx");
  ncx.append_attribute("xmlns") = "http://www.daisy.org/z3986/2005/ncx/";
  ncx.append_attribute("version") = "2005-1";

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
                   "Text/chapter" + num_to_str(i) + ".xhtml");
  }

  if (generate_postscript_) {
    append_nav_map(nav_map, "后记", "Text/postscript.xhtml");
  }

  if (!doc.save_file(toc_path_, ident_)) {
    throw klib::RuntimeError("can not save: {}", toc_path_);
  }
}

}  // namespace klib::epub
