/**
 * @see https://www.html-tidy.org/developer/
 */

#include "klib/html.h"

#include <tidy.h>
#include <tidybuffio.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

#define check_html_tidy(rc)                            \
  do {                                                 \
    if (!rc) {                                         \
      throw RuntimeError("HTML Tidy option set fail"); \
    }                                                  \
  } while (0)

namespace klib {

std::string html_tidy(const std::string &html, bool ignore_error) {
  TidyDoc doc = tidyCreate();
  SCOPE_EXIT { tidyRelease(doc); };

  auto ret = tidyOptSetBool(doc, TidyXhtmlOut, yes);
  check_html_tidy(ret);

  ret = tidyOptSetBool(doc, TidyShowWarnings, no);
  check_html_tidy(ret);

  ret = tidyOptSetInt(doc, TidyWrapLen, 0);
  check_html_tidy(ret);

  ret = tidyOptSetBool(doc, TidyHideComments, yes);
  check_html_tidy(ret);

  ret = tidyOptSetBool(doc, TidyMark, no);
  check_html_tidy(ret);

  TidyBuffer output_buffer = {};
  SCOPE_EXIT { tidyBufFree(&output_buffer); };

  TidyBuffer error_buffer = {};
  SCOPE_EXIT { tidyBufFree(&error_buffer); };

  auto rc = tidySetErrorBuffer(doc, &error_buffer);
  if (rc >= 0) {
    rc = tidyParseString(doc, html.c_str());
  }
  if (rc >= 0) {
    rc = tidyCleanAndRepair(doc);
  }
  if (rc >= 0) {
    rc = tidyRunDiagnostics(doc);
  }
  if (rc > 1) {
    ret = tidyOptSetBool(doc, TidyForceOutput, yes);
    check_html_tidy(ret);
  }

  rc = tidySaveBuffer(doc, &output_buffer);

  std::string xhtml;
  if (rc >= 0) {
    if (rc > 1 && !ignore_error) {
      throw RuntimeError("{}", reinterpret_cast<const char *>(error_buffer.bp));
    }
    xhtml.assign(reinterpret_cast<const char *>(output_buffer.bp),
                 output_buffer.size);
  } else {
    throw RuntimeError("html_tidy error: {}", rc);
  }

  return xhtml;
}

}  // namespace klib
