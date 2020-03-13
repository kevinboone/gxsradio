/*=============================================================================
  
  gxsradio
  ConnectDialog.c

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
#include "BoilerplateApplication.h"
#include "ConnectDialog.h"
#include "StationListController.h"
#include "StationListModel.h"
#include "boilerplatedialogs.h"

#define CONNECTDIALOG_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          CONNECTDIALOG_TYPE, ConnectDialogPriv))

typedef struct _ConnectDialogPriv ConnectDialogPriv;

struct _ConnectDialogPriv
  {
  GtkEntry *host_entry;
  GtkEntry *port_entry;
  };

G_DEFINE_TYPE (ConnectDialog, connectdialog, GTK_TYPE_DIALOG);

/*======================================================================
  connectdialog_response_event
======================================================================*/
static void connectdialog_response_event (GtkDialog *d,
  int id, gpointer user_data)
  {
  /*
  ConnectDialog *self = (ConnectDialog *)d;
  if (id == GTK_RESPONSE_OK)
    {
    printf ("foo\n");
    }
  */
  }

/*=============================================================================
  connectdialog_get_host
=============================================================================*/
const char *connectdialog_get_host (const ConnectDialog *self)
  {
  ConnectDialogPriv *priv = CONNECTDIALOG_PRIV (self);
  return gtk_entry_get_text (priv->host_entry);
  }

/*=============================================================================
  connectdialog_get_port
=============================================================================*/
int connectdialog_get_port (const ConnectDialog *self)
  {
  ConnectDialogPriv *priv = CONNECTDIALOG_PRIV (self);
  return atoi (gtk_entry_get_text (priv->port_entry));
  }

/*=============================================================================
  connectdialog_run
=============================================================================*/
int connectdialog_run (const ConnectDialog *self, const char *init_host, 
     int init_port, char **host, int *port)
  {
  ConnectDialogPriv *priv = CONNECTDIALOG_PRIV (self);
  gtk_entry_set_text (priv->host_entry, init_host);
  char s_init_port[20];
  snprintf (s_init_port, sizeof (s_init_port) - 1, "%d", init_port);
  gtk_entry_set_text (priv->port_entry, s_init_port);
  gtk_widget_show_all (GTK_WIDGET (self));

  int response;
  gboolean valid;
  do
    {
    valid = TRUE;
    response = gtk_dialog_run (GTK_DIALOG (self));
    if (response == GTK_RESPONSE_OK)
      {
      const char *new_host = connectdialog_get_host (self);
      int new_port = connectdialog_get_port (self);

      if (valid)
        {
        if (strlen (new_host) == 0)
          {
          boilerplatedialogs_error (GTK_WINDOW(self), NAME, 
            "Host name cannot be empty");
          gtk_widget_grab_focus (GTK_WIDGET (priv->host_entry));
          valid = FALSE;
          }
        if (valid && new_port <= 1024)
          {
          boilerplatedialogs_error (GTK_WINDOW(self), NAME, 
            "Port must be a number greater than 1024");
          gtk_widget_grab_focus (GTK_WIDGET (priv->port_entry));
          valid = FALSE;
          }
        }
      
      if (valid)
        {
        *port = new_port;
        *host = strdup (new_host);
        }
      }
    } while (response == GTK_RESPONSE_OK && !valid);

  gtk_widget_destroy (GTK_WIDGET (self));
  

  return response;
  }

/*=============================================================================
  connectdialog_init
=============================================================================*/
static void connectdialog_init (ConnectDialog *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  ConnectDialogPriv *priv = CONNECTDIALOG_PRIV (self);

  gtk_window_set_title (GTK_WINDOW (self), NAME ": connect");

  gtk_dialog_add_buttons (GTK_DIALOG (self),
       "_Cancel", GTK_RESPONSE_CANCEL, "C_onnect", GTK_RESPONSE_OK, NULL);

  GtkBox *content = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (self)));
  GtkGrid *content_box = GTK_GRID (gtk_grid_new ());
  gtk_grid_set_row_spacing (content_box, 10);
  gtk_widget_set_margin_start (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_end (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_top (GTK_WIDGET(content_box), 0);
  gtk_widget_set_margin_bottom (GTK_WIDGET(content_box), 0);

  GtkEntry *host_entry = GTK_ENTRY(gtk_entry_new ());
  GtkEntry *port_entry = GTK_ENTRY(gtk_entry_new ());
  priv->host_entry = host_entry;
  priv->port_entry = port_entry;

  GtkWidget *l = gtk_label_new  
       ("Enter the hostname/IP and port number "
        "of the xineserver instance to connect to");
  gtk_label_set_line_wrap (GTK_LABEL(l), TRUE);
  gtk_grid_attach (content_box, l, 
        1, 1, 2, 1);

  l = gtk_label_new_with_mnemonic ("_Host: ");
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET (host_entry));
  gtk_grid_attach (content_box, l,
        1, 2, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(host_entry),
        2, 2, 1, 1);

  l = gtk_label_new_with_mnemonic ("_Port: ");
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET (port_entry));
  gtk_grid_attach (content_box, l,
        1, 3, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(port_entry),
        2, 3, 1, 1);

  gtk_widget_set_halign (GTK_WIDGET(content_box), GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand (GTK_WIDGET(content_box), TRUE);
  gtk_box_pack_start (GTK_BOX (content),
        GTK_WIDGET (content_box), FALSE, FALSE, 5);

  gtk_box_pack_start (GTK_BOX (content),
         gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  GtkWidget *action_area = gtk_dialog_get_action_area (GTK_DIALOG(self));
  gtk_widget_set_margin_start (action_area, 5);
  gtk_widget_set_margin_end (action_area, 5);
  gtk_widget_set_margin_top (action_area, 5);
  gtk_widget_set_margin_bottom (action_area, 5);
#pragma GCC diagnostic pop

  g_signal_connect (G_OBJECT(self), "response",
     G_CALLBACK (connectdialog_response_event), self);
  gtk_dialog_set_default_response (GTK_DIALOG (self), GTK_RESPONSE_OK);

  }

/*=============================================================================
  connectdialog_class_init
=============================================================================*/

static void connectdialog_class_init (ConnectDialogClass *klass)
  {
  g_type_class_add_private (klass, sizeof (ConnectDialogPriv));
  //ConnectDialogClass *parent = GTK_DIALOG_CLASS (klass);
  }

/*=============================================================================
  connectdialog_new
=============================================================================*/
ConnectDialog *connectdialog_new (GtkWindow *parent)
  {
  ConnectDialog *self = g_object_new 
     (connectdialog_get_type(), NULL);
  gtk_window_set_transient_for (GTK_WINDOW(self), GTK_WINDOW (parent));
  return self;
  }


