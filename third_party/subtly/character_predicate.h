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

#ifndef TYPOGRAPHY_FONT_SFNTLY_SRC_SAMPLE_SUBTLY_CHARACTER_PREDICATE_H_
#define TYPOGRAPHY_FONT_SFNTLY_SRC_SAMPLE_SUBTLY_CHARACTER_PREDICATE_H_

#include "sfntly/port/refcount.h"
#include "sfntly/port/type.h"

namespace subtly {
class CharacterPredicate : virtual public sfntly::RefCount {
 public:
  CharacterPredicate() {}
  virtual ~CharacterPredicate() {}
  virtual bool operator()(int32_t character) const = 0;
};

// All characters in IntegerSet
// The set is OWNED by the predicate! Do not modify it.
// It will be freed when the predicate is destroyed.
class AcceptSet : public CharacterPredicate,
                  public sfntly::RefCounted<AcceptSet> {
 public:
  explicit AcceptSet(sfntly::IntegerSet* characters);
  ~AcceptSet();
  virtual bool operator()(int32_t character) const;

 private:
  sfntly::IntegerSet* characters_;
};
}  // namespace subtly

#endif  // TYPOGRAPHY_FONT_SFNTLY_SRC_SAMPLE_SUBTLY_CHARACTER_PREDICATE_H_
