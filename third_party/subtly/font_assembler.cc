/*
 * Copyright 2011 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "subtly/font_assembler.h"

#include <stdio.h>

#include <map>
#include <set>

#include "sfntly/font.h"
#include "sfntly/font_factory.h"
#include "sfntly/port/refcount.h"
#include "sfntly/port/type.h"
#include "sfntly/table/core/cmap_table.h"
#include "sfntly/table/core/maximum_profile_table.h"
#include "sfntly/table/truetype/glyph_table.h"
#include "sfntly/table/truetype/loca_table.h"
#include "sfntly/tag.h"
#include "subtly/font_info.h"

namespace subtly {
using namespace sfntly;

FontAssembler::FontAssembler(FontInfo* font_info, IntegerSet* table_blacklist)
    : table_blacklist_(table_blacklist) {
  font_info_ = font_info;
  Initialize();
}

FontAssembler::FontAssembler(FontInfo* font_info) : table_blacklist_(NULL) {
  font_info_ = font_info;
  Initialize();
}

void FontAssembler::Initialize() {
  font_factory_.Attach(sfntly::FontFactory::GetInstance());
  font_builder_.Attach(font_factory_->NewFontBuilder());
}

CALLER_ATTACH Font* FontAssembler::Assemble() {
  // Assemble tables we can subset.
  if (!AssembleCMapTable() || !AssembleGlyphAndLocaTables()) {
    return NULL;
  }
  // For all other tables, either include them unmodified or don't at all.
  const TableMap* common_table_map =
      font_info_->GetTableMap(font_info_->fonts()->begin()->first);
  for (TableMap::const_iterator it = common_table_map->begin(),
                                e = common_table_map->end();
       it != e; ++it) {
    if (table_blacklist_ &&
        table_blacklist_->find(it->first) != table_blacklist_->end()) {
      continue;
    }
    font_builder_->NewTableBuilder(it->first, it->second->ReadFontData());
  }
  return font_builder_->Build();
}

bool FontAssembler::AssembleCMapTable() {
  // Creating the new CMapTable and the new format 4 CMap
  Ptr<CMapTable::Builder> cmap_table_builder =
      down_cast<CMapTable::Builder*>(font_builder_->NewTableBuilder(Tag::cmap));
  if (!cmap_table_builder) return false;
  Ptr<CMapTable::CMapFormat4::Builder> cmap_builder =
      down_cast<CMapTable::CMapFormat4::Builder*>(
          cmap_table_builder->NewCMapBuilder(CMapFormat::kFormat4,
                                             CMapTable::WINDOWS_BMP));
  if (!cmap_builder) return false;
  // Creating the segments and the glyph id array
  CharacterMap* chars_to_glyph_ids = font_info_->chars_to_glyph_ids();
  SegmentList* segment_list = new SegmentList;
  IntegerList* glyph_id_array = new IntegerList;
  int32_t last_chararacter = -2;
  int32_t last_offset = 0;
  Ptr<CMapTable::CMapFormat4::Builder::Segment> current_segment;

  // For simplicity, we will have one segment per contiguous range.
  // To test the algorithm, we've replaced the original CMap with the CMap
  // generated by this code without removing any character.
  // Tuffy.ttf: CMap went from 3146 to 3972 bytes (1.7% to 2.17% of file)
  // AnonymousPro.ttf: CMap went from 1524 to 1900 bytes (0.96% to 1.2%)
  for (CharacterMap::iterator it = chars_to_glyph_ids->begin(),
                              e = chars_to_glyph_ids->end();
       it != e; ++it) {
    int32_t character = it->first;
    int32_t glyph_id = it->second.glyph_id();
    if (character != last_chararacter + 1) {  // new segment
      if (current_segment != NULL) {
        current_segment->set_end_count(last_chararacter);
        segment_list->push_back(current_segment);
      }
      // start_code = character
      // end_code = -1 (unknown for now)
      // id_delta = 0 (we don't use id_delta for this representation)
      // id_range_offset = last_offset (offset into the glyph_id_array)
      current_segment = new CMapTable::CMapFormat4::Builder::Segment(
          character, -1, 0, last_offset);
    }
    glyph_id_array->push_back(glyph_id);
    last_offset += DataSize::kSHORT;
    last_chararacter = character;
  }
  // The last segment is still open.
  current_segment->set_end_count(last_chararacter);
  segment_list->push_back(current_segment);
  // Updating the id_range_offset for every segment.
  for (int32_t i = 0, num_segs = segment_list->size(); i < num_segs; ++i) {
    Ptr<CMapTable::CMapFormat4::Builder::Segment> segment = segment_list->at(i);
    segment->set_id_range_offset(segment->id_range_offset() +
                                 (num_segs - i + 1) * DataSize::kSHORT);
  }
  // Adding the final, required segment.
  current_segment =
      new CMapTable::CMapFormat4::Builder::Segment(0xffff, 0xffff, 1, 0);
  segment_list->push_back(current_segment);
  // Writing the segments and glyph id array to the CMap
  cmap_builder->set_segments(segment_list);
  cmap_builder->set_glyph_id_array(glyph_id_array);
  delete segment_list;
  delete glyph_id_array;
  return true;
}

bool FontAssembler::AssembleGlyphAndLocaTables() {
  Ptr<LocaTable::Builder> loca_table_builder =
      down_cast<LocaTable::Builder*>(font_builder_->NewTableBuilder(Tag::loca));
  Ptr<GlyphTable::Builder> glyph_table_builder =
      down_cast<GlyphTable::Builder*>(
          font_builder_->NewTableBuilder(Tag::glyf));

  GlyphIdSet* resolved_glyph_ids = font_info_->resolved_glyph_ids();
  IntegerList loca_list;
  // Basic sanity check: all LOCA tables are of the same size
  // This is necessary but not suficient!
  int32_t previous_size = -1;
  for (FontIdMap::iterator it = font_info_->fonts()->begin();
       it != font_info_->fonts()->end(); ++it) {
    Ptr<LocaTable> loca_table =
        down_cast<LocaTable*>(font_info_->GetTable(it->first, Tag::loca));
    int32_t current_size = loca_table->header_length();
    if (previous_size != -1 && current_size != previous_size) {
      return false;
    }
    previous_size = current_size;
  }

  // Assuming all fonts referenced by the FontInfo are the subsets of the same
  // font, their loca tables should all have the same sizes.
  // We'll just get the size of the first font's LOCA table for simplicty.
  Ptr<LocaTable> first_loca_table = down_cast<LocaTable*>(
      font_info_->GetTable(font_info_->fonts()->begin()->first, Tag::loca));
  int32_t num_loca_glyphs = first_loca_table->num_glyphs();
  loca_list.resize(num_loca_glyphs);
  loca_list.push_back(0);
  int32_t last_glyph_id = 0;
  int32_t last_offset = 0;
  GlyphTable::GlyphBuilderList* glyph_builders =
      glyph_table_builder->GlyphBuilders();

  for (GlyphIdSet::iterator it = resolved_glyph_ids->begin(),
                            e = resolved_glyph_ids->end();
       it != e; ++it) {
    // Get the glyph for this resolved_glyph_id.
    int32_t resolved_glyph_id = it->glyph_id();
    int32_t font_id = it->font_id();
    // Get the LOCA table for the current glyph id.
    Ptr<LocaTable> loca_table =
        down_cast<LocaTable*>(font_info_->GetTable(font_id, Tag::loca));
    int32_t length = loca_table->GlyphLength(resolved_glyph_id);
    int32_t offset = loca_table->GlyphOffset(resolved_glyph_id);

    // Get the GLYF table for the current glyph id.
    Ptr<GlyphTable> glyph_table =
        down_cast<GlyphTable*>(font_info_->GetTable(font_id, Tag::glyf));
    GlyphPtr glyph;
    glyph.Attach(glyph_table->GetGlyph(offset, length));

    // The data reference by the glyph is copied into a new glyph and
    // added to the glyph_builders belonging to the glyph_table_builder.
    // When Build gets called, all the glyphs will be built.
    Ptr<ReadableFontData> data = glyph->ReadFontData();
    Ptr<WritableFontData> copy_data;
    copy_data.Attach(WritableFontData::CreateWritableFontData(data->Length()));
    data->CopyTo(copy_data);
    GlyphBuilderPtr glyph_builder;
    glyph_builder.Attach(glyph_table_builder->GlyphBuilder(copy_data));
    glyph_builders->push_back(glyph_builder);

    // If there are missing glyphs between the last glyph_id and the
    // current resolved_glyph_id, since the LOCA table needs to have the same
    // size, the offset is kept the same.
    loca_list.resize(
        std::max(loca_list.size(), static_cast<size_t>(resolved_glyph_id + 2)));
    for (int32_t i = last_glyph_id + 1; i <= resolved_glyph_id; ++i)
      loca_list[i] = last_offset;
    last_offset += length;
    loca_list[resolved_glyph_id + 1] = last_offset;
    last_glyph_id = resolved_glyph_id + 1;
  }
  // If there are missing glyph ids, their loca entries must all point
  // to the same offset as the last valid glyph id making them all zero length.
  for (int32_t i = last_glyph_id + 1; i <= num_loca_glyphs; ++i)
    loca_list[i] = last_offset;
  loca_table_builder->SetLocaList(&loca_list);
  return true;
}
}  // namespace subtly
