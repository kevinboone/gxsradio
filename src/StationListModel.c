/*==============================================================================
  
  gsxradio 
  StationListModel.c

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
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include "StationListModel.h"

#define STATIONLISTMODEL_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          STATIONLISTMODEL_TYPE, StationListModelPriv))

typedef struct _StationListModelPriv StationListModelPriv;

struct _StationListModelPriv
  {
  GtkListStore *list_model;
  gboolean dirty;
  };

G_DEFINE_TYPE (StationListModel, stationlistmodel, BOILERPLATEMODEL_TYPE);

/*======================================================================
  stationlistmodel_need_save
======================================================================*/
gboolean stationlistmodel_need_save (const BoilerplateModel *self)
  {
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  return priv->dirty;
  }

/*======================================================================
  stationlistmodel_do_close_file
======================================================================*/
void stationlistmodel_do_close_file (BoilerplateModel *self)
   {
   StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
   boilerplatemodel_set_current_filename (self, NULL);
   gtk_list_store_clear (priv->list_model);
   priv->dirty = FALSE;
   }

/*======================================================================
  stationlistmodel_do_open_file
======================================================================*/
gboolean stationlistmodel_do_open_file 
     (BoilerplateModel *self, const char *file, char **error)
   {
   g_debug ("%s: %s", __PRETTY_FUNCTION__, file);
   StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
   gboolean ret = FALSE;
   FILE *f = fopen (file, "r");
   if (f)
     {
     gtk_list_store_clear (priv->list_model);
     int n;
     int num = 1;
     int read = 0;
     GtkTreeIter   iter;
     do
       {
       char *line = NULL;
       size_t zero = 0;
       n = getline (&line, &zero, f); 
       if (n >= 2)
         {
	 line[n - 1] = 0;

         const char *name = strtok (line, "\t");
         const char *country = strtok (NULL, "\t");
         const char *uri = strtok (NULL, "\t");
         const char *s_ok = strtok (NULL, "\t");
         if (name && uri && s_ok)
	   {
           read++;
           gtk_list_store_append (priv->list_model, &iter);
           gtk_list_store_set (priv->list_model, &iter,
             NAME_COLUMN, name,
             COUNTRY_COLUMN, country,
             URI_COLUMN, uri,
             SELECTED_COLUMN, atoi (s_ok),
             -1);
	   }
	 else
	   {
	   printf ("error %d\n", num);
	   g_warning ("%s: wrong number of tokens on line %d", 
	     __PRETTY_FUNCTION__, num);
	   }
	 num++;
	 }
       if (line) free (line);
       } while (n >= 2);
     if (read > 0)
       {
       g_debug ("%s: read %d entries", 
	     __PRETTY_FUNCTION__, read);
       ret = TRUE;
       }
     else
       {
       asprintf (error, "No valid data in file %s", file);
       ret = FALSE;
       }
     priv->dirty = FALSE;
     fclose (f);
     }
   else
     {
     asprintf (error, "Can't read file '%s': %s", file, strerror (errno));
     ret = FALSE;
     } 
   if (ret)
     boilerplatemodel_set_current_filename (self, file);
   return ret;
   }

/*======================================================================
  stationlistmodel_count_selected
  Count the number of selected lines
======================================================================*/
int stationlistmodel_count_selected
     (const BoilerplateModel *self)
   {
   g_debug ("%s", __PRETTY_FUNCTION__);
   StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
   int ret = 0;

   GtkTreeIter iter;
   GtkTreeModel *list_store = GTK_TREE_MODEL (priv->list_model); 
   if (gtk_tree_model_get_iter_first (list_store, &iter))
     {
     do
       {
       gboolean ok;
       gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);
       if (ok) ret++;
       } while (gtk_tree_model_iter_next (list_store, &iter));
     }
   return ret;
   }

/*======================================================================
  stationlistmodel_do_save_selected
  Save only selected lines to file
======================================================================*/
gboolean stationlistmodel_do_save_selected
     (const BoilerplateModel *self, const char *file, char **error)
   {
   g_debug ("%s: %s", __PRETTY_FUNCTION__, file);
   StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
   gboolean ret = FALSE;
   FILE *f = fopen (file, "w");
   if (f)
     {
     GtkTreeIter iter;
     GtkTreeModel *list_store = GTK_TREE_MODEL (priv->list_model); 
     if (gtk_tree_model_get_iter_first (list_store, &iter))
       {
       do
         {
	 gchar *name;
	 gchar *uri;
	 gchar *country;
	 gboolean ok;
         gtk_tree_model_get (list_store, &iter, NAME_COLUMN, &name, -1);
         gtk_tree_model_get (list_store, &iter, COUNTRY_COLUMN, &country, -1);
         gtk_tree_model_get (list_store, &iter, URI_COLUMN, &uri, -1);
         gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);

         if (ok)
           fprintf (f, "%s\t%s\t%s\t%d\n", name, country, uri, ok);

         free (name);
	 free (uri);
	 free (country);

	 } while (gtk_tree_model_iter_next (list_store, &iter));
       }
     else
       {
       g_warning ("Saving an empty list");
       }

     fclose (f);
     ret = TRUE;
     }
   else
     {
     asprintf (error, "Can't write file '%s': %s", file, strerror (errno));
     ret = FALSE;
     } 
   return ret;
   }

/*======================================================================
  stationlistmodel_select_none
======================================================================*/
void  stationlistmodel_select_none (StationListModel *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  GtkTreeIter iter;
  GtkTreeModel *list_store = GTK_TREE_MODEL (priv->list_model); 
  if (gtk_tree_model_get_iter_first (list_store, &iter))
    {
    do
      {
      //gboolean ok;
      //gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);
      gtk_list_store_set (GTK_LIST_STORE (list_store), &iter, 
        SELECTED_COLUMN, 0, -1);

      } while (gtk_tree_model_iter_next (list_store, &iter));
   }
 }

/*======================================================================
  stationlistmodel_select_all
======================================================================*/
void  stationlistmodel_select_all (StationListModel *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  GtkTreeIter iter;
  GtkTreeModel *list_store = GTK_TREE_MODEL (priv->list_model); 
  if (gtk_tree_model_get_iter_first (list_store, &iter))
    {
    do
      {
      //gboolean ok;
      //gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);
      gtk_list_store_set (GTK_LIST_STORE (list_store), &iter, 
        SELECTED_COLUMN, 1, -1);

      } while (gtk_tree_model_iter_next (list_store, &iter));
   }
 }

/*======================================================================
  stationlistmodel_select_invert
======================================================================*/
void  stationlistmodel_select_invert (StationListModel *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  GtkTreeIter iter;
  GtkTreeModel *list_store = GTK_TREE_MODEL (priv->list_model); 
  if (gtk_tree_model_get_iter_first (list_store, &iter))
    {
    do
      {
      gboolean ok;
      gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);
      gtk_list_store_set (GTK_LIST_STORE (list_store), &iter, 
        SELECTED_COLUMN, ok ? 0 : 1, -1);

      } while (gtk_tree_model_iter_next (list_store, &iter));
   }
 }

/*======================================================================
  stationlistmodel_do_save_file
======================================================================*/
gboolean stationlistmodel_do_save_file 
     (BoilerplateModel *self, const char *file, char **error)
   {
   g_debug ("%s: %s", __PRETTY_FUNCTION__, file);
   StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
   gboolean ret = FALSE;
   FILE *f = fopen (file, "w");
   if (f)
     {
     GtkTreeIter iter;
     GtkTreeModel *list_store = GTK_TREE_MODEL (priv->list_model); 
     if (gtk_tree_model_get_iter_first (list_store, &iter))
       {
       do
         {
	 gchar *name;
	 gchar *uri;
	 gchar *country;
	 gboolean ok;
         gtk_tree_model_get (list_store, &iter, NAME_COLUMN, &name, -1);
         gtk_tree_model_get (list_store, &iter, COUNTRY_COLUMN, &country, -1);
         gtk_tree_model_get (list_store, &iter, URI_COLUMN, &uri, -1);
         gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);

         fprintf (f, "%s\t%s\t%s\t%d\n", name, country, uri, ok);

         free (name);
	 free (uri);
	 free (country);

	 } while (gtk_tree_model_iter_next (list_store, &iter));
       }
     else
       {
       g_warning ("Saving an empty list");
       }

     priv->dirty = FALSE;
     fclose (f);
     ret = TRUE;
     }
   else
     {
     asprintf (error, "Can't write file '%s': %s", file, strerror (errno));
     ret = FALSE;
     } 
   if (ret)
     boilerplatemodel_set_current_filename (self, file);
   return ret;
   }


/*======================================================================
  stationlistmodel_do_save_current_file
======================================================================*/
gboolean stationlistmodel_do_save_current_file 
     (BoilerplateModel *self, char **error)
   {
   g_debug ("%s", __PRETTY_FUNCTION__);
   char *current_filename
     = strdup (boilerplatemodel_get_current_filename (self));
   gboolean ret = stationlistmodel_do_save_file (self, 
     current_filename, error);
   free (current_filename);
   return ret;
   }


/*======================================================================
  stationlistmodel_get_buffer
======================================================================*/
GtkListStore *stationlistmodel_get_list_model (const StationListModel *self)
  {
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  return priv->list_model;
  }

/*======================================================================
  stationlistmodel_set_need_save
======================================================================*/
void stationlistmodel_set_need_save (StationListModel *self)
  {
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  priv->dirty = TRUE;
  }

/*=============================================================================
  stationlistmodel_setup_open_dialog
=============================================================================*/
void _stationlistmodel_setup_open_dialog (const BoilerplateModel *self,
         GtkFileChooserDialog *dialog)
  {
  GtkFileFilter *fz = gtk_file_filter_new ();
  gtk_file_filter_set_name (fz, "gxsradio files (*.gxsradio)");
  gtk_file_filter_add_pattern (fz, "*.gxsradio");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), fz);

  GtkFileFilter *fall = gtk_file_filter_new ();
  gtk_file_filter_set_name (fall, "All files");
  gtk_file_filter_add_pattern (fall, "*");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), fall);
  }

/*=============================================================================
 stationlistmodel_add_station
=============================================================================*/
void stationlistmodel_add_station (StationListModel *self, const char *name, 
       const char *uri, const char *country, gboolean selected)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  GtkTreeIter   iter;
  gtk_list_store_append (priv->list_model, &iter);
  gtk_list_store_set (priv->list_model, &iter,
             NAME_COLUMN, name,
             COUNTRY_COLUMN, country,
             URI_COLUMN, uri,
             SELECTED_COLUMN, selected,
             -1);
  priv->dirty = TRUE;
  }


/*=============================================================================
 stationlistmodel_dispose 
=============================================================================*/
static void stationlistmodel_dispose (GObject *self)
  {
  G_OBJECT_CLASS (stationlistmodel_parent_class)->dispose (self);
  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  if (priv->list_model)
    g_object_unref (priv->list_model);
  priv->list_model = NULL; 
  }

/*=============================================================================
  stationlistmodel_init
==============================================================================*/
static void stationlistmodel_init (StationListModel *self)
  {
  g_debug ("%s: intializing stationlistmodel", __PRETTY_FUNCTION__);

  StationListModelPriv *priv = STATIONLISTMODEL_PRIV (self);
  
  priv->dirty = FALSE;

  priv->list_model = gtk_list_store_new 
    (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

  g_debug ("%s: intialized stationlistmodel", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  stationlistmodel_class_init
==============================================================================*/
static void stationlistmodel_class_init
  (StationListModelClass *klass)
  {
  g_debug ("%s: initializing class", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (StationListModelPriv));
  BoilerplateModelClass *parent = 
     BOILERPLATEMODEL_CLASS (klass);
  parent->do_save_file = stationlistmodel_do_save_file;
  parent->do_save_current_file = stationlistmodel_do_save_current_file;
  parent->do_open_file = stationlistmodel_do_open_file;
  parent->do_close_file = stationlistmodel_do_close_file;
  parent->setup_open_dialog = _stationlistmodel_setup_open_dialog;
  parent->need_save = stationlistmodel_need_save;
  G_OBJECT_CLASS (klass)->dispose = stationlistmodel_dispose;
  g_debug ("%s: initialized class", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  stationlistmodel_new
==============================================================================*/
StationListModel *stationlistmodel_new (void)
  {
  g_debug ("%s: calling g_object_new", __PRETTY_FUNCTION__);
  StationListModel *self = g_object_new 
     (STATIONLISTMODEL_TYPE, NULL);
  g_debug ("%s: called g_object_new", __PRETTY_FUNCTION__);
  return self;
  }

