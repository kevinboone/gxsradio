/*==============================================================================
  
  gxsradio 
  BoilerplateApplication.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the 
  GPL, v3.0

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
#include <glib/gi18n.h>
#include "BoilerplateApplication.h"
#include "BoilerplateMainWindow.h"
#include "stringutil.h"

#define BOILERPLATEAPPLICATION_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          BOILERPLATEAPPLICATION_TYPE, BoilerplateApplicationPriv))

typedef struct _BoilerplateApplicationPriv BoilerplateApplicationPriv;

struct _BoilerplateApplicationPriv
  {
  BoilerplateMainWindow *main_window;
  };

G_DEFINE_TYPE (BoilerplateApplication, boilerplateapplication, G_TYPE_OBJECT);

/*=============================================================================
  _boilerplateapplication_create_main_window
=============================================================================*/
static BoilerplateMainWindow *_boilerplateapplication_create_main_window 
      (BoilerplateApplication *self)
  {
  g_debug ("%s: start", __PRETTY_FUNCTION__);

  BoilerplateMainWindow *w =  boilerplatemainwindow_new ();
  BOILERPLATEMAINWINDOW_CLASS (w)->setup (w, self);
  gtk_window_set_default_size (GTK_WINDOW (w), 200, 200);

  g_debug ("%s: end", __PRETTY_FUNCTION__);

  return w;
  }

/*=============================================================================
  _boilerplateapplication_get_user_rc_file
==============================================================================*/
char *_boilerplateapplication_get_user_rc_file 
         (const BoilerplateApplication *self)
  {
  // TODO -- Windows
  GString *user_rc = g_string_new (getenv ("HOME"));
  if (!stringutil_string_ends_with (user_rc, "/"))
    g_string_append (user_rc, "/");
  g_string_append (user_rc, "." NAME ".rc");
  char *ret = user_rc->str;
  g_string_free (user_rc, FALSE);
  return ret;
  }

/*=============================================================================
  boilerplateapplication_get_user_rc_file
==============================================================================*/
char *boilerplateapplication_get_user_rc_file 
        (const BoilerplateApplication *self)
  {
  char *user_rc = BOILERPLATEAPPLICATION_GET_CLASS (self)
    ->get_user_rc_file (self);
  return user_rc;
  }

/*=============================================================================
  _boilerplateapplication_get_system_rc_file
==============================================================================*/
char *_boilerplateapplication_get_system_rc_file 
         (const BoilerplateApplication *self)
  {
  // TODO -- Windows
  GString *system_rc = g_string_new ("/etc/");
  g_string_append (system_rc, NAME ".rc");
  char *ret = system_rc->str;
  g_string_free (system_rc, FALSE);
  return ret;
  }

/*=============================================================================
  _boilerplateapplication_parse_options
==============================================================================*/
gboolean _boilerplateapplication_parse_options
         (BoilerplateApplication *self, int *argc, char ***argv, char **error, 
	 gboolean *run_program)
  {
  gboolean ret = TRUE;
  *run_program = TRUE;

  static gboolean version = FALSE;
  static GOptionEntry entries[] =
    {
    { "version", 0, 0, G_OPTION_ARG_NONE, &version, "Show version", NULL},
    { NULL }
    };
  GOptionContext *context = g_option_context_new ("[filename]");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_set_help_enabled (context, TRUE); 
  GError *e = NULL;
  if (g_option_context_parse (context, argc, argv, &e))
    {
    ret = TRUE;
    if (version)
      {
      const char *msg = BOILERPLATEAPPLICATION_GET_CLASS (self)
          ->get_about_msg (self);
      printf ("%s", msg);
      *run_program = FALSE;
      }
    }
  else
    {
    *error = strdup (e->message);
    g_error_free (e);
    ret = FALSE;
    }
  return ret;
  }

/*=============================================================================
  boilerplateapplication_get_main_window
==============================================================================*/
BoilerplateMainWindow *boilerplateapplication_get_main_window
       (const BoilerplateApplication *self)
  {
  BoilerplateApplicationPriv *priv = BOILERPLATEAPPLICATION_PRIV (self);
  return priv->main_window;
  }

/*=============================================================================
  _boilerplateapplication_get_about_msg
==============================================================================*/
const char *_boilerplateapplication_get_about_msg 
       (const BoilerplateApplication *self)
  {
  const char *about_msg = "" NAME " version " VERSION "\n"
     "Copyright (c)2020 Kevin Boone\n"
     "Distributed under the terms of the GNU Public Licence, v3.0\n";
  return about_msg;
  }

/*=============================================================================
  boilerplateapplication_get_about_msg
==============================================================================*/
const char *boilerplateapplication_get_about_msg 
       (const BoilerplateApplication *self)
  {
  return BOILERPLATEAPPLICATION_GET_CLASS (self)->get_about_msg(self);
  }

/*=============================================================================
  boilerplateapplication_shutdown
==============================================================================*/
void boilerplateapplication_shutdown (GtkWidget *w, gpointer user_data)
  {
  gtk_main_quit();
  }

/*=============================================================================
  boilerplateapplication_read_rc_file
==============================================================================*/
void _boilerplateapplication_read_rc_file (const BoilerplateApplication *self,
        const char *rc_filename)
  {
  g_debug ("%s: %s", __PRETTY_FUNCTION__, rc_filename);
  GKeyFile *keyfile = g_key_file_new ();
  if (g_key_file_load_from_file (keyfile, rc_filename, G_KEY_FILE_NONE, NULL))
    {
    g_warning 
      ("RC file %s exists, but no implementation was provided to read it",
      rc_filename);
    }
  g_key_file_free (keyfile);
  }

/*=============================================================================
  boilerplateapplication_read_rc_files
==============================================================================*/
void _boilerplateapplication_read_rc_files (BoilerplateApplication *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  char *system_rc = BOILERPLATEAPPLICATION_GET_CLASS (self)
    ->get_system_rc_file (self);
  BOILERPLATEAPPLICATION_GET_CLASS (self)->read_rc_file (self, system_rc);
  free (system_rc);

  char *user_rc = BOILERPLATEAPPLICATION_GET_CLASS (self)
    ->get_user_rc_file (self);
  BOILERPLATEAPPLICATION_GET_CLASS (self)->read_rc_file (self, user_rc);
  free (user_rc);
  }

/*=============================================================================
  _boilerplateapplication_first_idle
==============================================================================*/
void _boilerplateapplication_first_idle (BoilerplateApplication *self)
  {
  // No implementation, unless a sub-class provides it
  }

/*=============================================================================
  boilerplateapplication_first_idle_internal
==============================================================================*/
int boilerplateapplication_first_idle_internal (gpointer data)
  {
  BoilerplateApplication *self = BOILERPLATEAPPLICATION (data);
  BOILERPLATEAPPLICATION_GET_CLASS (self)->first_idle (self);
  return 0;
  }

/*=============================================================================
  boilerplateapplication_run
==============================================================================*/
int boilerplateapplication_run (BoilerplateApplication *self, 
      int *_argc, char ***_argv)
  {
  g_debug ("%s: start", __PRETTY_FUNCTION__);

  g_debug ("%s: calling gtk_init", __PRETTY_FUNCTION__);
  gtk_init (_argc, _argv);
  g_debug ("%s: called gtk_init", __PRETTY_FUNCTION__);
  
  gboolean run = TRUE;

  BOILERPLATEAPPLICATION_GET_CLASS (self)->read_rc_files (self);

  if (run)
    {
    char *error = NULL;
    if (BOILERPLATEAPPLICATION_GET_CLASS (self)->parse_options 
       (self, _argc, _argv, &error, &run))
      {
      }
    else
      {
      g_warning (error);
      free (error);
      run = FALSE;
      }
    }
 
  if (run)
    {
    BoilerplateApplicationPriv *priv = BOILERPLATEAPPLICATION_PRIV (self);
    priv->main_window = BOILERPLATEAPPLICATION_GET_CLASS (self)
	      ->create_main_window (self);

    // TODO -- make this shutdown virtual, so subclasses can do
    //   further shutdown operations if necessary
    g_signal_connect (G_OBJECT (priv->main_window), "destroy", 
         G_CALLBACK (boilerplateapplication_shutdown), NULL);

    gtk_widget_show_all (GTK_WIDGET(priv->main_window));

    int argc = *_argc;
    char **argv = *_argv;

    if (argc >= 2)
      {
      BoilerplateController *controller 
	  = boilerplatemainwindow_get_controller (priv->main_window);
      const char *filename = argv[1];
      BOILERPLATECONTROLLER_GET_CLASS (controller)
       ->action_open_file (controller, filename);
      }

/*
  GtkCssProvider *cssp = gtk_css_provider_new();
  // TODO
  gtk_css_provider_load_from_data (cssp,
    "stationlistwindow {font-size: 20px} ", -1, NULL);
  gtk_style_context_add_provider_for_screen
                               (gdk_screen_get_default(),
                                GTK_STYLE_PROVIDER (cssp),
                               GTK_STYLE_PROVIDER_PRIORITY_USER);
*/

    g_idle_add (boilerplateapplication_first_idle_internal, self);
    gtk_main ();

    }

  g_debug ("%s: end", __PRETTY_FUNCTION__);
  return 0;
  }

/*=============================================================================
  boilerplateapplication_init
==============================================================================*/
static void boilerplateapplication_init (BoilerplateApplication *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  }

/*======================================================================
  _boilerplateapplication_dispose
======================================================================*/
static void _boilerplateapplication_dispose (GObject *self)
  {
  //BoilerplateApplicationPriv *priv = BOILERPLATEAPPLICATION_PRIV (self);
  G_OBJECT_CLASS (boilerplateapplication_parent_class)->dispose (self);
  }

/*=============================================================================
  boilerplateapplication_class_init
==============================================================================*/
static void boilerplateapplication_class_init
  (BoilerplateApplicationClass *klass)
  {
  g_debug ("%s: initializing class", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (BoilerplateApplicationPriv));
  klass->create_main_window = _boilerplateapplication_create_main_window;
  klass->parse_options = _boilerplateapplication_parse_options;
  klass->read_rc_files = _boilerplateapplication_read_rc_files;
  klass->get_about_msg = _boilerplateapplication_get_about_msg;
  klass->get_user_rc_file = _boilerplateapplication_get_user_rc_file;
  klass->get_system_rc_file = _boilerplateapplication_get_system_rc_file;
  klass->first_idle = _boilerplateapplication_first_idle;
  G_OBJECT_CLASS (klass)->dispose = _boilerplateapplication_dispose;
  g_debug ("%s: initialized class", __PRETTY_FUNCTION__);
  }


/*=============================================================================
  boilerplateapplication_new
==============================================================================*/
BoilerplateApplication *boilerplateapplication_new (void)
  {
  g_debug ("%s: calling g_object_new", __PRETTY_FUNCTION__);
  BoilerplateApplication *self = g_object_new 
     (BOILERPLATEAPPLICATION_TYPE, NULL);
  g_debug ("%s: called g_object_new", __PRETTY_FUNCTION__);
  return self;
  }

