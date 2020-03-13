/*==============================================================================
  
  gsxradio
  FixedDirFilePickerDialog.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

==============================================================================*/
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define FIXEDDIRFILEPICKERDIALOG_TYPE            (fixeddirfilepickerdialog_get_type ())
#define FIXEDDIRFILEPICKERDIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FIXEDDIRFILEPICKERDIALOG_TYPE, FixedDirFilePickerDialog))
#define FIXEDDIRFILEPICKERDIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FIXEDDIRFILEPICKERDIALOG_TYPE, FixedDirFilePickerDialogClass))
#define IS_FIXEDDIRFILEPICKERDIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FIXEDDIRFILEPICKERDIALOG_TYPE))
#define IS_FIXEDDIRFILEPICKERDIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FIXEDDIRFILEPICKERDIALOG_TYPE))
#define FIXEDDIRFILEPICKERDIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), FIXEDDIRFILEPICKERDIALOG_TYPE, FixedDirFilePickerDialogClass))


typedef struct _FixedDirFilePickerDialog FixedDirFilePickerDialog;
typedef struct _FixedDirFilePickerDialogClass  FixedDirFilePickerDialogClass;
typedef struct _FixedDirFilePickerDialogPriv  FixedDirFilePickerDialogPriv;

typedef enum
  {
  FIXEDDIRPICKER_OPEN,
  FIXEDDIRPICKER_SAVE
  } FixedDirFilePickerType;

/** A dialog box for entering server connection (host/port) information. */
struct _FixedDirFilePickerDialog 
  {
  GtkDialog base;
  };

struct _FixedDirFilePickerDialogClass
  {
  GtkDialogClass parent_class;
  gpointer padding[12];
  };

GType fixeddirfilepickerdialog_get_type (void);

FixedDirFilePickerDialog *fixeddirfilepickerdialog_new (GtkWindow *parent,
                  FixedDirFilePickerType type);

int            fixeddirfilepickerdialog_run 
                  (const FixedDirFilePickerDialog *self, 
                  const char *fixed_dir, char **pathname);

G_END_DECLS



