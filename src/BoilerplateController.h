/*=============================================================================
  
  gsxradio
  BoilerplateController.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define BOILERPLATECONTROLLER_TYPE            (boilerplatecontroller_get_type ())
#define BOILERPLATECONTROLLER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOILERPLATECONTROLLER_TYPE, BoilerplateController))
#define BOILERPLATECONTROLLER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOILERPLATECONTROLLER_TYPE, BoilerplateControllerClass))
#define IS_BOILERPLATECONTROLLER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOILERPLATECONTROLLER_TYPE))
#define IS_BOILERPLATECONTROLLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOILERPLATECONTROLLER_TYPE))
#define BOILERPLATECONTROLLER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BOILERPLATECONTROLLER_TYPE, BoilerplateControllerClass))

typedef struct _BoilerplateController BoilerplateController;
typedef struct _BoilerplateControllerClass  BoilerplateControllerClass;

struct _BoilerplateApplication;
struct _BoilerplateMainWindow;
struct _BoilerplateModel;

/** BoilerplateController is a base class for a user interface controller.
    It mediates between the user interface (the MainWindow and its
    subcomponents) and the data model. The boilerplate model contains
    logic for handling the main-menu operations that are found in
    most applications that handle files. Applications are expected
    to create at least one subclass of this controller, to handle
    application-specific control tasks. The methods in this class
    typically delegate to the model, and then inform the user
    interface that the model has changed, so the display can be
    updated appropriately */
struct _BoilerplateController
  {
  GObject base;
  };

struct _BoilerplateControllerClass
  {
  GObjectClass parent_class;
    /** Create a new file. This operation is typically delegated
        directly to the model */
    void          (*action_new_file) (BoilerplateController *self);

    /** Save furrent data to a file with the specified name 
        This operation is typically delegated directly to the model */
    void          (*action_save_file) (BoilerplateController *self, 
                    const char *filename);

    /** Open a new file, after ensuring that it is safe to close the
        existing file. This may involve saving unsaved data, which
	may involve prompting the user for a filename */
    void          (*action_open_file) (BoilerplateController *self, 
                      const char *filename);

    /** Save the current data to the current filename. This method will
        fail badly if no filename has been set */
    void          (*action_save_current_file) (BoilerplateController *self);

    /** Prompt the user for a filename, and open the new file. This 
        operation may involve saving existing data, which may involve
	prompting the user for a filename */
    void          (*action_prompt_open_file) (BoilerplateController *self);

    /** Prompt the user for a filename, save data to this file. */
    void          (*action_prompt_save_file) (BoilerplateController *self);

    /** Show the "About" dialog box. The message comes from 
        BoilerplateApplication.get_about_message() */
    void          (*action_about) (BoilerplateController *self);

    /** Close the current window, if possible. The user will be prompted
        to save usaved data. The window will not close if data was not
	correctly saved. This action closes the associated top-level 
	window, which may exit the application */
    void          (*action_close_window) (BoilerplateController *self);

    gboolean      (*prompt_for_open_filename)
                        (BoilerplateController *self, char **filename);

    /** Subclasses may provide a new "File save" dialog box. If they
        do, they should probably take care internally of checking 
	whether a file should be overwritten. */
    gboolean      (*prompt_for_save_filename)
                        (BoilerplateController *self, char **filename);

  gpointer padding[12];
  };

GType boilerplatecontroller_get_type (void) G_GNUC_CONST;

BoilerplateController 
  *boilerplatecontroller_new (struct _BoilerplateMainWindow *w,
                        struct _BoilerplateApplication *application,
			struct _BoilerplateModel *model);

struct _BoilerplateModel
  *boilerplatecontroller_get_model (const BoilerplateController *self);

struct _BoilerplateMainWindow
  *boilerplatecontroller_get_main_window (const BoilerplateController *self);

/** Convenience message to route a message to the status bar of the top-
    level window associated with this controller */
void boilerplatecontroller_statusbar_message (const BoilerplateController *self,
    const char *message);

gboolean boilerplatecontroller_prompt_for_save_filename 
     (BoilerplateController *self, char **filename);

struct _BoilerplateApplication *boilerplatecontroller_get_application
    (const BoilerplateController *self);

gboolean boilerplatecontroller_prompt_for_open_filename
     (BoilerplateController *self, char **filename);

gboolean boilerplatecontroller_prompt_for_save_filename
     (BoilerplateController *self, char **filename);

G_END_DECLS





