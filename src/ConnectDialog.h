/*==============================================================================
  
  gsxradio
  ConnectDialog.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

==============================================================================*/
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CONNECTDIALOG_TYPE            (connectdialog_get_type ())
#define CONNECTDIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONNECTDIALOG_TYPE, ConnectDialog))
#define CONNECTDIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CONNECTDIALOG_TYPE, ConnectDialogClass))
#define IS_CONNECTDIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONNECTDIALOG_TYPE))
#define IS_CONNECTDIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONNECTDIALOG_TYPE))
#define CONNECTDIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CONNECTDIALOG_TYPE, ConnectDialogClass))


typedef struct _ConnectDialog ConnectDialog;
typedef struct _ConnectDialogClass  ConnectDialogClass;
typedef struct _ConnectDialogPriv  ConnectDialogPriv;

/** A dialog box for entering server connection (host/port) information. */
struct _ConnectDialog 
  {
  GtkDialog base;
  };

struct _ConnectDialogClass
  {
  GtkDialogClass parent_class;
  gpointer padding[12];
  };

GType connectdialog_get_type (void);

ConnectDialog *connectdialog_new (GtkWindow *parent);

int            connectdialog_get_port (const ConnectDialog *self);

const char    *connectdialog_get_host (const ConnectDialog *self);

/** Populate the dialog with the init_host and init_port values. If use
    confirms, create return values in *host and *port, that the caller
    should free. */
int            connectdialog_run (const ConnectDialog *self, 
               const char *init_host, 
               int init_port, char **host, int *port);

G_END_DECLS



