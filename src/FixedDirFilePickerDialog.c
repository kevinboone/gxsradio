/*=============================================================================
  
  gxsradio
  FixedDirFilePickerDialog.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <string.h>
#include "FixedDirFilePickerDialog.h"
#include "boilerplatedialogs.h"
#include "stringutil.h"

#define FIXEDDIRFILEPICKERDIALOG_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          FIXEDDIRFILEPICKERDIALOG_TYPE, FixedDirFilePickerDialogPriv))

enum
   {
   NAME_COLUMN = 0,
   N_COLUMNS
   };

#define COLUMN_0_TITLE "Name"
typedef struct _FixedDirFilePickerDialogPriv FixedDirFilePickerDialogPriv;

struct _FixedDirFilePickerDialogPriv
  {
  GtkListStore *list_model;
  GtkTreeView *tree_view;
  GtkEntry *filename_entry;
  FixedDirFilePickerType type;
  };

G_DEFINE_TYPE (FixedDirFilePickerDialog, fixeddirfilepickerdialog, GTK_TYPE_DIALOG);

/*======================================================================
  fixeddirfilepickerdialog_response_event
======================================================================*/
static void fixeddirfilepickerdialog_response_event (GtkDialog *d,
  int id, gpointer user_data)
  {
  /*
  FixedDirFilePickerDialog *self = (FixedDirFilePickerDialog *)d;
  if (id == GTK_RESPONSE_OK)
    {
    printf ("foo\n");
    }
  */
  }


/*=============================================================================
  fixeddirfilepickerdialog_list_files
=============================================================================*/
static GList *fixeddirfilepickerdialog_list_files (const char *dir)
  {
  GList *ret = NULL;
  DIR *d = opendir (dir);
  if (d)
     {
     struct dirent *de = readdir (d);
     while (de)
        {
        if (strstr (de->d_name, ".gxsradio")) // TODO -- check at end
          ret = g_list_append (ret, strdup (de->d_name));
        de = readdir (d);
        }
     closedir (d);
     }
  else
    g_warning ("%s: Can't open directory %s: %s", __PRETTY_FUNCTION__,
      dir, strerror (errno));
  return ret;
  }

/*=============================================================================
  fixeddirfilepickerdialog_run
=============================================================================*/
int fixeddirfilepickerdialog_run (const FixedDirFilePickerDialog *self, 
     const char *fixed_dir, char **pathname)
  {
  FixedDirFilePickerDialogPriv *priv = FIXEDDIRFILEPICKERDIALOG_PRIV (self);

  if (priv->type == FIXEDDIRPICKER_SAVE)
    gtk_window_set_title (GTK_WINDOW (self), NAME ": save file");

  GList *files = fixeddirfilepickerdialog_list_files (fixed_dir); 
  GtkTreeIter   iter;
  for (GList *l = files; l != NULL; l = l->next)
    {
    const char *file = (const char *)(l->data);

    gtk_list_store_append (priv->list_model, &iter);
    gtk_list_store_set (priv->list_model, &iter,
             NAME_COLUMN, file, -1);
    }
  g_list_free_full (files, free);
  gtk_widget_show_all (GTK_WIDGET (self));

  int response;
  gboolean valid;
  do
    {
    valid = FALSE;
    response = gtk_dialog_run (GTK_DIALOG (self));
    if (response == GTK_RESPONSE_OK)
      {
      const char *filename = gtk_entry_get_text (priv->filename_entry);
      // TODO? -- sanitize filename to remove ".", "/", etc
      if (filename && filename[0])
        {
        GString *s_path = g_string_new (fixed_dir); 
	stringutil_append_to_path (s_path, filename);
        *pathname = s_path->str;
	g_string_free (s_path, FALSE);

	if (priv->type == FIXEDDIRPICKER_SAVE)
	  {
          if (access (*pathname, R_OK) == 0)
            {
	    if (boilerplatedialogs_yesno 
	          (GTK_WINDOW(self), NAME, 
	           "Overwrite this file?", GTK_RESPONSE_NO)
	         == GTK_RESPONSE_YES)
	       valid = TRUE;
	     else 
	       {
	       free (*pathname);
	       valid = FALSE;
	       }
	     }
	  else
	    valid = TRUE;
	  }
	else
	  valid = TRUE;
        }
      else 
        {
	boilerplatedialogs_error (GTK_WINDOW(self), NAME,
	  "Please select a file");
        valid = FALSE;
        }

      if (valid)
        {
        // TODO
        }
      }
    } while (response == GTK_RESPONSE_OK && !valid);

  gtk_widget_destroy (GTK_WIDGET (self));
  

  return response;
  }

/*=============================================================================
  fixeddirfilepickerdialog_foo
=============================================================================*/
void fixeddirfilepickerdialog_foo (GtkTreeView *w, 
        gpointer data)
  {
  FixedDirFilePickerDialog *self = FIXEDDIRFILEPICKERDIALOG (data);
  FixedDirFilePickerDialogPriv *priv = FIXEDDIRFILEPICKERDIALOG_PRIV (self);
  GtkTreeIter iter;
  GtkTreeModel *model;

  GtkTreeSelection *selection = gtk_tree_view_get_selection 
           (priv->tree_view);
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
    gchar *name;
    gtk_tree_model_get (model, &iter, NAME_COLUMN, &name, -1);
    gtk_entry_set_text (priv->filename_entry, name); 
    free (name);
    }
  }


/*=============================================================================
  fixeddirfilepickerdialog_on_row_activated 
=============================================================================*/
void fixeddirfilepickerdialog_on_row_activated (GtkTreeView *w, 
        GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
  {
  gtk_dialog_response (GTK_DIALOG (data), GTK_RESPONSE_OK);
  }

/*=============================================================================
  fixeddirfilepickerdialog_init
=============================================================================*/
static void fixeddirfilepickerdialog_init (FixedDirFilePickerDialog *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  FixedDirFilePickerDialogPriv *priv = FIXEDDIRFILEPICKERDIALOG_PRIV (self);

  gtk_window_set_title (GTK_WINDOW (self), NAME ": open file"); 

  gtk_dialog_add_buttons (GTK_DIALOG (self), // TODO
       "_Cancel", GTK_RESPONSE_CANCEL, "_OK", GTK_RESPONSE_OK, NULL);

  GtkBox *content = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (self)));
  GtkGrid *content_box = GTK_GRID (gtk_grid_new ());
  gtk_grid_set_row_spacing (content_box, 10);
  gtk_widget_set_margin_start (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_end (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_top (GTK_WIDGET(content_box), 0);
  gtk_widget_set_margin_bottom (GTK_WIDGET(content_box), 0);

  gtk_widget_set_halign (GTK_WIDGET(content_box), GTK_ALIGN_CENTER);

  priv->list_model = gtk_list_store_new 
    (N_COLUMNS, G_TYPE_STRING);
  priv->tree_view = GTK_TREE_VIEW 
      (gtk_tree_view_new_with_model (GTK_TREE_MODEL(priv->list_model)));
  g_signal_connect (G_OBJECT (priv->tree_view), "row-activated", 
    G_CALLBACK (fixeddirfilepickerdialog_on_row_activated), self);
  g_signal_connect (G_OBJECT (priv->tree_view), "cursor-changed", 
    G_CALLBACK (fixeddirfilepickerdialog_foo), self);
  
  GtkCellRenderer *renderer0 = gtk_cell_renderer_text_new ();
  g_object_set (renderer0, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
  GtkTreeViewColumn *column0 = gtk_tree_view_column_new_with_attributes
    (COLUMN_0_TITLE, renderer0, "text", 0, NULL);
  gtk_tree_view_column_set_sort_column_id (column0, 0);
  gtk_tree_view_column_set_min_width (column0, 500);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), column0);

  GtkScrolledWindow *s = GTK_SCROLLED_WINDOW 
    (gtk_scrolled_window_new (NULL, NULL));
  gtk_scrolled_window_set_shadow_type (s, GTK_SHADOW_IN);
  gtk_scrolled_window_set_min_content_height (s, 400);
  gtk_scrolled_window_set_propagate_natural_width (s, TRUE);
  gtk_scrolled_window_set_propagate_natural_height (s, TRUE);
  gtk_container_add (GTK_CONTAINER (s), GTK_WIDGET (priv->tree_view));

  priv->filename_entry = GTK_ENTRY (gtk_entry_new ());

  gtk_grid_attach (content_box, GTK_WIDGET (s), 
        1, 1, 1, 1);

  gtk_grid_attach (content_box, GTK_WIDGET (priv->filename_entry), 
        1, 2, 1, 1);

  gtk_box_pack_start (GTK_BOX (content),
        GTK_WIDGET (content_box), FALSE, FALSE, 5);

  gtk_box_pack_start (GTK_BOX (content),
         gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  GtkWidget *action_area = gtk_dialog_get_action_area (GTK_DIALOG(self));
  gtk_widget_set_margin_start (action_area, 5);
  gtk_widget_set_margin_end (action_area, 5);
  gtk_widget_set_margin_top (action_area, 5);
  gtk_widget_set_margin_bottom (action_area, 5);
#pragma GCC diagnostic pop

  g_signal_connect (G_OBJECT(self), "response",
     G_CALLBACK (fixeddirfilepickerdialog_response_event), self);
  gtk_dialog_set_default_response (GTK_DIALOG (self), GTK_RESPONSE_OK);

  }

/*=============================================================================
  fixeddirfilepickerdialog_class_init
=============================================================================*/

static void fixeddirfilepickerdialog_class_init (FixedDirFilePickerDialogClass *klass)
  {
  g_type_class_add_private (klass, sizeof (FixedDirFilePickerDialogPriv));
  //FixedDirFilePickerDialogClass *parent = GTK_DIALOG_CLASS (klass);
  }

/*=============================================================================
  fixeddirfilepickerdialog_new
=============================================================================*/
FixedDirFilePickerDialog *fixeddirfilepickerdialog_new (GtkWindow *parent,
     FixedDirFilePickerType type)
  {
  FixedDirFilePickerDialog *self = g_object_new 
     (fixeddirfilepickerdialog_get_type(), NULL);
  gtk_window_set_transient_for (GTK_WINDOW(self), GTK_WINDOW (parent));
  FixedDirFilePickerDialogPriv *priv = FIXEDDIRFILEPICKERDIALOG_PRIV (self);
  priv->type = type;
  return self;
  }


