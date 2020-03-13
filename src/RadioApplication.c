/*=============================================================================
  
  gxsradio 
  RadioApplication.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include "RadioApplication.h"
#include "StationListWindow.h"
#include "StationListController.h"
#include "stringutil.h"

#define RADIOAPPLICATION_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          RADIOAPPLICATION_TYPE, RadioApplicationPriv))

#define RC_CONNECTION_GROUP "connection"
#define RC_HOST_KEY "host"
#define RC_PORT_KEY "port"

#define RC_UI_GROUP "ui"
#define RC_FONT_KEY "font"
#define RC_SINGLE_KEY "single"

#define RC_FILES_GROUP "files"
#define RC_LASTFILE_KEY "last-file"

typedef struct _RadioApplicationPriv RadioApplicationPriv;

struct _RadioApplicationPriv
  {
  GString *host;
  int port;
  GString *font_name;
  GString *last_pathname;
  GString *css_filename;
  GString *fixed_directory;
  gboolean readonly;
  gboolean single; // Single-click mode
  };

G_DEFINE_TYPE (RadioApplication, radioapplication, BOILERPLATEAPPLICATION_TYPE);

/*=============================================================================
  radioapplication_is_readonly
=============================================================================*/
gboolean radioapplication_is_readonly (const RadioApplication *self)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  return priv->readonly;
  }

/*=============================================================================
  _radioapplication_first_idle
=============================================================================*/
void _radioapplication_first_idle (BoilerplateApplication *self)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  BoilerplateMainWindow *main_window 
       = boilerplateapplication_get_main_window (self);

  BoilerplateController *_controller 
      = boilerplatemainwindow_get_controller (main_window);

  StationListController *controller = STATIONLISTCONTROLLER (_controller);

  if (priv->css_filename)
    {
    GtkCssProvider *cssp = gtk_css_provider_new();
    GError *error = NULL;
    gtk_css_provider_load_from_path (cssp, priv->css_filename->str, &error);
    gtk_style_context_add_provider_for_screen
         (gdk_screen_get_default(),
         GTK_STYLE_PROVIDER (cssp),
         GTK_STYLE_PROVIDER_PRIORITY_USER);
    if (error)
      {
      g_warning (error->message);
      g_error_free (error);
      }
    g_object_unref (cssp); // Do we need this?
    }

  if (priv->font_name)
    {
    char *font_name = strdup (priv->font_name->str);
    radioapplication_set_font_name (RADIOAPPLICATION (self), 
       font_name);
    free (font_name);
    }

 BoilerplateModel *model = boilerplatecontroller_get_model (_controller);

 const char *current_filename = boilerplatemodel_get_current_filename 
    (model);

 // If the curent model has a filename at this point, it was passed on
 //   the command line. In that case, we should not attempt to load
 //   the last file (even if there was one)
 if (current_filename)
   {
   g_debug ("%s: not attempting to load the last filename", 
      __PRETTY_FUNCTION__);
   }
 else
   {
   g_debug ("%s: attempting to load the last filename", __PRETTY_FUNCTION__);
   if (priv->last_pathname)
     {
     char *last_pathname = strdup (priv->last_pathname->str);
     g_debug ("%s: last filename was %s", __PRETTY_FUNCTION__, last_pathname); 
     BOILERPLATECONTROLLER_GET_CLASS (controller)
       ->action_open_file (_controller, last_pathname);
     free (last_pathname);
     }
   else
     {
     g_debug ("%s: no last filename -- not loading anything", 
       __PRETTY_FUNCTION__);
     }
   } 

  if (priv->host && priv->host->len > 0 && priv->port > 1024)
    {
    char *host = strdup (priv->host->str);
    STATIONLISTCONTROLLER_GET_CLASS (controller)
       ->action_connect (controller, host, priv->port);
    free (host);
    }
  else
    {
    STATIONLISTCONTROLLER_GET_CLASS (controller)
       ->action_prompt_connect (controller);
    }
  }

/*=============================================================================
  _radioapplication_parse_options
==============================================================================*/
gboolean _radioapplication_parse_options
         (BoilerplateApplication *self, int *argc, char ***argv, char **error, 
	 gboolean *run_program)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);

  gboolean ret = TRUE;
  *run_program = TRUE;

  static gboolean version = FALSE;
  static gboolean readonly = FALSE;
  static gboolean single = FALSE;
  static char *font_name = NULL;
  static char *host = NULL;
  static char *css_filename = NULL;
  static char *fixed_directory = NULL;
  static int port = 0;

  static GOptionEntry entries[] =
    {
    { "css", 'c', 0, G_OPTION_ARG_STRING, &css_filename, 
        "CSS theme file", NULL},
    { "fixed-dir", 'd', 0, G_OPTION_ARG_STRING, &fixed_directory, 
        "Fixed station file directory", NULL},
    { "font", 'f', 0, G_OPTION_ARG_STRING, &font_name, "Font name", NULL},
    { "host", 'h', 0, G_OPTION_ARG_STRING, &host, "Server host", NULL},
    { "port", 'p', 0, G_OPTION_ARG_INT, &port, "Server port", NULL},
    { "readonly", 'r', 0, G_OPTION_ARG_NONE, &readonly, "Read only mode", NULL},
    { "single", 's', 0, G_OPTION_ARG_NONE, &single, "Single-click mode", NULL},
    { "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Show version", NULL},
    { NULL }
    };
  GOptionContext *context = g_option_context_new ("[filename]");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_set_help_enabled (context, TRUE); 
  GError *e = NULL;
  if (g_option_context_parse (context, argc, argv, &e))
    {
    if (readonly)
      priv->readonly = TRUE;
    ret = TRUE;
    if (version)
      {
      const char *msg = BOILERPLATEAPPLICATION_GET_CLASS (self)
          ->get_about_msg (self);
      printf ("%s", msg);
      *run_program = FALSE;
      }
    if (*run_program)
      {
      // No point parsing anything else, if we aren't actually going to
      //   run the GUI
      if (host)
        {
	stringutil_replace_value (&priv->host, host);
        free (host);
        }
      if (font_name)
        {
	stringutil_replace_value (&priv->font_name, font_name);
        free (font_name);
        }
      if (css_filename)
        {
	stringutil_replace_value (&priv->css_filename, css_filename);
        free (font_name);
        }
      if (fixed_directory)
        {
	stringutil_replace_value (&priv->fixed_directory, fixed_directory);
        free (fixed_directory);
        }
      if (single)
        {
	priv->single = TRUE;
	}
      if (port != 0) priv->port = port;
      }
    }
  else
    {
    *error = strdup (e->message);
    g_error_free (e);
    ret = FALSE;
    }
  g_option_context_free (context);
  return ret;
  }

/*=============================================================================
  radioapplication_save_user_rc
==============================================================================*/
void radioapplication_save_user_rc (const RadioApplication *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
 
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);

  if (!priv->readonly)
    {
    char *user_rc_file 
      = boilerplateapplication_get_user_rc_file (BOILERPLATEAPPLICATION (self));

    g_debug ("%s: %s", __PRETTY_FUNCTION__, user_rc_file);

    FILE *f = fopen (user_rc_file, "w");
    if (f)
      {
      fprintf (f, "[%s]\n", RC_CONNECTION_GROUP);
      if (priv->host)
	fprintf (f, "%s=%s\n", RC_HOST_KEY, priv->host->str);
      if (priv->port > 0)
	fprintf (f, "%s=%d\n", RC_PORT_KEY, priv->port);
      fprintf (f, "[%s]\n", RC_UI_GROUP);
      if (priv->font_name)
	fprintf (f, "%s=%s\n", RC_FONT_KEY, priv->font_name->str);
      fprintf (f, "%s=%d\n", RC_SINGLE_KEY, priv->single);
      fprintf (f, "[%s]\n", RC_FILES_GROUP);
      if (priv->last_pathname)
	fprintf (f, "%s=%s\n", RC_LASTFILE_KEY, priv->last_pathname->str);
      fclose (f);
      }
    else
      g_warning ("Can't open user RC file %s for writing", user_rc_file);

    free (user_rc_file);
    }
  }

/*=============================================================================
  _radioapplication_read_rc_file
==============================================================================*/
void _radioapplication_read_rc_file (const BoilerplateApplication *self,
        const char *rc_filename)
  {
  g_debug ("%s: %s", __PRETTY_FUNCTION__, rc_filename);
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  GKeyFile *keyfile = g_key_file_new ();
  if (g_key_file_load_from_file (keyfile, rc_filename, G_KEY_FILE_NONE, NULL))
    {
    char *host = g_key_file_get_string 
      (keyfile, RC_CONNECTION_GROUP, RC_HOST_KEY, NULL);

    stringutil_replace_value (&priv->host, host);

    priv->port = g_key_file_get_integer 
      (keyfile, RC_CONNECTION_GROUP, RC_PORT_KEY, NULL);

    char *font_name = g_key_file_get_string 
      (keyfile, RC_UI_GROUP, RC_FONT_KEY, NULL);
    if (font_name)
      {
      stringutil_replace_value (&priv->font_name, font_name);
      }

    gboolean single = g_key_file_get_boolean 
      (keyfile, RC_UI_GROUP, RC_SINGLE_KEY, FALSE);
    priv->single = single; 

    char *last_pathname = g_key_file_get_string 
      (keyfile, RC_FILES_GROUP, RC_LASTFILE_KEY, NULL);
    if (last_pathname)
      {
      stringutil_replace_value (&priv->last_pathname, last_pathname);
      }

    if (host) free (host);
    if (font_name) free (font_name);
    }
  g_key_file_free (keyfile);
  }


/*=============================================================================
  radioapplication_set_single_click_mode
==============================================================================*/
void radioapplication_set_single_click_mode (RadioApplication *self, 
        gboolean single)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  priv->single = single;
  BoilerplateMainWindow *main_window 
       = boilerplateapplication_get_main_window (BOILERPLATEAPPLICATION (self));
  stationlistwindow_set_single_click_mode (STATIONLISTWINDOW (main_window), 
       single);
  }

/*=============================================================================
  radioapplication_get_single_click_mode
==============================================================================*/
gboolean radioapplication_get_single_click_mode (const RadioApplication *self)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  return priv->single;
  }

/*=============================================================================
  radioapplication_get_host
==============================================================================*/
const char *radioapplication_get_host (const RadioApplication *self)
  {
  const char *ret = NULL;
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  if (priv->host)
    ret = priv->host->str;
  return ret;
  }

/*=============================================================================
  radioapplication_get_font_name
==============================================================================*/
const char *radioapplication_get_font_name (const RadioApplication *self)
  {
  const char *ret = NULL;
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  if (priv->font_name)
    ret = priv->font_name->str;
  return ret;
  }


/*=============================================================================
  radioapplication_get_port
==============================================================================*/
int radioapplication_get_port (const RadioApplication *self)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  return priv->port;
  }

/*=============================================================================
  radioapplication_set_port
==============================================================================*/
void radioapplication_set_port (RadioApplication *self, int port)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  priv->port = port;
  }

/*=============================================================================
  radioapplication_set_host
==============================================================================*/
void radioapplication_set_host (RadioApplication *self, const char *host)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  stringutil_replace_value (&priv->host, host);
  }

/*=============================================================================
  radioapplication_set_last_pathname
==============================================================================*/
void radioapplication_set_last_pathname (RadioApplication *self, 
       const char *pathname)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  stringutil_replace_value (&priv->last_pathname, pathname);
  }

/*=============================================================================
  radioapplication_get_last_pathname
==============================================================================*/
const char *radioapplication_get_last_pathname (const RadioApplication *self)
  {
  const char *ret = NULL;
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  if (priv->last_pathname)
    ret = priv->last_pathname->str;
  return ret;
  }

/*=============================================================================
  radioapplication_get_fixed_directory
==============================================================================*/
const char *radioapplication_get_fixed_directory (const RadioApplication *self)
  {
  const char *ret = NULL;
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  if (priv->fixed_directory)
    ret = priv->fixed_directory->str;
  return ret;
  }

/*=============================================================================
  radioapplication_set_font
==============================================================================*/
void radioapplication_set_font_name (RadioApplication *self, 
    const char *font_name)
  {
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  stringutil_replace_value (&priv->font_name, font_name);
  GtkCssProvider *cssp = gtk_css_provider_new();
      char *css;
      asprintf (&css, "* {font: %s}", font_name);
  gtk_css_provider_load_from_data (cssp, css, -1, NULL);
  gtk_style_context_add_provider_for_screen
       (gdk_screen_get_default(),
       GTK_STYLE_PROVIDER (cssp),
       GTK_STYLE_PROVIDER_PRIORITY_USER);
  free (css); 
  g_object_unref (cssp); // Do we need this?
  }

/*=============================================================================
  _radioapplication_create_main_window
=============================================================================*/
static BoilerplateMainWindow *_radioapplication_create_main_window 
      (BoilerplateApplication *self)
  {
  g_debug ("%s: start", __PRETTY_FUNCTION__);

  //RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);

  StationListWindow *w =  stationlistwindow_new ();
  BOILERPLATEMAINWINDOW_GET_CLASS (BOILERPLATEMAINWINDOW (w))->setup 
       (BOILERPLATEMAINWINDOW(w), self);

  gtk_window_set_default_size (GTK_WINDOW (w), 700, 400);
  //gtk_window_fullscreen (GTK_WINDOW (w));

  g_debug ("%s: end", __PRETTY_FUNCTION__);

  return BOILERPLATEMAINWINDOW (w);
  }

/*=============================================================================
  radioapplication_dispose
=============================================================================*/
static void radioapplication_dispose (GObject *self)
  {
  G_OBJECT_CLASS (radioapplication_parent_class)->dispose (self);
  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV 
    (RADIOAPPLICATION (self));
  stringutil_replace_value (&priv->host, NULL);
  stringutil_replace_value (&priv->font_name, NULL);
  stringutil_replace_value (&priv->last_pathname, NULL);
  stringutil_replace_value (&priv->fixed_directory, NULL);
  }

/*=============================================================================
  radioapplication_init
=============================================================================*/
static void radioapplication_init (RadioApplication *self)
  {
  g_debug ("%s: intializing radioapplication", __PRETTY_FUNCTION__);

  RadioApplicationPriv *priv = RADIOAPPLICATION_PRIV (self);
  priv->host = NULL; 
  priv->font_name = NULL; 
  priv->last_pathname = NULL; 
  priv->readonly = FALSE; 
  priv->fixed_directory = NULL; 

  g_debug ("%s: intialized radioapplication", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  radioapplication_class_init
=============================================================================*/
static void radioapplication_class_init
  (RadioApplicationClass *klass)
  {
  g_debug ("%s: intializing class", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (RadioApplicationPriv));
  BoilerplateApplicationClass *parent = 
     BOILERPLATEAPPLICATION_CLASS (klass);
  parent->create_main_window = _radioapplication_create_main_window;
  parent->read_rc_file = _radioapplication_read_rc_file;
  parent->first_idle = _radioapplication_first_idle;
  parent->parse_options = _radioapplication_parse_options;
  G_OBJECT_CLASS (klass)->dispose = radioapplication_dispose;
  g_debug ("%s: intialized class", __PRETTY_FUNCTION__);
  }


/*=============================================================================
  radioapplication_new
=============================================================================*/
RadioApplication *radioapplication_new (void)
  {
  g_debug ("%s: calling g_object_new", __PRETTY_FUNCTION__);
  RadioApplication *self = g_object_new 
     (RADIOAPPLICATION_TYPE, NULL);
  g_debug ("%s: called g_object_new", __PRETTY_FUNCTION__);
  return self;
  }



