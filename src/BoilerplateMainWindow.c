/*============================================================================
  
  gxsradio
  BoilerplateMainWindow.c

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
#include "BoilerplateMainWindow.h"
#include "BoilerplateApplication.h"
#include "BoilerplateController.h"
#include "BoilerplateMenuBar.h"
#include "BoilerplateModel.h"
#include "stringutil.h"

#define BOILERPLATEMAINWINDOW_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          BOILERPLATEMAINWINDOW_TYPE, BoilerplateMainWindowPriv))

typedef struct _BoilerplateMainWindowPriv BoilerplateMainWindowPriv;

struct _BoilerplateMainWindowPriv
  {
  BoilerplateApplication *application;
  BoilerplateModel *model;
  BoilerplateController *controller;
  GtkWidget *main_widget;
  GtkWidget *statusbar;
  int timer; // Timer for the status bar timeout
  };

G_DEFINE_TYPE (BoilerplateMainWindow, boilerplatemainwindow, GTK_TYPE_WINDOW);

/*=============================================================================
  boilerplatemainwindow_get_model
=============================================================================*/
BoilerplateModel *boilerplatemainwindow_get_model 
          (const BoilerplateMainWindow *self)
  {
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  return priv->model;
  }

/*=============================================================================
  boilerplatemainwindow_get_controller
=============================================================================*/
BoilerplateController  *boilerplatemainwindow_get_controller
          (const BoilerplateMainWindow *self)
  {
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  return priv->controller;
  }

/*=============================================================================
  boilerplatemainwindow_get_main_widget
=============================================================================*/
GtkWidget *boilerplatemainwindow_get_main_widget 
           (const BoilerplateMainWindow *self)
  {
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  return priv->main_widget;
  }

/*=============================================================================
  _boilerplatemainwindow_on_window_delete
=============================================================================*/
static void _boilerplatemainwindow_on_window_delete 
    (BoilerplateMainWindow *self)
  {
  g_debug ("%s: close clicked", __PRETTY_FUNCTION__);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  BOILERPLATECONTROLLER_GET_CLASS (priv->controller)
      ->action_close_window (priv->controller); 
  } 

/*=============================================================================
  boilerplatemainwindow_on_window_delete
=============================================================================*/
static gboolean boilerplatemainwindow_on_window_delete (GtkWidget *w, 
  GdkEvent *e, gpointer user_data)
  {
  g_debug ("%s: close clicked", __PRETTY_FUNCTION__);
  BoilerplateMainWindow *self = BOILERPLATEMAINWINDOW (user_data);
  BOILERPLATEMAINWINDOW_GET_CLASS(self)->on_window_delete (self);
  return TRUE;
  } 

/*=============================================================================
  _boilerplatemainwindow_create_main_menu
=============================================================================*/
static GtkMenuBar* _boilerplatemainwindow_create_main_menu 
          (BoilerplateMainWindow *self, GtkAccelGroup *accel_group,
	   BoilerplateController *controller)
  {
  g_debug ("%s: creating main menu", __PRETTY_FUNCTION__);

  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);

  BoilerplateMenuBar *menuBar = boilerplatemenubar_new 
      (accel_group, controller, priv->model, self);
  gtk_widget_show_all (GTK_WIDGET (menuBar));

  g_debug ("%s: created main menu", __PRETTY_FUNCTION__);
  return GTK_MENU_BAR (menuBar);
  }

/*=============================================================================
  _boilerplatemainwindow_model_file_changed

  Update the main window caption to indicate whether a 
  file is open or not.
==============================================================================*/
void _boilerplatemainwindow_model_file_changed (BoilerplateMainWindow *self)
  {
  g_debug ("%s: enter", __PRETTY_FUNCTION__);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  const char *current_file = 
    boilerplatemodel_get_current_filename (priv->model);
  if (current_file)
     {
     char *caption = NULL;
     GString *short_name = stringutil_get_filename (current_file);
     asprintf (&caption, "%s: %s", NAME, short_name->str);
     gtk_window_set_title (GTK_WINDOW (self), caption);
     g_string_free (short_name, TRUE);
     free (caption);
     }
  else
     {
     gtk_window_set_title (GTK_WINDOW (self), NAME);
     }
  g_debug ("%s: exit", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  boilerplatemainwindow_model_file_changed
==============================================================================*/
void boilerplatemainwindow_model_file_changed (BoilerplateMainWindow *self)
  {
  BOILERPLATEMAINWINDOW_GET_CLASS(self)->model_file_changed (self);
  }

/*=============================================================================
  _boilerplatemainwindow_create_main_widget
=============================================================================*/
static GtkWidget *_boilerplatemainwindow_create_main_widget 
        (BoilerplateMainWindow *self)
  {
  GtkWidget *l = gtk_label_new ("foo");
  return l;
  }

/*=============================================================================
  _boilerplatemainwindow_create_controller
=============================================================================*/
static BoilerplateController *_boilerplatemainwindow_create_controller 
        (BoilerplateMainWindow *self, 
         BoilerplateApplication *application)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  BoilerplateController *controller = boilerplatecontroller_new (self,
     application, priv->model); 
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  return controller;
  }

/*=============================================================================
  _boilerplatemainwindow_create_model
=============================================================================*/
static BoilerplateModel *_boilerplatemainwindow_create_model
        (BoilerplateMainWindow *self)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  BoilerplateModel *model = boilerplatemodel_new ();
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  return model;
  }

/*============================================================================
  _boilerplatemainwindow_statusbar_clear
=============================================================================*/
void _boilerplatemainwindow_statusbar_clear
    (const BoilerplateMainWindow *self) 
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  if (priv->statusbar)
    gtk_statusbar_remove_all (GTK_STATUSBAR(priv->statusbar), 0);
  }

/*============================================================================
  boilerplatemainwindow_statusbar_clear
=============================================================================*/
void boilerplatemainwindow_statusbar_clear
           (const BoilerplateMainWindow *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BOILERPLATEMAINWINDOW_GET_CLASS (self)->statusbar_clear (self);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  if (priv->timer) g_source_remove (priv->timer);
  }

/*============================================================================
  _boilerplatemainwindow_statusbar_timeout
=============================================================================*/
gboolean _boilerplatemainwindow_statusbar_timeout (gpointer user_data)
  {
  BoilerplateMainWindow *self = BOILERPLATEMAINWINDOW (user_data);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  boilerplatemainwindow_statusbar_clear (self);
  priv->timer = 0;
  return FALSE;
  }

/*============================================================================
  _boilerplatemainwindow_statusbar_message
=============================================================================*/
void _boilerplatemainwindow_statusbar_message
    (BoilerplateMainWindow *self, const char *message) 
  {
  g_debug ("%s: %s", __PRETTY_FUNCTION__, message);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  if (priv->statusbar)
    {
    if (priv->timer) g_source_remove (priv->timer);
    gtk_statusbar_push (GTK_STATUSBAR(priv->statusbar), 0, message);
    int timer = g_timeout_add_seconds 
      (3, _boilerplatemainwindow_statusbar_timeout, self);
    priv->timer = timer;
    }
  }

/*============================================================================
  boilerplatemainwindow_statusbar_message
=============================================================================*/
void boilerplatemainwindow_statusbar_message 
           (BoilerplateMainWindow *self, const char *message)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BOILERPLATEMAINWINDOW_GET_CLASS (self)->statusbar_message (self, message);
  }


/*============================================================================
  _boilerplatemainwindow_create_statusbar
=============================================================================*/
GtkWidget *_boilerplatemainwindow_create_statusbar 
    (BoilerplateMainWindow *self, const BoilerplateModel *model, 
     const BoilerplateController *controller)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  GtkWidget *sb = gtk_statusbar_new (); 
  // Remove the pointless default padding that applies to statusbars and
  //   which can't, so far as I can tell, be overcome in CSS
  gtk_widget_set_margin_top (GTK_WIDGET(sb), 0);
  gtk_widget_set_margin_bottom (GTK_WIDGET(sb), 0);
  gtk_widget_set_margin_start (GTK_WIDGET(sb), 0);
  gtk_widget_set_margin_end (GTK_WIDGET(sb), 0);
  return sb;
  }


/*=============================================================================
  _boilerplatemainwindow_setup
=============================================================================*/
static void _boilerplatemainwindow_setup (BoilerplateMainWindow *self, 
    BoilerplateApplication *application)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  priv->application = application;

  BoilerplateModel *model = 
      BOILERPLATEMAINWINDOW_GET_CLASS (self)
      ->create_model (self);
  priv->model = model;

  BoilerplateController *controller = 
      BOILERPLATEMAINWINDOW_GET_CLASS (self)
      ->create_controller (self, application);
  priv->controller = controller;

  g_signal_connect (G_OBJECT(self), "delete-event",
     G_CALLBACK (boilerplatemainwindow_on_window_delete), self);       

  GtkGrid *main_box = GTK_GRID (gtk_grid_new ());
  gtk_widget_set_name (GTK_WIDGET(main_box), "boilerplateclient");

  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group (GTK_WINDOW(self), accel_group);
  GtkMenuBar *menu_bar = BOILERPLATEMAINWINDOW_GET_CLASS (self)
         ->create_main_menu (self, accel_group, controller);

  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (main_box));

  gtk_grid_attach (main_box, GTK_WIDGET (menu_bar),
        1, 1, 1, 1);
  gtk_widget_set_hexpand (GTK_WIDGET(menu_bar), TRUE);
  
  GtkWidget *main_widget = BOILERPLATEMAINWINDOW_GET_CLASS (self)->
    create_main_widget (self);
  gtk_widget_set_vexpand (main_widget, TRUE);
  gtk_grid_attach (main_box, main_widget, 1, 2, 1, 1);

  priv->statusbar = BOILERPLATEMAINWINDOW_GET_CLASS (self)->
    create_statusbar (self, priv->model, priv->controller);
  if (priv->statusbar)
    {
    gtk_grid_attach (main_box, priv->statusbar, 1, 3, 1, 1);
    gtk_widget_set_name (GTK_WIDGET(self), "statusbar");
    }

  priv->main_widget = main_widget;

  g_debug ("%s: out", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  boilerplatemainwindow_init
=============================================================================*/
static void boilerplatemainwindow_init (BoilerplateMainWindow *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  BoilerplateMainWindowPriv *priv = BOILERPLATEMAINWINDOW_PRIV (self);
  priv->timer = 0;
  }

/*=============================================================================
  boilerplatemainwindow_class_init
=============================================================================*/
static void boilerplatemainwindow_class_init
  (BoilerplateMainWindowClass *klass)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (BoilerplateMainWindowPriv));
  klass->create_main_widget = _boilerplatemainwindow_create_main_widget;
  klass->setup = _boilerplatemainwindow_setup;
  klass->create_controller = _boilerplatemainwindow_create_controller;
  klass->create_main_menu = _boilerplatemainwindow_create_main_menu;
  klass->create_model = _boilerplatemainwindow_create_model;
  klass->model_file_changed = _boilerplatemainwindow_model_file_changed;
  klass->on_window_delete = _boilerplatemainwindow_on_window_delete;
  klass->create_statusbar= _boilerplatemainwindow_create_statusbar;
  klass->statusbar_message = _boilerplatemainwindow_statusbar_message;
  klass->statusbar_clear = _boilerplatemainwindow_statusbar_clear;
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  boilerplatemainwindow_new
=============================================================================*/
BoilerplateMainWindow *boilerplatemainwindow_new (void)
  {
  BoilerplateMainWindow *self = g_object_new 
     (BOILERPLATEMAINWINDOW_TYPE, NULL);
  return self;
  }

