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

#include "subtly/subsetter.h"

#include "sfntly/font.h"
#include "sfntly/font_factory.h"
#include "sfntly/tag.h"
#include "subtly/character_predicate.h"
#include "subtly/font_assembler.h"
#include "subtly/font_info.h"

namespace subtly {
using namespace sfntly;

/******************************************************************************
 * Subsetter class
 ******************************************************************************/
Subsetter::Subsetter(Font* font, CharacterPredicate* predicate)
    : font_(font), predicate_(predicate) {}

CALLER_ATTACH Font* Subsetter::Subset() {
  Ptr<FontSourcedInfoBuilder> info_builder =
      new FontSourcedInfoBuilder(font_, 0, predicate_);

  Ptr<FontInfo> font_info;
  font_info.Attach(info_builder->GetFontInfo());
  if (!font_info) {
#if defined(SUBTLY_DEBUG)
    fprintf(stderr,
            "Couldn't create font info. No subset will be generated.\n");
#endif
    return NULL;
  }
  IntegerSet* table_blacklist = new IntegerSet;
  table_blacklist->insert(Tag::DSIG);
  table_blacklist->insert(Tag::GDEF);
  table_blacklist->insert(Tag::GPOS);
  table_blacklist->insert(Tag::GSUB);
  table_blacklist->insert(Tag::kern);
  table_blacklist->insert(Tag::hdmx);
  table_blacklist->insert(Tag::vmtx);
  table_blacklist->insert(Tag::VDMX);
  table_blacklist->insert(Tag::LTSH);
  table_blacklist->insert(Tag::vhea);
  table_blacklist->insert(Tag::morx);
  table_blacklist->insert(GenerateTag('m', 'o', 'r', 't'));

  Ptr<FontAssembler> font_assembler =
      new FontAssembler(font_info, table_blacklist);
  Ptr<Font> font_subset;
  font_subset.Attach(font_assembler->Assemble());
  delete table_blacklist;
  return font_subset.Detach();
}
}  // namespace subtly
