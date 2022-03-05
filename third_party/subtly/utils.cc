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

#include "subtly/utils.h"

#include "sfntly/port/memory_input_stream.h"

namespace subtly {
using namespace sfntly;

CALLER_ATTACH sfntly::Font* LoadFont(const char* font, std::size_t size) {
  Ptr<FontFactory> font_factory;
  font_factory.Attach(FontFactory::GetInstance());
  FontArray fonts;
  LoadFonts(font, size, font_factory, &fonts);
  return fonts[0].Detach();
}

void LoadFonts(const char* font, std::size_t size, sfntly::FontFactory* factory,
               sfntly::FontArray* fonts) {
  MemoryInputStream input_stream;
  input_stream.Attach(reinterpret_cast<const uint8_t*>(font), size);
  factory->LoadFonts(&input_stream, fonts);
  input_stream.Close();
}
};  // namespace subtly
