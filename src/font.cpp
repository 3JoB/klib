#include "klib/font.h"

#include <cstdint>

#include "klib/exception.h"
#include "sfntly/font.h"
#include "sfntly/port/refcount.h"
#include "subtly/character_predicate.h"
#include "subtly/subsetter.h"
#include "subtly/utils.h"

namespace klib {

void font_subset(const std::string &font_path, const std::string &out_path,
                 const std::u32string &unicodes) {
  sfntly::FontPtr font;
  font.Attach(subtly::LoadFont(font_path.c_str()));
  if (font->num_tables() == 0) {
    throw RuntimeError("Could not load font: {}", font_path);
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
    throw RuntimeError("Cannot create subset", font_path);
  }

  if (!subtly::SerializeFont(out_path.c_str(), new_font)) {
    throw RuntimeError("Cannot create font file", font_path);
  }
}

}  // namespace klib
