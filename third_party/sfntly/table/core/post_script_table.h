#ifndef FONT_SUBSETTER_POST_SCRIPT_TABLE_H
#define FONT_SUBSETTER_POST_SCRIPT_TABLE_H

#include <string>

#include "sfntly/table/table.h"
#include "sfntly/table/table_based_table_builder.h"

namespace sfntly {

class PostScriptTable : public Table, public RefCounted<PostScriptTable> {
 public:
  constexpr static int32_t VERSION_1 = 0x10000;
  constexpr static int32_t VERSION_2 = 0x20000;
  constexpr static int32_t NUM_STANDARD_NAMES = 258;

  enum class Offset {
    kVersion = 0,
    kItalicAngle = 4,
    kUnderlinePosition = 8,
    kUnderlineThickness = 10,
    kIsFixedPitch = 12,
    kMinMemType42 = 16,
    kMaxMemType42 = 20,
    kMinMemType1 = 24,
    kMaxMemType1 = 28,
    kNumberOfGlyphs = 32,
    kGlyphNameIndex = 34,
  };

  constexpr static const char* STANDARD_NAMES[] = {".notdef",
                                                   ".null",
                                                   "nonmarkingreturn",
                                                   "space",
                                                   "exclam",
                                                   "quotedbl",
                                                   "numbersign",
                                                   "dollar",
                                                   "percent",
                                                   "ampersand",
                                                   "quotesingle",
                                                   "parenleft",
                                                   "parenright",
                                                   "asterisk",
                                                   "plus",
                                                   "comma",
                                                   "hyphen",
                                                   "period",
                                                   "slash",
                                                   "zero",
                                                   "one",
                                                   "two",
                                                   "three",
                                                   "four",
                                                   "five",
                                                   "six",
                                                   "seven",
                                                   "eight",
                                                   "nine",
                                                   "colon",
                                                   "semicolon",
                                                   "less",
                                                   "equal",
                                                   "greater",
                                                   "question",
                                                   "at",
                                                   "A",
                                                   "B",
                                                   "C",
                                                   "D",
                                                   "E",
                                                   "F",
                                                   "G",
                                                   "H",
                                                   "I",
                                                   "J",
                                                   "K",
                                                   "L",
                                                   "M",
                                                   "N",
                                                   "O",
                                                   "P",
                                                   "Q",
                                                   "R",
                                                   "S",
                                                   "T",
                                                   "U",
                                                   "V",
                                                   "W",
                                                   "X",
                                                   "Y",
                                                   "Z",
                                                   "bracketleft",
                                                   "backslash",
                                                   "bracketright",
                                                   "asciicircum",
                                                   "underscore",
                                                   "grave",
                                                   "a",
                                                   "b",
                                                   "c",
                                                   "d",
                                                   "e",
                                                   "f",
                                                   "g",
                                                   "h",
                                                   "i",
                                                   "j",
                                                   "k",
                                                   "l",
                                                   "m",
                                                   "n",
                                                   "o",
                                                   "p",
                                                   "q",
                                                   "r",
                                                   "s",
                                                   "t",
                                                   "u",
                                                   "v",
                                                   "w",
                                                   "x",
                                                   "y",
                                                   "z",
                                                   "braceleft",
                                                   "bar",
                                                   "braceright",
                                                   "asciitilde",
                                                   "Adieresis",
                                                   "Aring",
                                                   "Ccedilla",
                                                   "Eacute",
                                                   "Ntilde",
                                                   "Odieresis",
                                                   "Udieresis",
                                                   "aacute",
                                                   "agrave",
                                                   "acircumflex",
                                                   "adieresis",
                                                   "atilde",
                                                   "aring",
                                                   "ccedilla",
                                                   "eacute",
                                                   "egrave",
                                                   "ecircumflex",
                                                   "edieresis",
                                                   "iacute",
                                                   "igrave",
                                                   "icircumflex",
                                                   "idieresis",
                                                   "ntilde",
                                                   "oacute",
                                                   "ograve",
                                                   "ocircumflex",
                                                   "odieresis",
                                                   "otilde",
                                                   "uacute",
                                                   "ugrave",
                                                   "ucircumflex",
                                                   "udieresis",
                                                   "dagger",
                                                   "degree",
                                                   "cent",
                                                   "sterling",
                                                   "section",
                                                   "bullet",
                                                   "paragraph",
                                                   "germandbls",
                                                   "registered",
                                                   "copyright",
                                                   "trademark",
                                                   "acute",
                                                   "dieresis",
                                                   "notequal",
                                                   "AE",
                                                   "Oslash",
                                                   "infinity",
                                                   "plusminus",
                                                   "lessequal",
                                                   "greaterequal",
                                                   "yen",
                                                   "mu",
                                                   "partialdiff",
                                                   "summation",
                                                   "product",
                                                   "pi",
                                                   "integral",
                                                   "ordfeminine",
                                                   "ordmasculine",
                                                   "Omega",
                                                   "ae",
                                                   "oslash",
                                                   "questiondown",
                                                   "exclamdown",
                                                   "logicalnot",
                                                   "radical",
                                                   "florin",
                                                   "approxequal",
                                                   "Delta",
                                                   "guillemotleft",
                                                   "guillemotright",
                                                   "ellipsis",
                                                   "nonbreakingspace",
                                                   "Agrave",
                                                   "Atilde",
                                                   "Otilde",
                                                   "OE",
                                                   "oe",
                                                   "endash",
                                                   "emdash",
                                                   "quotedblleft",
                                                   "quotedblright",
                                                   "quoteleft",
                                                   "quoteright",
                                                   "divide",
                                                   "lozenge",
                                                   "ydieresis",
                                                   "Ydieresis",
                                                   "fraction",
                                                   "currency",
                                                   "guilsinglleft",
                                                   "guilsinglright",
                                                   "fi",
                                                   "fl",
                                                   "daggerdbl",
                                                   "periodcentered",
                                                   "quotesinglbase",
                                                   "quotedblbase",
                                                   "perthousand",
                                                   "Acircumflex",
                                                   "Ecircumflex",
                                                   "Aacute",
                                                   "Edieresis",
                                                   "Egrave",
                                                   "Iacute",
                                                   "Icircumflex",
                                                   "Idieresis",
                                                   "Igrave",
                                                   "Oacute",
                                                   "Ocircumflex",
                                                   "apple",
                                                   "Ograve",
                                                   "Uacute",
                                                   "Ucircumflex",
                                                   "Ugrave",
                                                   "dotlessi",
                                                   "circumflex",
                                                   "tilde",
                                                   "macron",
                                                   "breve",
                                                   "dotaccent",
                                                   "ring",
                                                   "cedilla",
                                                   "hungarumlaut",
                                                   "ogonek",
                                                   "caron",
                                                   "Lslash",
                                                   "lslash",
                                                   "Scaron",
                                                   "scaron",
                                                   "Zcaron",
                                                   "zcaron",
                                                   "brokenbar",
                                                   "Eth",
                                                   "eth",
                                                   "Yacute",
                                                   "yacute",
                                                   "Thorn",
                                                   "thorn",
                                                   "minus",
                                                   "multiply",
                                                   "onesuperior",
                                                   "twosuperior",
                                                   "threesuperior",
                                                   "onehalf",
                                                   "onequarter",
                                                   "threequarters",
                                                   "franc",
                                                   "Gbreve",
                                                   "gbreve",
                                                   "Idotaccent",
                                                   "Scedilla",
                                                   "scedilla",
                                                   "Cacute",
                                                   "cacute",
                                                   "Ccaron",
                                                   "ccaron",
                                                   "dcroat"};

  PostScriptTable(Header* header, ReadableFontData* data);
  int32_t Version();
  int64_t IsFixedPitchRaw();
  int32_t NumberOfGlyphs();

  std::string GlyphName(int32_t glyphNum);

 private:
  std::vector<std::string> getNames();
  std::vector<std::string> parse();

  std::vector<std::string> names_;

 public:
  class Builder : public TableBasedTableBuilder, public RefCounted<Builder> {
   public:
    Builder(Header* header, WritableFontData* data);
    Builder(Header* header, ReadableFontData* data);
    virtual ~Builder(){};
    virtual CALLER_ATTACH FontDataTable* SubBuildTable(ReadableFontData* data);

    static CALLER_ATTACH Builder* CreateBuilder(Header* header,
                                                WritableFontData* data);
  };
};

}  // namespace sfntly

#endif  // FONT_SUBSETTER_POST_SCRIPT_TABLE_H