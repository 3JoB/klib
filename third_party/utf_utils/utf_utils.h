//==================================================================================================
//  File:       unicode_utils.h
//
//  Summary:    Header file for fast UTF-8 to UTF-32/UTF-16 conversion routines.
//
//  Copyright (c) 2018 Bob Steagall and KEWB Computing, All Rights Reserved
//==================================================================================================
//
#ifndef KEWB_UNICODE_UTILS_H_DEFINED
#define KEWB_UNICODE_UTILS_H_DEFINED

#include <cstddef>
#include <cstdint>
#include <string>

//- Detect the compiler; only Clang, GCC, and Visual C++ are currently
// supported.
//
#if defined __clang__

#define KEWB_COMPILER_CLANG
#ifdef __OPTIMIZE__
#define KEWB_FORCE_INLINE inline __attribute__((always_inline))
#else
#define KEWB_FORCE_INLINE inline
#endif
#define KEWB_ALIGN_FN __attribute__((aligned(128)))

#elif defined __GNUG__ || defined __GNUC__

#define KEWB_COMPILER_GCC
#ifdef __OPTIMIZE__
#define KEWB_FORCE_INLINE inline __attribute__((always_inline))
#else
#define KEWB_FORCE_INLINE inline
#endif
#define KEWB_ALIGN_FN __attribute__((aligned(128)))

#elif defined _MSC_VER

#define KEWB_COMPILER_MSVC
#ifdef NDEBUG
#define KEWB_FORCE_INLINE inline __forceinline
#else
#define KEWB_FORCE_INLINE inline
#endif
#define KEWB_ALIGN_FN

#else
#error "Unsupported combination of compiler and platform"
#endif

//- Detect OS and include relevant SSE headers; only Linux and Windows are
// currently supported.
//
#if defined __linux__
#define KEWB_PLATFORM_LINUX
#elif defined _WIN32
#define KEWB_PLATFORM_WINDOWS
#else
#error "Unsupported combination of compiler and platform"
#endif

namespace uu {
//--------------------------------------------------------------------------------------------------
/// \brief  Traits style class to perform conversions from UTF-8 to
/// UTF-32/UTF-16
///
/// \details
///     This traits-style class provides a demonstration of functions for
///     converting strings of UTF-8 code units to strings of UTF-32 code points,
///     as well as transcoding UTF-8 into strings of UTF-16 code units.  Its
///     focus is on converting _from_ UTF-8 as quickly as possible, although it
///     does include member functions for converting a UTF-32 code point into
///     sequences of UTF-8/UTF-16 code units.
///
///     It implements conversion from UTF-8 in three different, but related
///     ways:
///       * using a purely DFA-based approach to recognizing valid sequences of
///       UTF-8 code units;
///       * using the DFA-based approach with a short-circuit optimization for
///       ASCII code units;
///       * using the DFA-based approach with an SSE-based optimization for
///       ASCII code units.
///
///     The member functions implement STL-style argument ordering, with source
///     arguments on the left and destination arguments on the right.  The
///     string-to-string conversion member functions are analogous to
///     std::copy() in that the first two arguments define an input range and
///     the third argument defines the starting point of the output range.
///
///     This class is not intended for production usage, as it does not
///     currently provide a mechanism for reporting/handling errors.  No
///     checking is done for null pointers; it is assumed that the input and
///     output pointers sensibly point to buffers that exist.
///
///     Finally, please note that this was developed and tested on x64/x86
///     hardware, and so there is an implicit assumption that UTF-32 code points
///     and UTF-16 code units are little endian.
//--------------------------------------------------------------------------------------------------
//
class UtfUtils {
 public:
  using ptrdiff_t = std::ptrdiff_t;

 public:
  static bool GetCodePoint(char8_t const* pSrc, char8_t const* pSrcEnd,
                           char32_t& cdpt) noexcept;

  //- Conversion to UTF-32/UTF-16 using pre-computed first code unit lookup
  // table.
  //
  static ptrdiff_t SseBigTableConvert(char8_t const* pSrc,
                                      char8_t const* pSrcEnd,
                                      char32_t* pDst) noexcept;

 private:
  enum CharClass : uint8_t {
    ILL = 0,   //- C0..C1, F5..FF  ILLEGAL octets that should never appear in a
               // UTF-8 sequence
               //
    ASC = 1,   //- 00..7F          ASCII leading byte range
               //
    CR1 = 2,   //- 80..8F          Continuation range 1
    CR2 = 3,   //- 90..9F          Continuation range 2
    CR3 = 4,   //- A0..BF          Continuation range 3
               //
    L2A = 5,   //- C2..DF          Leading byte range A / 2-byte sequence
               //
    L3A = 6,   //- E0              Leading byte range A / 3-byte sequence
    L3B = 7,   //- E1..EC, EE..EF  Leading byte range B / 3-byte sequence
    L3C = 8,   //- ED              Leading byte range C / 3-byte sequence
               //
    L4A = 9,   //- F0              Leading byte range A / 4-byte sequence
    L4B = 10,  //- F1..F3          Leading byte range B / 4-byte sequence
    L4C = 11,  //- F4              Leading byte range C / 4-byte sequence
  };

  enum State : uint8_t {
    BGN = 0,    //- Start
    ERR = 12,   //- Invalid sequence
                //
    CS1 = 24,   //- Continuation state 1
    CS2 = 36,   //- Continuation state 2
    CS3 = 48,   //- Continuation state 3
                //
    P3A = 60,   //- Partial 3-byte sequence state A
    P3B = 72,   //- Partial 3-byte sequence state B
                //
    P4A = 84,   //- Partial 4-byte sequence state A
    P4B = 96,   //- Partial 4-byte sequence state B
                //
    END = BGN,  //- Start and End are the same state!
    err = ERR,  //- For readability in the state transition table
  };

  struct FirstUnitInfo {
    char8_t mFirstOctet;
    State mNextState;
  };

  struct alignas(2048) LookupTables {
    FirstUnitInfo maFirstUnitTable[256];
    CharClass maOctetCategory[256];
    State maTransitions[108];
    std::uint8_t maFirstOctetMask[16];
  };

 private:
  static LookupTables const smTables;

 private:
  static int32_t AdvanceWithBigTable(char8_t const*& pSrc,
                                     char8_t const* pSrcEnd,
                                     char32_t& cdpt) noexcept;
  static int32_t AdvanceWithSmallTable(char8_t const*& pSrc,
                                       char8_t const* pSrcEnd,
                                       char32_t& cdpt) noexcept;
  static void ConvertAsciiWithSse(char8_t const*& pSrc,
                                  char32_t*& pDst) noexcept;
  static int32_t GetTrailingZeros(int32_t x) noexcept;
};

//--------------------------------------------------------------------------------------------------
/// \brief  Converts a sequence of UTF-8 code units to a UTF-32 code point.
///
/// \param pSrc
///     A non-null pointer defining the beginning of the input range of code
///     units.
/// \param pSrcEnd
///     A non-null past-the-end pointer defining the end of the input range.
/// \param cdpt
///     A mutable reference to a char32_t variable which will receive the code
///     unit.
///
/// \returns
///     Boolean value `true` on success.
//--------------------------------------------------------------------------------------------------
//
KEWB_FORCE_INLINE bool UtfUtils::GetCodePoint(char8_t const* pSrc,
                                              char8_t const* const pSrcEnd,
                                              char32_t& cdpt) noexcept {
  return (pSrc < pSrcEnd) ? (AdvanceWithSmallTable(pSrc, pSrcEnd, cdpt) != ERR)
                          : false;
}

//--------------------------------------------------------------------------------------------------
/// \brief  Converts a sequence of UTF-8 code units to a UTF-32 code point.
///
/// \details
///     This static member function reads input octets and uses them to traverse
///     a DFA that recognizes valid sequences of UTF-8 code units.  It is the
///     heart of all non-ASCII conversions in all member functions of this
///     class.  This function uses the "big" first-unit lookup table and the
///     state machine table to traverse the DFA.
///
/// \param pSrc
///     A reference to a non-null pointer defining the beginning of the code
///     unit input range.
/// \param pSrcEnd
///     A non-null past-the-end pointer defining the end of the code unit input
///     range.
/// \param cdpt
///     A reference to the output code point.
///
/// \returns
///     An internal flag describing the current DFA state.
//--------------------------------------------------------------------------------------------------
//
KEWB_FORCE_INLINE int32_t UtfUtils::AdvanceWithBigTable(
    char8_t const*& pSrc, char8_t const* const pSrcEnd,
    char32_t& cdpt) noexcept {
  FirstUnitInfo info;  //- The descriptor for the first code unit
  char32_t unit;       //- The current UTF-8 code unit
  int32_t type;        //- The current code unit's character class
  int32_t curr;        //- The current DFA state

  info = smTables.maFirstUnitTable[*pSrc++];  //- Look up the first code unit
                                              // descriptor
  cdpt = info.mFirstOctet;  //- From it, get the initial code point value
  curr = info.mNextState;   //- From it, get the second state

  while (curr > ERR) {
    if (pSrc < pSrcEnd) {
      unit = *pSrc++;  //- Cache the current code unit
      cdpt = (cdpt << 6) |
             (unit & 0x3F);  //- Adjust code point with continuation bits
      type = smTables.maOctetCategory[unit];       //- Look up the code unit's
                                                   // character class
      curr = smTables.maTransitions[curr + type];  //- Look up the next state
    } else {
      return ERR;
    }
  }
  return curr;
}

//--------------------------------------------------------------------------------------------------
/// \brief  Converts a sequence of UTF-8 code units to a UTF-32 code point.
///
/// \details
///     This static member function reads input octets and uses them to traverse
///     a DFA that recognizes valid sequences of UTF-8 code units.  It is the
///     heart of all non-ASCII conversions in all member functions of this
///     class.  This function uses the "small" first-unit lookup table and the
///     state machine table to traverse the DFA.
///
/// \param pSrc
///     A reference to a non-null pointer defining the beginning of the code
///     unit input range.
/// \param pSrcEnd
///     A non-null past-the-end pointer defining the end of the code unit input
///     range.
/// \param cdpt
///     A reference to the output code point.
///
/// \returns
///     An internal flag describing the current DFA state.
//--------------------------------------------------------------------------------------------------
//
KEWB_FORCE_INLINE int32_t UtfUtils::AdvanceWithSmallTable(
    char8_t const*& pSrc, char8_t const* const pSrcEnd,
    char32_t& cdpt) noexcept {
  char32_t unit;  //- The current UTF-8 code unit
  int32_t type;   //- The current code unit's character class
  int32_t curr;   //- The current DFA state

  unit = *pSrc++;  //- Cache the first code unit
  type =
      smTables
          .maOctetCategory[unit];  //- Get the first code unit's character class
  cdpt = smTables.maFirstOctetMask[type] & unit;  //- Apply the first octet mask
  curr = smTables.maTransitions[type];            //- Look up the second state

  while (curr > ERR) {
    if (pSrc < pSrcEnd) {
      unit = *pSrc++;  //- Cache the current code unit
      cdpt = (cdpt << 6) |
             (unit & 0x3F);  //- Adjust code point with continuation bits
      type = smTables.maOctetCategory[unit];       //- Look up the code unit's
                                                   // character class
      curr = smTables.maTransitions[curr + type];  //- Look up the next state
    } else {
      return ERR;
    }
  }
  return curr;
}

}  // namespace uu
#endif  //- KEWB_UNICODE_UTILS_H_DEFINED