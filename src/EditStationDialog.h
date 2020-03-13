/*==============================================================================
  
  gsxradio
  EditStationDialog.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

==============================================================================*/
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EDITSTATIONDIALOG_TYPE            (editstationdialog_get_type ())
#define EDITSTATIONDIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDITSTATIONDIALOG_TYPE, EditStationDialog))
#define EDITSTATIONDIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EDITSTATIONDIALOG_TYPE, EditStationDialogClass))
#define IS_EDITSTATIONDIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EDITSTATIONDIALOG_TYPE))
#define IS_EDITSTATIONDIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EDITSTATIONDIALOG_TYPE))
#define EDITSTATIONDIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EDITSTATIONDIALOG_TYPE, EditStationDialogClass))


typedef struct _EditStationDialog EditStationDialog;
typedef struct _EditStationDialogClass  EditStationDialogClass;
typedef struct _EditStationDialogPriv  EditStationDialogPriv;

/** A dialog box for entering or editing the station name, URI, and
    country information. */
struct _EditStationDialog 
  {
  GtkDialog base;
  };

struct _EditStationDialogClass
  {
  GtkDialogClass parent_class;
  gpointer padding[12];
  };

GType editstationdialog_get_type (void);

EditStationDialog   *editstationdialog_new (GtkWindow *parent);
int            editstationdialog_run (const EditStationDialog *self,
                 const char *init_name, const char *init_uri, 
                 const char *init_country, char **name, char **uri,
                 char **country);

G_END_DECLS



