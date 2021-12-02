#include "klib/html.h"

#include <tidy.h>
#include <tidybuffio.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

namespace klib {

namespace {

void check_opt_set(bool ok) {
  if (!ok) {
    throw RuntimeError("option set fail");
  }
}

}  // namespace

// https://www.html-tidy.org/developer/
std::string html_tidy(const std::string &html, bool ignore_error) {
  TidyDoc doc = tidyCreate();
  SCOPE_EXIT { tidyRelease(doc); };

  check_opt_set(tidyOptSetBool(doc, TidyXhtmlOut, yes));
  check_opt_set(tidyOptSetBool(doc, TidyShowWarnings, no));
  check_opt_set(tidyOptSetInt(doc, TidyWrapLen, 0));
  check_opt_set(tidyOptSetBool(doc, TidyHideComments, yes));
  check_opt_set(tidyOptSetBool(doc, TidyMark, no));

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
    check_opt_set(tidyOptSetBool(doc, TidyForceOutput, yes));
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
