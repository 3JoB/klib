#include "klib/html.h"

#include <memory>

#include <tidy.h>
#include <tidybuffio.h>

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
std::string html_tidy(const std::string &html) {
  if (std::empty(html)) {
    return "";
  }

  auto free_tidy_doc = [](TidyDoc *doc) { tidyRelease(*doc); };
  TidyDoc temp_doc = tidyCreate();
  std::unique_ptr<TidyDoc, decltype(free_tidy_doc)> doc(&temp_doc,
                                                        free_tidy_doc);

  check_opt_set(tidyOptSetBool(*doc, TidyXhtmlOut, yes));
  check_opt_set(tidyOptSetBool(*doc, TidyShowWarnings, no));
  check_opt_set(tidyOptSetInt(*doc, TidyWrapLen, 0));
  check_opt_set(tidyOptSetBool(*doc, TidyHideComments, yes));
  check_opt_set(tidyOptSetBool(*doc, TidyMark, no));

  auto free_tidy_buffer = [](TidyBuffer *buffer) { tidyBufFree(buffer); };
  TidyBuffer temp_output = {};
  std::unique_ptr<TidyBuffer, decltype(free_tidy_buffer)> output(
      &temp_output, free_tidy_buffer);

  TidyBuffer temp_error_buffer = {};
  std::unique_ptr<TidyBuffer, decltype(free_tidy_buffer)> error_buffer(
      &temp_error_buffer, free_tidy_buffer);

  auto rc = tidySetErrorBuffer(*doc, error_buffer.get());
  if (rc >= 0) {
    rc = tidyParseString(*doc, html.c_str());
  }
  if (rc >= 0) {
    rc = tidyCleanAndRepair(*doc);
  }
  if (rc >= 0) {
    rc = tidyRunDiagnostics(*doc);
  }
  if (rc > 1) {
    check_opt_set(tidyOptSetBool(*doc, TidyForceOutput, yes));
  }

  rc = tidySaveBuffer(*doc, output.get());

  std::string xhtml;
  if (rc >= 0) {
    if (rc > 1) {
      throw RuntimeError("{}",
                         reinterpret_cast<const char *>(error_buffer->bp));
    }
    xhtml.assign(reinterpret_cast<const char *>(output->bp), output->size);
  } else {
    throw RuntimeError("html_tidy error: {}", rc);
  }

  return xhtml;
}

}  // namespace klib
