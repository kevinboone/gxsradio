/*=============================================================================
  
  gsxradio
  BoilerplateModel.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define BOILERPLATEMODEL_TYPE            (boilerplatemodel_get_type ())
#define BOILERPLATEMODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOILERPLATEMODEL_TYPE, BoilerplateModel))
#define BOILERPLATEMODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOILERPLATEMODEL_TYPE, BoilerplateModelClass))
#define IS_BOILERPLATEMODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOILERPLATEMODEL_TYPE))
#define IS_BOILERPLATEMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOILERPLATEMODEL_TYPE))
#define BOILERPLATEMODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BOILERPLATEMODEL_TYPE, BoilerplateModelClass))

typedef struct _BoilerplateModel BoilerplateModel;
typedef struct _BoilerplateModelClass  BoilerplateModelClass;

/** BoilerplateModel is the base class -- or, rather, interface --
    for data models. It specifies the basic open and save behaviour.
    Subclasses should override _all_ the methods in this class. 
    Calling any of the boilerplate methods will abort the program. */
struct _BoilerplateModel
  {
  GObject base;
  };

struct _BoilerplateModelClass
  {
  GObjectClass parent_class;
  
  /** Read a file into the model, and return any errors as a
      char ** */
  gboolean               (*do_open_file) (BoilerplateModel *self, 
                              const char *file, char **error);
  
  /** Save the file, and return any errors as a char ** */
  gboolean               (*do_save_file) (BoilerplateModel *self, 
                              const char *file, char **error);

  /** Save the file to the current filename, and return any 
      errors as a char ** */
  gboolean               (*do_save_current_file) (BoilerplateModel *self, 
                              char **error);

  /** Close the current file. Subclasses should always override this */
  void                   (*do_close_file) (BoilerplateModel *self);
  /** need_save() should return TRUE if the current data needs to be
      saved. Subclasses should definitely override this method */
  gboolean               (*need_save) (const BoilerplateModel *self);

  /** Called when a file dialog is about to be shown. Typically the
      model will set filters for the types of files it handles */
  void                   (*setup_open_dialog) 
                            (const BoilerplateModel *self,
                            GtkFileChooserDialog *dialog);

  gpointer padding[12];
  };

GType boilerplatemodel_get_type (void) G_GNUC_CONST;

BoilerplateModel 
  *boilerplatemodel_new (void);

const char       
  *boilerplatemodel_get_current_filename (const BoilerplateModel *self);

void 
  boilerplatemodel_set_current_filename (BoilerplateModel *self, 
        const char *filename);

gboolean
  boilerplatemodel_need_save (const BoilerplateModel *self);

G_END_DECLS





