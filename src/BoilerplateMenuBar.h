/*=============================================================================
  
  gsxradio
  BoilerplateMenuBar.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateController.h" 
#include "BoilerplateModel.h" 

#define BMB_MENU_NEW "_New"
#define BMB_MENU_OPEN "_Open..."
#define BMB_MENU_SAVE "_Save"
#define BMB_MENU_SAVE_AS "Save _as..."
#define BMB_MENU_CLOSE_WINDOW "_Close"
#define BMB_MENU_ABOUT "_About"


G_BEGIN_DECLS

#define BOILERPLATEMENUBAR_TYPE            (boilerplatemenubar_get_type ())
#define BOILERPLATEMENUBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOILERPLATEMENUBAR_TYPE, BoilerplateMenuBar))
#define BOILERPLATEMENUBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOILERPLATEMENUBAR_TYPE, BoilerplateMenuBarClass))
#define IS_BOILERPLATEMENUBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOILERPLATEMENUBAR_TYPE))
#define IS_BOILERPLATEMENUBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOILERPLATEMENUBAR_TYPE))
#define BOILERPLATEMENUBAR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BOILERPLATEMENUBAR_TYPE, BoilerplateMenuBarClass))

typedef struct _BoilerplateMenuBar BoilerplateMenuBar;
typedef struct _BoilerplateMenuBarClass BoilerplateMenuBarClass;
struct _BoilerplateModel;
struct _BoilerplateMainWindow;

/* A subclass of GtkMenuBar that provides basic File and Help menus.
   The object is provided with references to a controller and a model.
   Menu operations will usually result in calls on controller methods,
   and the status of the menu's display will be controlled by information
   obtained from the model */
struct _BoilerplateMenuBar
  {
  GtkMenuBar base;
  };

struct _BoilerplateMenuBarClass
  {
  GtkMenuBarClass parent_class;
  /** Called when a menu is shown (i.e., dropped down). Boilerplate
      behaviour is to call update_from_model() */ 
  void   (*on_menu_shown) (BoilerplateMenuBar *self);

  /** Called when a menu is shown (i.e., dropped down). Boilerplate
      behaviour is to update the sensitivity of the File menu items
      according to whether the model things data needs to be saved
      or not */
  void   (*update_from_model) (BoilerplateMenuBar *self);
  gpointer padding[12];
  };

GType boilerplatemenubar_get_type (void) G_GNUC_CONST;

BoilerplateMenuBar    
  *boilerplatemenubar_new (GtkAccelGroup *accel, 
                    BoilerplateController *controller,
		    struct _BoilerplateModel *model,
                    struct _BoilerplateMainWindow *w);

/** A helper function to append items to a specific drop-down menu.
    If the key argument is non-zero, it is taken as an ASCII character
    code of a key to be used in conjunction with the CTRL key to
    form an accelerator */
void                   
  boilerplatemenubar_append_menu_item  (BoilerplateMenuBar *self, 
                    GtkAccelGroup *accel_group, 
                    GtkMenu *menu, const char *label, int key, 
                    GCallback callback);

BoilerplateController 
  *boilerplatemenubar_get_controller 
                    (const BoilerplateMenuBar *self);

BoilerplateModel *boilerplatemenubar_get_model
                     (const BoilerplateMenuBar *self);

GtkAccelGroup          
  *boilerplatemenubar_get_accel_group
                    (const BoilerplateMenuBar *self);

void 
  boilerplatemenubar_on_menu_shown (GtkWidget *w, gpointer data);

/** A helper function to find the GtkMenuItem that corresponds to 
    a particular label. This can be used to find a menu item's action
    from its label, rather than storing references to all the 
    items in the menu. */
GtkMenuItem 
  *boilerplatemenubar_menuitem_from_label (const GtkMenu *m, 
  const char *_label);

struct _BoilerplateMainWindow
  *boilerplatemenubar_get_main_window (const BoilerplateMenuBar *self);

GtkMenu *boilerplatemenubar_get_file_menu
         (const BoilerplateMenuBar *self);
                      
G_END_DECLS




