/*=============================================================================
  
  gxsradio 
  EditStationDialog.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#define _GNU_SOURCE
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
#include "BoilerplateApplication.h"
#include "EditStationDialog.h"
#include "StationListController.h"
#include "StationListModel.h"
#include "boilerplatedialogs.h"

#define EDITSTATIONDIALOG_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          EDITSTATIONDIALOG_TYPE, EditStationDialogPriv))

typedef struct _EditStationDialogPriv EditStationDialogPriv;

struct _EditStationDialogPriv
  {
  GtkEntry *name_entry;
  GtkEntry *uri_entry;
  GtkEntry *country_entry;
  };

G_DEFINE_TYPE (EditStationDialog, editstationdialog, GTK_TYPE_DIALOG);

/*======================================================================
  editstationdialog_response_event
======================================================================*/
static void editstationdialog_response_event (GtkDialog *d,
  int id, gpointer user_data)
  {
  }

/*=============================================================================
  editstationdialog_run
=============================================================================*/
int editstationdialog_run (const EditStationDialog *self,
       const char *init_name, const char *init_uri, const char *init_country, 
       char **name, char **uri, char **country)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  EditStationDialogPriv *priv = EDITSTATIONDIALOG_PRIV (self);

  gtk_widget_show_all (GTK_WIDGET(self));
  int response;
  gboolean valid;

  if (init_name) gtk_entry_set_text (priv->name_entry, init_name);
  if (init_uri) gtk_entry_set_text (priv->uri_entry, init_uri);
  if (init_country) gtk_entry_set_text (priv->country_entry, init_country);

  do
    {
    valid = TRUE;
    response = gtk_dialog_run (GTK_DIALOG (self));
    if (response == GTK_RESPONSE_OK)
      {
      valid = TRUE;

      const char *new_name = gtk_entry_get_text (priv->name_entry);
      const char *new_uri = gtk_entry_get_text (priv->uri_entry);
      const char *new_country = gtk_entry_get_text (priv->country_entry);
      
      if (valid)
        {
        if (strlen (new_name) == 0)
          {
          boilerplatedialogs_error (GTK_WINDOW(self), NAME, 
            "Station name cannot be empty");
          gtk_widget_grab_focus (GTK_WIDGET (priv->name_entry));
          valid = FALSE;
          } 
        }
      if (valid)
        {
        if (strlen (new_country) == 0)
          {
          boilerplatedialogs_error (GTK_WINDOW(self), NAME, 
            "Country cannot be empty");
          gtk_widget_grab_focus (GTK_WIDGET (priv->country_entry));
          valid = FALSE;
          } 
        }
      if (valid)
        {
        if (strlen (new_uri) == 0)
          {
          boilerplatedialogs_error (GTK_WINDOW(self), NAME, 
            "URI cannot be empty");
          gtk_widget_grab_focus (GTK_WIDGET (priv->uri_entry));
          valid = FALSE;
          } 
        }

      if (valid)
        {
        *name = strdup (new_name);
        *uri = strdup (new_uri);
        *country = strdup (new_country);
        }
      }
    } while (response == GTK_RESPONSE_OK && !valid);

  gtk_widget_destroy (GTK_WIDGET (self));

  return response;
  }

/*=============================================================================
  editstationdialog_init
=============================================================================*/
static void editstationdialog_init (EditStationDialog *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  EditStationDialogPriv *priv = EDITSTATIONDIALOG_PRIV (self);

  gtk_window_set_title (GTK_WINDOW (self), NAME ": preferences");

  gtk_dialog_add_buttons (GTK_DIALOG (self),
       "_Cancel", GTK_RESPONSE_CANCEL, "C_onfirm", GTK_RESPONSE_OK, NULL);

  GtkBox *content = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (self)));
  GtkGrid *content_box = GTK_GRID (gtk_grid_new ());
  gtk_grid_set_row_spacing (content_box, 10);
  gtk_widget_set_margin_start (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_end (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_top (GTK_WIDGET(content_box), 0);
  gtk_widget_set_margin_bottom (GTK_WIDGET(content_box), 0);

  GtkEntry *name_entry = GTK_ENTRY(gtk_entry_new ());
  GtkEntry *uri_entry = GTK_ENTRY(gtk_entry_new ());
  GtkEntry *country_entry = GTK_ENTRY(gtk_entry_new ());
  priv->name_entry = name_entry;
  priv->uri_entry = uri_entry;
  priv->country_entry = country_entry;

  GtkWidget *l = gtk_label_new  ("Enter the station details:");
  gtk_label_set_line_wrap (GTK_LABEL(l), TRUE);
  gtk_grid_attach (content_box, l, 
        1, 1, 2, 1);

  l = gtk_label_new_with_mnemonic ("_Name: ");
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET (name_entry));
  gtk_grid_attach (content_box, l,
        1, 2, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(name_entry),
        2, 2, 1, 1);

  l = gtk_label_new_with_mnemonic ("_URI: ");
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET (uri_entry));
  gtk_grid_attach (content_box, l,
        1, 3, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(uri_entry),
        2, 3, 1, 1);

  l = gtk_label_new_with_mnemonic ("_Country: ");
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET (country_entry));
  gtk_grid_attach (content_box, l,
        1, 4, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(country_entry),
        2, 4, 1, 1);

  gtk_box_pack_start (GTK_BOX (content),
         GTK_WIDGET(content_box), FALSE, FALSE, 5);
  gtk_box_pack_start (GTK_BOX (content),
         gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), TRUE, FALSE, 5);

  gtk_widget_set_halign (GTK_WIDGET(content_box), GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand (GTK_WIDGET(content_box), TRUE);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  GtkWidget *action_area = gtk_dialog_get_action_area (GTK_DIALOG(self));
  gtk_widget_set_margin_start (action_area, 5);
  gtk_widget_set_margin_end (action_area, 5);
  gtk_widget_set_margin_top (action_area, 5);
  gtk_widget_set_margin_bottom (action_area, 5);
#pragma GCC diagnostic pop

  g_signal_connect (G_OBJECT(self), "response",
     G_CALLBACK (editstationdialog_response_event), self);
  gtk_dialog_set_default_response (GTK_DIALOG (self), GTK_RESPONSE_OK);

  }

/*=============================================================================
  editstationdialog_class_init
=============================================================================*/

static void editstationdialog_class_init (EditStationDialogClass *klass)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (EditStationDialogPriv));
  //EditStationDialogClass *parent = GTK_DIALOG_CLASS (klass);
  }

/*=============================================================================
  editstationdialog_new
=============================================================================*/
EditStationDialog *editstationdialog_new (GtkWindow *parent)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  EditStationDialog *self = g_object_new 
     (editstationdialog_get_type(), NULL);
  gtk_window_set_transient_for (GTK_WINDOW(self), GTK_WINDOW (parent));
  return self;
  }


