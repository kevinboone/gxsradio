/*=============================================================================

  gxsradio
  stringutil.h

  General utility functions for GString manipulation

=============================================================================*/

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

gboolean stringutil_string_ends_with (const GString *s, const char *end);
  /**
  A helper function for replacing the value of a GString. If the GString is not
  null on entry, it is freed first. Then, if the value argument is non-null, a
  new GString is created with the new value.  Replacing a string with NULL
  effectively frees it.
  */
void     stringutil_replace_value (GString **s, const char *value);

  /** 
  Give a string representing a pathname, return a new string with just a
  filename. If the path ends in a separator, then and empty string will be
  returned. 
  */
GString *stringutil_get_filename (const char *path);

  /** Taking 'path' as a directory, which may or many not end in a 
      separator, appends 'file'. The separator is inserted if
      necessary */
void stringutil_append_to_path (GString *path, const char *file);

  /** Returns true if the string ends with the filesystem separator
      character */
gboolean stringutil_string_ends_with_sep (const GString *s);

G_END_DECLS




