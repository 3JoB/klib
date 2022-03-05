#include "sfntly/table/core/post_script_table.h"

namespace sfntly {
PostScriptTable::PostScriptTable(sfntly::Header *header,
                                 sfntly::ReadableFontData *data)
    : Table(header, data) {}

int32_t PostScriptTable::Version() {
  return this->data_->ReadFixed(static_cast<int32_t>(Offset::kVersion));
}

int32_t PostScriptTable::NumberOfGlyphs() {
  if (this->Version() == VERSION_1) {
    return NUM_STANDARD_NAMES;
  } else if (this->Version() == VERSION_2) {
    return this->data_->ReadUShort(
        static_cast<int32_t>(Offset::kNumberOfGlyphs));
  } else {
    // TODO: should probably be better at signaling unsupported format
    return -1;
  }
}

std::string PostScriptTable::GlyphName(int32_t glyphNum) {
  int32_t numberOfGlyphs = this->NumberOfGlyphs();
  if (numberOfGlyphs > 0 && (glyphNum < 0 || glyphNum >= numberOfGlyphs)) {
#if defined(SUBTLY_DEBUG)
    fprintf(
        stderr,
        "numberOfGlyphs > 0 && (glyphNum < 0 || glyphNum >= numberOfGlyphs)");
#endif
    return "";
  }
  int32_t glyphNameIndex = 0;
  if (this->Version() == VERSION_1) {
    glyphNameIndex = glyphNum;
  } else if (this->Version() == VERSION_2) {
    glyphNameIndex = this->data_->ReadUShort(
        static_cast<int32_t>(Offset::kGlyphNameIndex) + 2 * glyphNum);
  } else {
    return "";
  }

  if (glyphNameIndex < NUM_STANDARD_NAMES) {
    return STANDARD_NAMES[glyphNameIndex];
  }
  return getNames()[glyphNameIndex - NUM_STANDARD_NAMES];
}

std::vector<std::string> PostScriptTable::getNames() {
  std::vector<std::string> result = names_;
  if (result.empty() && this->Version() == VERSION_2) {
    result = this->parse();
    names_ = result;
  }
  return result;
}

std::vector<std::string> PostScriptTable::parse() {
  std::vector<std::string> names;
  if (this->Version() == VERSION_2) {
    int32_t index =
        static_cast<int32_t>(Offset::kGlyphNameIndex) + 2 * NumberOfGlyphs();
    while (index < this->DataLength()) {
      int32_t strlen = this->data_->ReadUByte(index);
      uint8_t *nameBytes = new uint8_t[strlen];
      this->data_->ReadBytes(index + 1, nameBytes, 0, strlen);
      names.emplace_back((char *)nameBytes);
      index += 1 + strlen;
      delete[] nameBytes;
    }
  } else if (Version() == VERSION_1) {
#if defined(SUBTLY_DEBUG)
    fprintf(stderr, "Not meaningful to parse version 1 table");
#endif
    return std::vector<std::string>();
  }
  return names;
}

PostScriptTable::Builder::Builder(sfntly::Header *header,
                                  sfntly::ReadableFontData *data)
    : TableBasedTableBuilder(header, data) {}

PostScriptTable::Builder::Builder(sfntly::Header *header,
                                  sfntly::WritableFontData *data)
    : TableBasedTableBuilder(header, data) {}

CALLER_ATTACH
PostScriptTable::Builder *PostScriptTable::Builder::CreateBuilder(
    sfntly::Header *header, sfntly::WritableFontData *data) {
  Ptr<PostScriptTable::Builder> builder;
  builder = new PostScriptTable::Builder(header, data);
  return builder.Detach();
}

CALLER_ATTACH
FontDataTable *PostScriptTable::Builder::SubBuildTable(ReadableFontData *data) {
  FontDataTablePtr table = new PostScriptTable(header(), data);
  return table.Detach();
}
}  // namespace sfntly
