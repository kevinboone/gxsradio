/*=============================================================================
  
  gxsradio
  BoilerplateMenuBar.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <string.h>
#include "BoilerplateMenuBar.h"
#include "BoilerplateController.h"
#include "BoilerplateModel.h"
#include "BoilerplateMainWindow.h"

#define BOILERPLATEMENUBAR_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          BOILERPLATEMENUBAR_TYPE, BoilerplateMenuBarPriv))

typedef struct _BoilerplateMenuBarPriv BoilerplateMenuBarPriv;

struct _BoilerplateMenuBarPriv
  {
  // We need to store reference to the file menu, as its items will
  //   change state according to the model
  GtkMenu *file_menu;
  BoilerplateController *controller;
  GtkAccelGroup *accel_group;
  BoilerplateModel *model;
  BoilerplateMainWindow *main_window;
  };

G_DEFINE_TYPE (BoilerplateMenuBar, boilerplatemenubar, GTK_TYPE_MENU_BAR);

typedef enum
  {
  PROP_ACCEL_GROUP = 1,
  PROP_CONTROLLER = 2,
  PROP_MODEL = 3,
  PROP_MAIN_WINDOW = 4,
  N_PROPERTIES
  } BoilerplateMenuBarProperty;

/*=============================================================================
  boilerplatemenubar_get_file_menu
=============================================================================*/
GtkMenu *boilerplatemenubar_get_file_menu
         (const BoilerplateMenuBar *self)
  {
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  return (priv->file_menu);
  }

/*=============================================================================
  boilerplatemenubar_get_controller
=============================================================================*/
BoilerplateController *boilerplatemenubar_get_controller 
         (const BoilerplateMenuBar *self)
  {
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  return (priv->controller);
  }

/*=============================================================================
  boilerplatemenubar_get_model
=============================================================================*/
BoilerplateModel *boilerplatemenubar_get_model
         (const BoilerplateMenuBar *self)
  {
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  return (priv->model);
  }

/*=============================================================================
  boilerplatemenubar_get_accel_group
=============================================================================*/
GtkAccelGroup *boilerplatemenubar_get_accel_group
                        (const BoilerplateMenuBar *self)
  {
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  return (priv->accel_group);
  }

/*=============================================================================
  boilerplatemenubar_append_menu_item
=============================================================================*/
void boilerplatemenubar_append_menu_item (BoilerplateMenuBar *self, 
        GtkAccelGroup *accel_group, GtkMenu *menu, const char *label, 
	int key, GCallback callback)
  {
  GtkMenuItem *aMenuItem = GTK_MENU_ITEM 
    (gtk_menu_item_new_with_mnemonic (label));
  g_signal_connect (G_OBJECT(aMenuItem), "activate",
     G_CALLBACK (callback),  
       self);       
  if (key)
    gtk_widget_add_accelerator(GTK_WIDGET (aMenuItem), 
      "activate", accel_group, key,  
         GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), 
    GTK_WIDGET (aMenuItem));
  } 


/*=============================================================================
  boilerplatemenubar_init
=============================================================================*/
static void boilerplatemenubar_init (BoilerplateMenuBar *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  boilerplatemenubar_set_property
=============================================================================*/
static void _boilerplatemenubar_set_property (GObject *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
  {
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (object);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);

  switch ((BoilerplateMenuBarProperty) property_id)
    {
    case PROP_ACCEL_GROUP:
      priv->accel_group = g_value_get_object (value); 
      break;
    case PROP_CONTROLLER:
      priv->controller = g_value_get_object (value); 
      break;
    case PROP_MODEL:
      priv->model = g_value_get_object (value); 
      break;
    case PROP_MAIN_WINDOW:
      priv->main_window = g_value_get_object (value); 
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
  }

/*=============================================================================
  boilerplatemenubar_get_property
=============================================================================*/
static void _boilerplatemenubar_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
  {
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (object);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  switch (property_id)
    {
    case PROP_ACCEL_GROUP:
      g_value_set_object (value, priv->accel_group);
      break;
    case PROP_CONTROLLER:
      g_value_set_object (value, priv->controller);
      break;
    case PROP_MODEL:
      g_value_set_object (value, priv->model);
      break;
    case PROP_MAIN_WINDOW:
      g_value_set_object (value, priv->main_window);
      break;
    }
  }

/*=============================================================================
  boilerplatemenubar_on_menu_new 
=============================================================================*/
static void boilerplatemenubar_on_menu_new (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
    ->action_new_file (priv->controller);
  }

/*=============================================================================
  boilerplatemenubar_on_menu_open
=============================================================================*/
static void boilerplatemenubar_on_menu_open (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
    ->action_prompt_open_file (priv->controller);
  }

/*=============================================================================
  boilerplatemenubar_on_menu_save
=============================================================================*/
static void boilerplatemenubar_on_menu_save (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
    ->action_save_current_file (priv->controller);
  }

/*=============================================================================
  boilerplatemenubar_on_menu_save_as
=============================================================================*/
static void boilerplatemenubar_on_menu_save_as (GtkWidget *w, 
      gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
    ->action_prompt_save_file (priv->controller);
  }

/*=============================================================================
  boilerplatemenubar_on_menu_close_window
=============================================================================*/
static void boilerplatemenubar_on_menu_close_window 
    (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
    ->action_close_window (priv->controller);
  }


/*=============================================================================
  boilerplatemenubar_on_menu_about
=============================================================================*/
static void boilerplatemenubar_on_menu_about
    (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
    ->action_about (priv->controller);
  }

/*=============================================================================
  _boilerplatemenubar_update_from_model
=============================================================================*/
void _boilerplatemenubar_update_from_model (BoilerplateMenuBar *self)
  {
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  gboolean need_save = boilerplatemodel_need_save (priv->model);
  GtkMenuItem *saveMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->file_menu, BMB_MENU_SAVE);
  if (need_save)
    gtk_widget_set_sensitive (GTK_WIDGET (saveMenuItem), TRUE); 
  else
    gtk_widget_set_sensitive (GTK_WIDGET (saveMenuItem), FALSE); 
  }

/*=============================================================================
  boilerplatemenubar_update_from_model
=============================================================================*/
void boilerplatemenubar_update_from_model (BoilerplateMenuBar *self)
  {
  BOILERPLATEMENUBAR_GET_CLASS(self)->update_from_model (self);
  }

/*=============================================================================
  _boilerplatemenubar_on_menu_shown
=============================================================================*/
static void _boilerplatemenubar_on_menu_shown (BoilerplateMenuBar *self)
  {
  boilerplatemenubar_update_from_model (self);
  }

/*=============================================================================
  boilerplatemenubar_on_menu_shown
=============================================================================*/
void boilerplatemenubar_on_menu_shown (GtkWidget *w, gpointer data)
  {
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (data);
  BOILERPLATEMENUBAR_GET_CLASS (self)->on_menu_shown (self);
  }

/*=============================================================================
  boilerplatemenubar_get_main_window
=============================================================================*/
BoilerplateMainWindow *boilerplatemenubar_get_main_window 
    (const BoilerplateMenuBar *self)
  {
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);
  return priv->main_window; 
  }
                      
/*=============================================================================
  boilerplatemenubar_menuitem_from_label
=============================================================================*/
GtkMenuItem *boilerplatemenubar_menuitem_from_label
     (const GtkMenu *m, const char *_label)
  {
  GtkMenuItem *ret = NULL;
  gboolean found = FALSE;
  GList *children = gtk_container_get_children (GTK_CONTAINER(m));
  for (GList *l = children; l != NULL && !found; l = l->next)
    {
    GtkMenuItem *mi = GTK_MENU_ITEM (l->data);
    const char *label = gtk_menu_item_get_label (mi);
    if (strcmp (label, _label) == 0)
      {
      found = TRUE;
      ret = mi;
      }
    }
  g_list_free (children);
  return ret;
  }

/*=============================================================================
  boilerplatemenubar_constructed
=============================================================================*/
static void _boilerplatemenubar_constructed (GObject *obj)
  {
  BoilerplateMenuBar *self = BOILERPLATEMENUBAR (obj);
  BoilerplateMenuBarPriv *priv = BOILERPLATEMENUBAR_PRIV (self);

  GtkAccelGroup *accel_group = priv->accel_group; 

  GtkMenuItem *fileMenuItem = GTK_MENU_ITEM 
    (gtk_menu_item_new_with_mnemonic ("_File"));
  priv->file_menu = GTK_MENU (gtk_menu_new ());

  g_signal_connect (G_OBJECT(priv->file_menu), "show",
     G_CALLBACK (boilerplatemenubar_on_menu_shown),  
       self);       

  boilerplatemenubar_append_menu_item (self, accel_group, 
     priv->file_menu, BMB_MENU_NEW, 'n', 
     G_CALLBACK (boilerplatemenubar_on_menu_new));
  boilerplatemenubar_append_menu_item (self, accel_group, 
     priv->file_menu, BMB_MENU_OPEN, 'o', 
     G_CALLBACK (boilerplatemenubar_on_menu_open));
  boilerplatemenubar_append_menu_item (self, accel_group, 
     priv->file_menu, BMB_MENU_SAVE, 's', 
     G_CALLBACK (boilerplatemenubar_on_menu_save));
  boilerplatemenubar_append_menu_item (self, accel_group, 
     priv->file_menu, BMB_MENU_SAVE_AS, 0, 
     G_CALLBACK (boilerplatemenubar_on_menu_save_as));
  boilerplatemenubar_append_menu_item (self, accel_group, 
     priv->file_menu, BMB_MENU_CLOSE_WINDOW, 'w', 
     G_CALLBACK (boilerplatemenubar_on_menu_close_window));
  
  // File menu add to bar 
  gtk_menu_item_set_submenu (fileMenuItem, 
      GTK_WIDGET (priv->file_menu));
  gtk_menu_shell_append (GTK_MENU_SHELL (self), GTK_WIDGET (fileMenuItem));
  
  // Help menu create
  GtkMenuItem *helpMenuItem = GTK_MENU_ITEM 
    (gtk_menu_item_new_with_mnemonic ("_Help"));
  GtkMenu *help_menu = GTK_MENU (gtk_menu_new ());
  g_signal_connect (G_OBJECT(help_menu), "show",
     G_CALLBACK (boilerplatemenubar_on_menu_shown),  
       self);       

  boilerplatemenubar_append_menu_item (self, accel_group, 
     help_menu, "_About", 0, 
     G_CALLBACK (boilerplatemenubar_on_menu_about));

  // Help menu add to bar 
  gtk_menu_item_set_submenu (helpMenuItem, 
      GTK_WIDGET (help_menu));
  gtk_menu_shell_append (GTK_MENU_SHELL (self), GTK_WIDGET (helpMenuItem));

  G_OBJECT_CLASS (boilerplatemenubar_parent_class)->constructed (obj);
  }

/*=============================================================================
  boilerplatemenubar_class_init
=============================================================================*/
static void boilerplatemenubar_class_init
  (BoilerplateMenuBarClass *klass)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (BoilerplateMenuBarPriv));
  G_OBJECT_CLASS (klass)->set_property =  _boilerplatemenubar_set_property;
  G_OBJECT_CLASS (klass)->get_property = _boilerplatemenubar_get_property;

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_ACCEL_GROUP, 
      g_param_spec_object ("accel-group", "foo", "foo", GTK_TYPE_ACCEL_GROUP,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_CONTROLLER, 
      g_param_spec_object ("controller", "foo", "foo", 
      BOILERPLATECONTROLLER_TYPE, 
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_MODEL, 
      g_param_spec_object ("model", "foo", "foo", 
      BOILERPLATEMODEL_TYPE, 
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_MAIN_WINDOW, 
      g_param_spec_object ("main-window", "foo", "foo", 
      BOILERPLATEMAINWINDOW_TYPE, 
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
 
  klass->on_menu_shown = _boilerplatemenubar_on_menu_shown;
  klass->update_from_model = _boilerplatemenubar_update_from_model;
  G_OBJECT_CLASS (klass)->constructed = _boilerplatemenubar_constructed; 
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  boilerplatemenubar_new
=============================================================================*/
BoilerplateMenuBar *boilerplatemenubar_new (GtkAccelGroup *accel_group,
     BoilerplateController *controller, BoilerplateModel *model,
     BoilerplateMainWindow *w)
  {
  BoilerplateMenuBar *self = g_object_new 
     (BOILERPLATEMENUBAR_TYPE, "accel-group", accel_group, 
      "controller", controller, "model", model, "main-window",
      w, NULL);
  return self;
  }

