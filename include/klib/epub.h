/**
 * @file epub.h
 * @brief Contains generate epub file modules
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace klib::epub {

class Epub {
 public:
  /**
   * @brief Default constructor
   */
  Epub();

  /**
   * @brief Set the name of the creator
   * @param creator: Represents the name of the creator
   */
  void set_creator(const std::string &creator);

  /**
   * @brief Set book name
   * @param book_name: Represents the book name
   */
  void set_book_name(const std::string &book_name);

  /**
   * @brief Set the name of the book author
   * @param author: Represents the name of the book author
   */
  void set_author(const std::string &author);

  /**
   * @brief Set the book introduction
   * @param introduction: Represents the book introduction
   */
  void set_introduction(const std::vector<std::string> &introduction);

  /**
   * @brief Set whether to generate cover
   * @param generate_cover: True to generate cover
   */
  void set_generate_cover(bool generate_cover);

  /**
   * @brief Set whether to generate postscript
   * @param generate_postscript: True to generate postscript
   */
  void set_generate_postscript(bool generate_postscript);

  /**
   * @brief Set the number of illustration
   * @param illustration_num: The number of illustration
   */
  void set_illustration_num(std::int32_t illustration_num);

  /**
   * @brief Set the number of images
   * @param image_num: The number of images
   */
  void set_image_num(std::int32_t image_num);

  /**
   * @brief Set font
   * @param font: The string obtained by reading the font file
   */
  void set_font(const std::string &font);
  void set_font(std::string_view font);

  /**
   * @brief Set uuid
   * @param uuid: Represents the uuid
   * @note For testing
   */
  void set_uuid(const std::string &uuid);

  /**
   * @brief Set date
   * @param uuid: Represents the date
   * @note For testing
   */
  void set_date(const std::string &date);

  /**
   * @brief Add book title and text
   * @param title: Represents title
   * @param text: Represents text
   */
  void add_content(const std::string &title,
                   const std::vector<std::string> &text);

  /**
   * @brief Add book title and text(book sub-volume)
   * @param volume_name: Represents Volume name
   * @param title: Represents title
   * @param text: Represents text
   */
  void add_content(const std::string &volume_name, const std::string &title,
                   const std::vector<std::string> &content);

  /**
   * @brief Generate epub file
   * @param archive: Whether to compress
   */
  void generate(bool archive = true);

  /**
   * @brief Epub file internal path
   * @note For testing
   */
  constexpr static std::string_view meta_inf_dir = "META-INF";
  constexpr static std::string_view oebps_dir = "OEBPS";
  constexpr static std::string_view fonts_dir = "OEBPS/Fonts";
  constexpr static std::string_view images_dir = "OEBPS/Images";
  constexpr static std::string_view styles_dir = "OEBPS/Styles";
  constexpr static std::string_view text_dir = "OEBPS/Text";

  constexpr static std::string_view container_path = "META-INF/container.xml";
  constexpr static std::string_view font_path =
      "OEBPS/Fonts/SourceHanSansHWSC-Bold.otf";
  constexpr static std::string_view style_path = "OEBPS/Styles/style.css";
  constexpr static std::string_view cover_path = "OEBPS/Text/cover.xhtml";
  constexpr static std::string_view introduction_path =
      "OEBPS/Text/introduction.xhtml";
  constexpr static std::string_view message_path = "OEBPS/Text/message.xhtml";
  constexpr static std::string_view postscript_path =
      "OEBPS/Text/postscript.xhtml";
  constexpr static std::string_view content_path = "OEBPS/content.opf";
  constexpr static std::string_view toc_path = "OEBPS/toc.ncx";
  constexpr static std::string_view mimetype_path = "mimetype";

 private:
  void generate_container() const;
  void generate_font() const;
  void generate_style() const;
  void generate_chapter() const;
  void generate_cover() const;
  void generate_illustration() const;
  void generate_introduction() const;
  void generate_message() const;
  void generate_postscript() const;
  void generate_content() const;
  void generate_toc() const;
  void generate_mimetype() const;

  std::string creator_ = "TODO";
  std::string book_name_ = "TODO";
  std::string author_ = "TODO";
  std::vector<std::string> introduction_ = {"TODO"};

  bool generate_cover_ = false;
  bool generate_postscript_ = false;
  std::int32_t illustration_num_ = 0;
  std::int32_t image_num_ = 0;

  std::string uuid_;
  std::string date_;

  std::string_view style_;
  std::string_view font_;
  std::vector<std::tuple<std::string, std::string, std::vector<std::string>>>
      content_;
};

}  // namespace klib::epub
