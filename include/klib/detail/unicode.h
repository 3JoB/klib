#pragma once

namespace klib::detail {

// https://stackoverflow.com/questions/62531882/is-there-a-way-to-detect-chinese-characters-in-c-using-boost
template <char32_t a, char32_t b>
class UnicodeRange {
  static_assert(a <= b, "proper range");

 public:
  constexpr bool operator()(char32_t x) const noexcept {
    return x >= a && x <= b;
  }
};

using UnifiedIdeographs = UnicodeRange<0x4E00, 0x9FFF>;
using UnifiedIdeographsA = UnicodeRange<0x3400, 0x4DBF>;
using UnifiedIdeographsB = UnicodeRange<0x20000, 0x2A6DF>;
using UnifiedIdeographsC = UnicodeRange<0x2A700, 0x2B73F>;
using UnifiedIdeographsD = UnicodeRange<0x2B740, 0x2B81F>;
using UnifiedIdeographsE = UnicodeRange<0x2B820, 0x2CEAF>;
using CompatibilityIdeographs = UnicodeRange<0xF900, 0xFAFF>;
using CompatibilityIdeographsSupplement = UnicodeRange<0x2F800, 0x2FA1F>;

}  // namespace klib::detail
