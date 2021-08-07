#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace klib::epub {

class Epub {
 public:
  void set_creator(const std::string &creator);

  void set_book_name(const std::string &book_name);
  void set_author(const std::string &author);
  void set_introduction(const std::vector<std::string> &introduction);

  void set_generate_cover(bool generate_cover);
  void set_generate_postscript(bool generate_postscript);
  void set_illustration_num(std::int32_t illustration_num);
  void set_image_num(std::int32_t image_num);

  void set_convert_tc_to_sc(bool convert_tc_to_sc);
  void set_old_style(bool old_style);

  void add_content(const std::string &title,
                   const std::vector<std::string> &content);
  void add_content(const std::string &volume_name, const std::string &title,
                   const std::vector<std::string> &content);

  void generate();

 private:
  void generate_container() const;
  void generate_mimetype() const;
  void generate_content() const;
  void generate_toc() const;

  void generate_image() const;
  void generate_cover() const;
  void generate_message() const;
  void generate_introduction() const;
  void generate_illustration() const;
  void generate_chapter() const;
  void generate_postscript() const;

  std::string creator_ = "TODO";
  std::string book_name_ = "TODO";
  std::string author_ = "TODO";
  std::string introduction_ = {"TODO"};

  std::vector<std::pair<std::string, std::vector<std::string>>> content_;

  bool generate_cover_ = false;
  bool generate_postscript_ = false;
  std::int32_t illustration_num_ = 0;
  std::int32_t image_num_ = 0;

  bool convert_tc_to_sc_ = false;
  // TODO ???
  bool connect_chinese_ = false;
  bool old_style_ = false;

  std::filesystem::path root_;

  const char *container_path_ = "META-INF/container.xml";
  const char *content_path_ = "OEBPS/content.opf";
  const char *toc_path_ = "OEBPS/toc.ncx";
  const char *mimetype_path_ = "mimetype";

  const char *ident_ = "    ";
  std::string uuid_ = "urn:uuid:";
};

}  // namespace klib::epub
