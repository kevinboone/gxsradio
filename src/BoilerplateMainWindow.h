/*============================================================================
  
  gsxradio
  BoilerplateMainWindow.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateController.h" 
#include "BoilerplateModel.h" 

G_BEGIN_DECLS

#define BOILERPLATEMAINWINDOW_TYPE            (boilerplatemainwindow_get_type ())
#define BOILERPLATEMAINWINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOILERPLATEMAINWINDOW_TYPE, BoilerplateMainWindow))
#define BOILERPLATEMAINWINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOILERPLATEMAINWINDOW_TYPE, BoilerplateMainWindowClass))
#define IS_BOILERPLATEMAINWINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOILERPLATEMAINWINDOW_TYPE))
#define IS_BOILERPLATEMAINWINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOILERPLATEMAINWINDOW_TYPE))
#define BOILERPLATEMAINWINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BOILERPLATEMAINWINDOW_TYPE, BoilerplateMainWindowClass))

typedef struct _BoilerplateMainWindow BoilerplateMainWindow;
typedef struct _BoilerplateMainWindowClass  BoilerplateMainWindowClass;

/** BoilerplateMainWindow is a general top-level window, that has
    an associated Model (a subclass of BoilerplateModel) and an
    associated controller (a subclass of BoilerplateController).
    The boilerplate implementation creates a menu bar and a main
    widget, and packs them into a grid. Subclasses are expected
    to override many of the methods in this class, to provide
    application-specific appearance */
struct _BoilerplateMainWindow 
  {
  GtkWindow base;
  };

struct _BoilerplateApplication;
struct _BoilerplateModel;
struct _BoilerplateController;

struct _BoilerplateMainWindowClass
  {
  GtkWindowClass parent_class;
  /** Create the main menu. Boilerplate behaviour to create an instance
      of BoilerplateMenuBar, which contains 'File' and 'Help' menus.
      Subclasses should override this and provide a subclass of
      BoilerplateMenuBar */

  GtkMenuBar *(*create_main_menu)(BoilerplateMainWindow *self, 
                GtkAccelGroup *accel, BoilerplateController *controller);
  
  /** Create the main widget -- that is, whatever fills the 
      part of the window frame that isn't occupied by the menu.
      Subclasses will always override this. There is no 'boilerplate
      widget' -- the main widget is any subclass of GtkWidget */
  GtkWidget  *(*create_main_widget) (BoilerplateMainWindow *self);
  
  /** Setup does most of the work involved in creating the top-level
      window -- it initializes the model and the controller, and then
      packs the user interface elements into a grid. Subclasses probably
      do not need to change this behaviour */
  void        (*setup) (BoilerplateMainWindow *self, 
                struct _BoilerplateApplication *application);

  /** on_window_delete() is linked by the setup method to the 
      window manager close operation. Boilerplate behaviour is to
      call the controller's action_close_window() method */
  void        (*on_window_delete) (BoilerplateMainWindow *self);

  /** Create a controller. Subclasses should override this and
      provide an application-specific subclass of BoilerplateController */
  BoilerplateController *(*create_controller) 
                (BoilerplateMainWindow *self, 
                struct _BoilerplateApplication *application);

  /** Create a model. Subclasses should override this and
      provide an application-specific subclass of BoilerplateModel */
  BoilerplateModel *(*create_model) (BoilerplateMainWindow *self);

  /** Respond to major changes in the model, such as a new file
      being opened. Boilerplate behaviour is to update the window
      caption */
  void        (*model_file_changed) (BoilerplateMainWindow *self);

  GtkWidget  *(*create_statusbar) (BoilerplateMainWindow *self, 
               const BoilerplateModel *model, 
	       const BoilerplateController *controller);

  void        (*statusbar_message) (BoilerplateMainWindow *self, 
               const char *message);

  void        (*statusbar_clear) (const BoilerplateMainWindow *self);

  gpointer padding[12];
  };

GType boilerplatemainwindow_get_type (void) G_GNUC_CONST;

BoilerplateMainWindow *boilerplatemainwindow_new (void);

/** Models or controllers should call this to notify the window of
    substantial changes to the model. */
void 
  boilerplatemainwindow_model_file_changed (BoilerplateMainWindow *self);

/** Get the model that was created during object set-up. */
struct _BoilerplateModel *
  boilerplatemainwindow_get_model 
           (const BoilerplateMainWindow *self);

/** Get the controller that was created during object set-up. */
BoilerplateController *boilerplatemainwindow_get_controller 
           (const BoilerplateMainWindow *self); 

/** Get the main widget that was created during object set-up. */
GtkWidget *boilerplatemainwindow_get_main_widget 
           (const BoilerplateMainWindow *self);

void boilerplatemainwindow_statusbar_message 
           (BoilerplateMainWindow *self, const char *message);

void boilerplatemainwindow_statusbar_clear 
           (const BoilerplateMainWindow *self);

G_END_DECLS




