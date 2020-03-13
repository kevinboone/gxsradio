/*==============================================================================
  
  gsxradio
  PrefsDialog.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

==============================================================================*/
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PREFSDIALOG_TYPE            (prefsdialog_get_type ())
#define PREFSDIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFSDIALOG_TYPE, PrefsDialog))
#define PREFSDIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFSDIALOG_TYPE, PrefsDialogClass))
#define IS_PREFSDIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFSDIALOG_TYPE))
#define IS_PREFSDIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFSDIALOG_TYPE))
#define PREFSDIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PREFSDIALOG_TYPE, PrefsDialogClass))


typedef struct _PrefsDialog PrefsDialog;
typedef struct _PrefsDialogClass  PrefsDialogClass;
typedef struct _PrefsDialogPriv  PrefsDialogPriv;

/** A dialog box for entering preferences. At present, only the display
    font can be changed. */
struct _PrefsDialog 
  {
  GtkDialog base;
  };

struct _PrefsDialogClass
  {
  GtkDialogClass parent_class;
  gpointer padding[12];
  };

GType prefsdialog_get_type (void);

PrefsDialog   *prefsdialog_new (GtkWindow *parent);
int            prefsdialog_run (const PrefsDialog *self,
                 gboolean init_single, gboolean *single,
		 const char *init_font, char **font);

G_END_DECLS



