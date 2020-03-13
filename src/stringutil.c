/*==============================================================================
  
  gsxradio
  stringutil.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the 
  GPL, v3.0

  General utility functions for GString manipulation

==============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include "stringutil.h" 

#ifdef WINDOWS
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif

/*==============================================================================
  
  string_append_to_path

==============================================================================*/
void stringutil_append_to_path (GString *path, const char *file)
  {
  if (!stringutil_string_ends_with_sep (path))
    g_string_append_c (path, SEPARATOR);
  g_string_append (path, file);
  }

/*==============================================================================
  
  string_ends_with

==============================================================================*/
gboolean stringutil_string_ends_with (const GString *s, const char *end)
  {
  gboolean ret = FALSE;
  const char *p = strstr (s->str, end);
  if (p)
    {
    if (p - s->str == strlen (s->str) - strlen (end))
      ret = TRUE; 
    } 
  return ret;
  }

/*==============================================================================
  
  string_ends_with_sep

==============================================================================*/
gboolean stringutil_string_ends_with_sep (const GString *s)
  {
  static char s_sep[] = { SEPARATOR, 0 };
  return stringutil_string_ends_with (s, s_sep);;
  }

/*==============================================================================
  
  replace_value 

==============================================================================*/
void stringutil_replace_value (GString **s, const char *value)
  {
  if (*s)
    {
    g_string_free (*s, TRUE);
    *s = NULL;
    }
  if (value)
    {
    *s = g_string_new (value);
    }
  }

/*==============================================================================
  
  get_filename 

==============================================================================*/
GString *stringutil_get_filename (const char *path)
  {
  GString *ret = NULL;
  char *sep = strrchr (path, SEPARATOR);
  if (sep)
    {
    ret = g_string_new (sep + 1);
    }
  else
    {
    ret = g_string_new (path);
    }
  return ret;
  }



