/*==============================================================================
  
  gxsradio
  BoilerplateModel.c

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
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include "BoilerplateModel.h"

#define BOILERPLATEMODEL_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          BOILERPLATEMODEL_TYPE, BoilerplateModelPriv))

typedef struct _BoilerplateModelPriv BoilerplateModelPriv;

struct _BoilerplateModelPriv
  {
  char *current_file;
  };

G_DEFINE_TYPE (BoilerplateModel, boilerplatemodel, G_TYPE_OBJECT);

/*=============================================================================
  _boilerplatemodel_need_save
=============================================================================*/
gboolean _boilerplatemodel_need_save (const BoilerplateModel *self)
  {
  return FALSE;
  }

/*=============================================================================
  _boilerplatemodel_need_save
=============================================================================*/
gboolean boilerplatemodel_need_save (const BoilerplateModel *self)
  {
  gboolean need_save = BOILERPLATEMODEL_GET_CLASS (self)->need_save (self);
  return need_save;
  }


/*=============================================================================
  boilerplatemodel_do_open_file
=============================================================================*/
gboolean boilerplatemodel_do_open_file 
     (BoilerplateModel *self, const char *file, char **error)
   {
   g_error ("boilerplatemodel::do_open_file()"
       " has not been implemented");
   *error = strdup ("Open file is not implemented in boilerplate");
   return FALSE;
   }


/*=============================================================================
  boilerplatemodel_do_close_file
=============================================================================*/
void boilerplatemodel_do_close_file (BoilerplateModel *self)
   {
   g_error ("boilerplatemodel::do_close_file()"
       " has not been implemented");
   }


/*=============================================================================
  boilerplatemodel_do_save_file
=============================================================================*/
gboolean boilerplatemodel_do_save_file 
     (BoilerplateModel *self, const char *file, char **error)
   {
   g_error ("boilerplatemodel::do_save_file()"
       " has not been implemented");
   *error = strdup ("Save file is not implemented in boilerplate");
   return FALSE;
   }

/*=============================================================================
  boilerplatemodel_do_save_current_file
=============================================================================*/
gboolean boilerplatemodel_do_save_current_file 
     (BoilerplateModel *self, char **error)
   {
   BoilerplateModelPriv *priv = BOILERPLATEMODEL_PRIV (self);
   if (priv->current_file == NULL)
      g_error ("Internal error: %s with NULL current filename", 
         __PRETTY_FUNCTION__);
   return boilerplatemodel_do_save_file (self, priv->current_file, error);
   }

/*=============================================================================
  boilerplatemodel_get_current_filename
=============================================================================*/
const char *boilerplatemodel_get_current_filename (const BoilerplateModel *self)
  {
  BoilerplateModelPriv *priv = BOILERPLATEMODEL_PRIV (self);
  return priv->current_file;
  }

/*=============================================================================
  boilerplatemodel_set_current_filename
=============================================================================*/
void boilerplatemodel_set_current_filename (BoilerplateModel *self, 
       const char *filename)
  {
  BoilerplateModelPriv *priv = BOILERPLATEMODEL_PRIV (self);
  if (priv->current_file) free (priv->current_file);
  priv->current_file = NULL;
  if (filename)
    priv->current_file = strdup (filename);
  else
    priv->current_file = NULL; 
  }

/*=============================================================================
  boilerplatemodel_setup_open_dialog
=============================================================================*/
void boilerplatemodel_setup_open_dialog (const BoilerplateModel *self,
     GtkFileChooserDialog *dialog)
  {
  GtkFileFilter *fz = gtk_file_filter_new ();
  gtk_file_filter_set_name (fz, "Text files (*.txt)");
  gtk_file_filter_add_pattern (fz, "*.txt");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), fz);

  GtkFileFilter *fall = gtk_file_filter_new ();
  gtk_file_filter_set_name (fall, "All files");
  gtk_file_filter_add_pattern (fall, "*");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), fall);
  }

/*=============================================================================
  boilerplatemodel_init
=============================================================================*/
static void boilerplatemodel_init (BoilerplateModel *self)
  {
  g_debug ("%s: intializing controller", __PRETTY_FUNCTION__);

  BoilerplateModelPriv *priv = BOILERPLATEMODEL_PRIV (self);
  priv->current_file = NULL;

  g_debug ("%s: intialized controller", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  boilerplatemodel_dispose
=============================================================================*/
static void boilerplatemodel_dispose (GObject *self)
  {
  BoilerplateModelPriv *priv = BOILERPLATEMODEL_PRIV (self);
  if (priv->current_file) free (priv->current_file);
  priv->current_file = NULL;
  G_OBJECT_CLASS (boilerplatemodel_parent_class)->dispose (self);
  }

/*=============================================================================
  boilerplatemodel_class_init
=============================================================================*/
static void boilerplatemodel_class_init
  (BoilerplateModelClass *klass)
  {
  g_debug ("%s: initializing class", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (BoilerplateModelPriv));
  klass->do_open_file = boilerplatemodel_do_open_file;
  klass->do_close_file = boilerplatemodel_do_close_file;
  klass->do_save_file = boilerplatemodel_do_save_file;
  klass->do_save_current_file = boilerplatemodel_do_save_current_file;
  klass->need_save = _boilerplatemodel_need_save;
  klass->setup_open_dialog = boilerplatemodel_setup_open_dialog;
  G_OBJECT_CLASS (klass)->dispose = boilerplatemodel_dispose;
  g_debug ("%s: initialized class", __PRETTY_FUNCTION__);
  }


/*=============================================================================
  boilerplatemodel_new
=============================================================================*/
BoilerplateModel *boilerplatemodel_new (void)
  {
  g_debug ("%s: calling g_object_new", __PRETTY_FUNCTION__);
  BoilerplateModel *self = g_object_new 
     (BOILERPLATEMODEL_TYPE, NULL);
  g_debug ("%s: called g_object_new", __PRETTY_FUNCTION__);
  return self;
  }

