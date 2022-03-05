#include "klib/font.h"

#include <cstdint>

#include <woff2/decode.h>
#include <woff2/encode.h>
#include <woff2/output.h>

#include "klib/exception.h"
#include "sfntly/font.h"
#include "sfntly/port/memory_output_stream.h"
#include "sfntly/port/refcount.h"
#include "subtly/character_predicate.h"
#include "subtly/subsetter.h"
#include "subtly/utils.h"

namespace klib {

std::string ttf_subset(const std::string &ttf_font_path,
                       const std::u32string &unicodes) {
  return ttf_subset(ttf_font_path.c_str(), unicodes);
}

std::string ttf_subset(std::string_view ttf_font_path,
                       const std::u32string &unicodes) {
  return ttf_subset(std::data(ttf_font_path), unicodes);
}

std::string ttf_subset(const char *ttf_font_path,
                       const std::u32string &unicodes) {
  sfntly::FontPtr font;
  font.Attach(subtly::LoadFont(ttf_font_path));
  if (font->num_tables() == 0) {
    throw RuntimeError("Could not load font: {}", ttf_font_path);
  }

  auto characters = new sfntly::IntegerSet;
  for (const auto unicode : unicodes) {
    characters->insert(static_cast<std::int32_t>(unicode));
  }

  sfntly::Ptr<subtly::CharacterPredicate> set_predicate =
      new subtly::AcceptSet(characters);
  sfntly::Ptr<subtly::Subsetter> subsetter =
      new subtly::Subsetter(font, set_predicate);
  sfntly::Ptr<sfntly::Font> new_font;
  new_font.Attach(subsetter->Subset());
  if (!new_font) {
    throw RuntimeError("Cannot create subset: {}", ttf_font_path);
  }

  sfntly::FontFactoryPtr font_factory;
  font_factory.Attach(sfntly::FontFactory::GetInstance());
  sfntly::MemoryOutputStream output_stream;
  font_factory->SerializeFont(new_font, &output_stream);

  std::string result(reinterpret_cast<const char *>(output_stream.Get()),
                     output_stream.Size());

  return result;
}

std::string ttf_to_woff2(const std::string &ttf_font) {
  auto data_ptr = reinterpret_cast<const uint8_t *>(std::data(ttf_font));
  auto data_size = std::size(ttf_font);

  std::string result;
  auto max_size = woff2::MaxWOFF2CompressedSize(data_ptr, data_size);
  result.resize(max_size);

  woff2::WOFF2Params params;
  params.brotli_quality = 6;
  auto rc = woff2::ConvertTTFToWOFF2(
      data_ptr, data_size, reinterpret_cast<uint8_t *>(std::data(result)),
      &max_size, params);
  if (!rc) {
    throw RuntimeError("ConvertTTFToWOFF failed");
  }
  result.resize(max_size);

  return result;
}

std::string woff2_to_ttf(const std::string &woff2_font) {
  auto data_ptr = reinterpret_cast<const uint8_t *>(std::data(woff2_font));
  auto data_size = std::size(woff2_font);

  std::string result;
  auto size = woff2::ComputeWOFF2FinalSize(data_ptr, data_size);
  if (size == 0) {
    throw RuntimeError("ComputeWOFF2FinalSize failed");
  }
  result.resize(size);

  woff2::WOFF2StringOut buff(&result);
  auto rc = woff2::ConvertWOFF2ToTTF(data_ptr, data_size, &buff);
  if (!rc) {
    throw RuntimeError("ConvertWOFF2ToTTF failed");
  }

  return result;
}

}  // namespace klib
