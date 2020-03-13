/*==============================================================================
  
  gxsradio 
  BoilerplateController.c

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
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "BoilerplateController.h"
#include "BoilerplateApplication.h"
#include "boilerplatedialogs.h"
#include "BoilerplateMainWindow.h"

#define BOILERPLATECONTROLLER_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          BOILERPLATECONTROLLER_TYPE, BoilerplateControllerPriv))

typedef struct _BoilerplateControllerPriv BoilerplateControllerPriv;

struct _BoilerplateControllerPriv
  {
  BoilerplateModel *model;
  BoilerplateApplication *application;
  BoilerplateMainWindow *main_window;
  };

G_DEFINE_TYPE (BoilerplateController, boilerplatecontroller, G_TYPE_OBJECT);

typedef enum
  {
  PROP_MAIN_WINDOW = 1,
  PROP_APPLICATION,
  PROP_MODEL,
  N_PROPERTIES
  } BoilerplateControllerProperty;


/*=============================================================================
  _boilerplatecontroller_prompt_for_open_filename

  Prompt for a filename. Returns TRUE if the user selects a file,
  and sets **filename, which the caller must free. If the user
  does not select anything, returns FALSE, and **is not set.

  Note that this method does not actually open anything -- it just
  prompts.
==============================================================================*/
static gboolean _boilerplatecontroller_prompt_for_open_filename
     (BoilerplateController *self, char **filename)
  {
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  gboolean ret = FALSE;
  g_debug ("%s: about to open file dialog", __PRETTY_FUNCTION__);

  GtkFileChooserDialog *d = GTK_FILE_CHOOSER_DIALOG 
    (gtk_file_chooser_dialog_new ("Open File",
    GTK_WINDOW (priv->main_window),
    GTK_FILE_CHOOSER_ACTION_OPEN,
    _("Cancel"), GTK_RESPONSE_CANCEL,
    _("_Open"), GTK_RESPONSE_ACCEPT,
    NULL));

  gtk_widget_set_size_request (GTK_WIDGET(d), 800, 400);
  
  BOILERPLATEMODEL_GET_CLASS (priv->model)->setup_open_dialog (priv->model, d);

  if (gtk_dialog_run (GTK_DIALOG (d)) == GTK_RESPONSE_ACCEPT)
    {
    *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (d));
    gtk_widget_destroy (GTK_WIDGET(d));
    ret = TRUE;
    }
  else
    {
    gtk_widget_destroy (GTK_WIDGET(d));
    ret = FALSE;
    }

  return ret;
  }

/*=============================================================================
  boilerplatecontroller_prompt_for_open_filename
=============================================================================*/
gboolean boilerplatecontroller_prompt_for_open_filename
     (BoilerplateController *self, char **filename)
  {
  return BOILERPLATECONTROLLER_GET_CLASS (self)
       ->prompt_for_open_filename (self, filename);
  }

/*=============================================================================
  _boilerplatecontroller_prompt_for_save_filename

  Prompt for a filename. Returns TRUE if the user selects a file,
  and sets **filename, which the caller must free. If the user
  does not select anything, returns FALSE, and **is not set.

  Note that this method does not actually open anything -- it just
  prompts.
==============================================================================*/
gboolean _boilerplatecontroller_prompt_for_save_filename 
     (BoilerplateController *self, char **filename)
  {
  g_debug ("%s: about to open file dialog", __PRETTY_FUNCTION__);
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);

  gboolean ret = FALSE;

  *filename = NULL;

  GtkFileChooserDialog *d = GTK_FILE_CHOOSER_DIALOG 
    (gtk_file_chooser_dialog_new ("Save File",
    GTK_WINDOW (priv->main_window),
    GTK_FILE_CHOOSER_ACTION_SAVE,
    _("Cancel"), GTK_RESPONSE_CANCEL,
    _("_Save"), GTK_RESPONSE_ACCEPT,
    NULL));

  gtk_widget_set_size_request (GTK_WIDGET(d), 800, 400);

  const char *current_filename 
     = boilerplatemodel_get_current_filename (priv->model);
  if (current_filename)
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (d), current_filename);
  
  BOILERPLATEMODEL_GET_CLASS (priv->model)->setup_open_dialog (priv->model, d);

  gboolean done = FALSE;
  while (!done)
    {
    if (gtk_dialog_run (GTK_DIALOG (d)) == GTK_RESPONSE_ACCEPT)
      {
      *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (d));
      if (access (*filename, R_OK) == 0)
        {
	if (boilerplatedialogs_yesno 
	     (GTK_WINDOW(priv->main_window), NAME, 
	       "Overwrite this file?", GTK_RESPONSE_NO)
	       == GTK_RESPONSE_YES)
	  {
	  done = TRUE;
          ret = TRUE;
	  }
	else
	  {
	  done = FALSE;
	  }
	}
      else
        {
	ret = TRUE;
	done = TRUE;
	}
      }
    else
      {
      ret = FALSE;
      done = TRUE;
      }
    }
  gtk_widget_destroy (GTK_WIDGET(d));

  if (!ret)
    {
    if (*filename) free (*filename);
    *filename = NULL;
    }

  return ret;
  }

/*=============================================================================
  boilerplatecontroller_prompt_for_save_filename
=============================================================================*/
gboolean boilerplatecontroller_prompt_for_save_filename
     (BoilerplateController *self, char **filename)
  {
  return BOILERPLATECONTROLLER_GET_CLASS (self)
       ->prompt_for_save_filename (self, filename);
  }

/*=============================================================================
  boilerplatecontroller_save_to_current_file

  Saves the current data to the current name, which is assumed
  to be set.  Report errors via the 
  GUI, but also return a value to indcate whether the data actually
  got saved
==============================================================================*/
static gboolean boilerplatecontroller_save_to_current_file
         (BoilerplateController *self)
  {
  gboolean ret = FALSE;
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  char *error = NULL;
  if (BOILERPLATEMODEL_GET_CLASS (priv->model)->do_save_current_file 
         (priv->model, &error))
    {
    // OK
    ret = TRUE;
    }
  else
    {
    const char *filename = boilerplatemodel_get_current_filename 
               (priv->model); 
    g_debug ("%s: save file error: %s, %s", __PRETTY_FUNCTION__, 
      filename, error);
    boilerplatedialogs_error (GTK_WINDOW(priv->main_window), NAME, error);
    free (error);
    ret = FALSE;
    }
  return ret;
  }

/*=============================================================================
  boilerplatecontroller_save_file

  Saves the current data to the specified name. Report errors via the 
  GUI, but also return a value to indcate whether the data actually
  got saved
==============================================================================*/
static gboolean boilerplatecontroller_save_file
         (BoilerplateController *self, const char *filename)
  {
  gboolean ret = FALSE;
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  char *error = NULL;
  if (BOILERPLATEMODEL_GET_CLASS (priv->model)->do_save_file 
         (priv->model, filename, &error))
    {
    // OK
    ret = TRUE;
    boilerplatemainwindow_model_file_changed (priv->main_window);
    }
  else
    {
    g_debug ("%s: save file error: %s, %s", __PRETTY_FUNCTION__, 
      filename, error);
    boilerplatedialogs_error (GTK_WINDOW(priv->main_window), NAME, error);
    free (error);
    ret = FALSE;
    }
  return ret;
  }

/*=============================================================================
  boilerplatecontroller_save_data

  Saves the current data, either to the current file if there is one, or
  by prompting for a new filename. Returns TRUE if all data is saved
==============================================================================*/
static gboolean boilerplatecontroller_save_data
         (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  const char *current_file = boilerplatemodel_get_current_filename 
               (priv->model); 
  gboolean did_save = FALSE;
  if (current_file == NULL)
    {
    g_debug ("%s: no filename; will prompt", __PRETTY_FUNCTION__);
    char *filename = NULL;
    if (boilerplatecontroller_prompt_for_save_filename (self, &filename))
      {
      did_save = boilerplatecontroller_save_file (self, filename);
      boilerplatemainwindow_model_file_changed (priv->main_window);
      }
    else
      {
      g_debug ("%s: user did not select a filename", __PRETTY_FUNCTION__);
      did_save = FALSE;
      }
    }
  else 
    {
    did_save = boilerplatecontroller_save_to_current_file (self);
    // NO need to update UI -- nothing has changed 
    }
  return did_save;
  }

/*=============================================================================
  boilerplatecontroller_prepare_to_close

  Get ready to close the current file. If there is no data to save,
  just exit. If there is data to save, then save it, either to the
  current filename if there is one, or by prompting to get a new filename.
  Returns TRUE if it is OK to proceed to close the file (possibly losing
  changes if the user agreed to that).
==============================================================================*/
static gboolean boilerplatecontroller_prepare_to_close 
         (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  gboolean ret = TRUE;
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  if (BOILERPLATEMODEL_GET_CLASS (priv->model)->need_save (priv->model))
    {
    g_debug ("%s: data needs save", __PRETTY_FUNCTION__);

    gint response = boilerplatedialogs_yesnocancel 
      (GTK_WINDOW(priv->main_window), 
      NAME, "Data has been modified. Save?", GTK_RESPONSE_YES);

    switch (response)
      {
      case GTK_RESPONSE_YES:
        if (boilerplatecontroller_save_data (self))
          {
          ret = TRUE; 
          }
        else
          {
          ret = FALSE;
          }
        break;
      case GTK_RESPONSE_NO: ret = TRUE; break; // Don't save
      default: ret = FALSE; // Assume the user did not want to close 
      }
    }
  else
    {
    g_debug ("%s: data not changed, can close file", __PRETTY_FUNCTION__);
    ret = TRUE;
    }

  return ret;
  }

/*=============================================================================
  boilerplatecontroller_close_file
  Closes the current file without further prompting. Update the UI
  to indicate the change of status
==============================================================================*/
void boilerplatecontroller_close_file (BoilerplateController *self)
  {
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  BOILERPLATEMODEL_GET_CLASS (priv->model)->do_close_file (priv->model);
  boilerplatemainwindow_model_file_changed (priv->main_window);
  }

/*=============================================================================
  boilerplatecontroller_prepare_and_close
  Calls prepare to close to see it is is OK to close. If it is, closes
  the current file and updates the UI to indicate the change of status.
==============================================================================*/
gboolean boilerplatecontroller_prepare_and_close (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  gboolean ret = FALSE;
  if (boilerplatecontroller_prepare_to_close (self))
    {
    boilerplatecontroller_close_file (self);
    ret = TRUE;
    }
  else
    ret = FALSE;
  return ret;
  }

/*=============================================================================
  boilerplatecontroller_action_open_file
==============================================================================*/
void boilerplatecontroller_action_open_file (BoilerplateController *self,
       const char *filename)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  char *error = NULL;
  if (BOILERPLATEMODEL_GET_CLASS (priv->model)->do_open_file 
       (priv->model, filename, &error))
    {
    boilerplatemainwindow_model_file_changed (priv->main_window);
    }
  else
    {
    g_debug ("%s: open file error: %s, %s", __PRETTY_FUNCTION__, 
       filename, error);
    boilerplatedialogs_error (GTK_WINDOW(priv->main_window), NAME, error);
    free (error);
    }
  }

/*=============================================================================
  boilerplatecontroller_action_prompt_open_file
==============================================================================*/
void boilerplatecontroller_action_prompt_open_file (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  char *filename = NULL;
  if (BOILERPLATECONTROLLER_GET_CLASS (self)
       ->prompt_for_open_filename (self, &filename))
    {
    if (boilerplatecontroller_prepare_and_close (self))
      {
      boilerplatecontroller_action_open_file (self, filename);
      }
    else
      {
      g_debug ("%s: not prepared to close", __PRETTY_FUNCTION__);
      }
    }
  }

/*=============================================================================
  boilerplatecontroller_prepare_to_quit
==============================================================================*/
gboolean boilerplatecontroller_prepare_to_quit (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  return boilerplatecontroller_prepare_to_close (self);
  }

/*=============================================================================
  boilerplatecontroller_action_new_file
==============================================================================*/
void boilerplatecontroller_action_new_file (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  boilerplatecontroller_prepare_and_close (self);
  }

/*=============================================================================
  boilerplatecontroller_statusbar_message
==============================================================================*/
void boilerplatecontroller_statusbar_message (const BoilerplateController *self,
    const char *message)
  {
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  boilerplatemainwindow_statusbar_message (priv->main_window, message);
  }


/*=============================================================================
  _boilerplatecontroller_action_about
==============================================================================*/
void _boilerplatecontroller_action_about (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  boilerplatecontroller_statusbar_message (self, "BUGGER IT");

  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  const char *about_msg = 
     BOILERPLATEAPPLICATION_GET_CLASS (priv->application)
      ->get_about_msg (priv->application);
  boilerplatedialogs_message (GTK_WINDOW(priv->main_window), 
      NAME, about_msg);
  }

/*=============================================================================
  boilerplatecontroller_action_save_current_file
==============================================================================*/
void boilerplatecontroller_action_save_current_file 
     (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  boilerplatecontroller_save_data (self);
  }

/*=============================================================================
  boilerplatecontroller_action_save_file
==============================================================================*/
void boilerplatecontroller_action_save_file (BoilerplateController *self,
       const char *filename)
  {
  g_debug ("%s: %s", __PRETTY_FUNCTION__, filename);
  boilerplatecontroller_save_file (self, filename);
  }

/*=============================================================================
  boilerplatecontroller_action_prompt_save_file
==============================================================================*/
void boilerplatecontroller_action_prompt_save_file (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  char *filename = NULL;
  if (boilerplatecontroller_prompt_for_save_filename (self, &filename))
    {
    boilerplatecontroller_save_file (self, filename);
    }
  else
    {
    g_debug ("%s: user did not select a filename", __PRETTY_FUNCTION__);
    }
  }

/*=============================================================================
  _boilerplatecontroller_action_close_window
==============================================================================*/
void _boilerplatecontroller_action_close_window (BoilerplateController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  if (boilerplatecontroller_prepare_and_close (self))
    {
    gtk_widget_destroy (GTK_WIDGET(priv->main_window));
    }
  }

/*============================================================================
  boilerplatecontroller_get_main_window
=======================================================i=====================*/
struct _BoilerplateMainWindow
    *boilerplatecontroller_get_main_window (const BoilerplateController *self)
  {
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  return priv->main_window;
  }

/*=============================================================================
  boilerplatecontroller_get_model
=============================================================================*/
BoilerplateModel *boilerplatecontroller_get_model 
    (const BoilerplateController *self)
  {
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  return priv->model;
  }

/*=============================================================================
  boilerplatecontroller_get_application
=============================================================================*/
BoilerplateApplication *boilerplatecontroller_get_application
    (const BoilerplateController *self)
  {
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  return priv->application;
  }

/*======================================================================
  boilerplatecontroller_set_property
======================================================================*/
static void _boilerplatecontroller_set_property (GObject *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
  {
  BoilerplateController *self = BOILERPLATECONTROLLER (object);
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);

  switch ((BoilerplateControllerProperty) property_id)
    {
    case PROP_MAIN_WINDOW:
      priv->main_window = g_value_get_object (value); 
      break;
    case PROP_APPLICATION:
      priv->application = g_value_get_object (value); 
      break;
    case PROP_MODEL:
      priv->model = g_value_get_object (value); 
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
  }

/*======================================================================
  boilerplatecontroller_get_property
======================================================================*/
static void _boilerplatecontroller_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
  {
  BoilerplateController *self = BOILERPLATECONTROLLER (object);
  BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  switch (property_id)
    {
    case PROP_MAIN_WINDOW:
      g_value_set_object (value, priv->main_window);
      break;
    case PROP_APPLICATION:
      g_value_set_object (value, priv->application);
      break;
    case PROP_MODEL:
      g_value_set_object (value, priv->model);
      break;
    }
  }

/*=============================================================================
  boilerplatecontroller_init
==============================================================================*/
 static void boilerplatecontroller_init (BoilerplateController *self)
  {
  g_debug ("%s: intializing controller", __PRETTY_FUNCTION__);

  //BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);

  g_debug ("%s: intialized controller", __PRETTY_FUNCTION__);
  }

/*======================================================================
  boilerplatecontroller_dispose
======================================================================*/
static void boilerplatecontroller_dispose (GObject *self)
  {
  //BoilerplateControllerPriv *priv = BOILERPLATECONTROLLER_PRIV (self);
  G_OBJECT_CLASS (boilerplatecontroller_parent_class)->dispose (self);
  }

/*=============================================================================
  boilerplatecontroller_class_init
==============================================================================*/
static void boilerplatecontroller_class_init
  (BoilerplateControllerClass *klass)
  {
  g_debug ("%s: initializing class", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (BoilerplateControllerPriv));
  G_OBJECT_CLASS (klass)->set_property =  _boilerplatecontroller_set_property;
  G_OBJECT_CLASS (klass)->get_property = _boilerplatecontroller_get_property;

  g_object_class_install_property (G_OBJECT_CLASS (klass), 
      PROP_MAIN_WINDOW, 
      g_param_spec_object ("main-window", "foo", "foo", 
      BOILERPLATEMAINWINDOW_TYPE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
 
  g_object_class_install_property (G_OBJECT_CLASS (klass), 
      PROP_APPLICATION, 
      g_param_spec_object ("application", "foo", "foo", 
      BOILERPLATEAPPLICATION_TYPE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
 
  g_object_class_install_property (G_OBJECT_CLASS (klass), 
      PROP_MODEL, 
      g_param_spec_object ("model", "foo", "foo", 
      BOILERPLATEMODEL_TYPE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
 
  klass->action_open_file = boilerplatecontroller_action_open_file;
  klass->action_save_file = boilerplatecontroller_action_save_file;
  klass->action_new_file = boilerplatecontroller_action_new_file;
  klass->action_save_current_file = 
      boilerplatecontroller_action_save_current_file;
  klass->action_prompt_open_file = boilerplatecontroller_action_prompt_open_file;
  klass->action_prompt_save_file = boilerplatecontroller_action_prompt_save_file;
  klass->action_about = _boilerplatecontroller_action_about;
  klass->action_close_window = _boilerplatecontroller_action_close_window;
  klass->action_about = _boilerplatecontroller_action_about;
  klass->prompt_for_open_filename = 
     _boilerplatecontroller_prompt_for_open_filename;
  klass->prompt_for_save_filename = 
     _boilerplatecontroller_prompt_for_save_filename;
  G_OBJECT_CLASS (klass)->dispose = boilerplatecontroller_dispose;
  g_debug ("%s: initialized class", __PRETTY_FUNCTION__);
  }


/*=============================================================================
  boilerplatecontroller_new
==============================================================================*/
BoilerplateController *boilerplatecontroller_new 
     (BoilerplateMainWindow *w, BoilerplateApplication *application,
      BoilerplateModel *model)
  {
  g_debug ("%s: calling g_object_new", __PRETTY_FUNCTION__);
  BoilerplateController *self = g_object_new 
     (BOILERPLATECONTROLLER_TYPE, "main-window", w, 
      "application", application, "model", model, NULL);
  g_debug ("%s: called g_object_new", __PRETTY_FUNCTION__);
  return self;
  }

