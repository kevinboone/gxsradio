/*=============================================================================
  
  gxsradio
  StationListMenuBar.c

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
#include "RadioApplication.h"
#include "StationListMenuBar.h"
#include "StationListWindow.h"
#include "BoilerplateController.h"
#include "BoilerplateMainWindow.h"
#include "StationListController.h"

#define STATIONLISTMENUBAR_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          STATIONLISTMENUBAR_TYPE, StationListMenuBarPriv))

typedef struct _StationListMenuBarPriv StationListMenuBarPriv;

struct _StationListMenuBarPriv
  {
  BoilerplateController *controller;
  GtkMenu *edit_menu;
  GtkMenu *audio_menu;
  };

G_DEFINE_TYPE (StationListMenuBar, stationlistmenubar, BOILERPLATEMENUBAR_TYPE);


/*=============================================================================
  stationlistmenubar_init
=============================================================================*/
static void stationlistmenubar_init (StationListMenuBar *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  stationlistmenubar_on_menu_delete_entry
=============================================================================*/
void stationlistmenubar_on_menu_delete_entry (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_delete_entry (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_add_entry
=============================================================================*/
void stationlistmenubar_on_menu_add_entry (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_add_entry (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_preferences
=============================================================================*/
void stationlistmenubar_on_menu_preferences (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_preferences (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_connect
=============================================================================*/
void stationlistmenubar_on_menu_connect (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_prompt_connect (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_play
=============================================================================*/
void stationlistmenubar_on_menu_play (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_play (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_pause
=============================================================================*/
void stationlistmenubar_on_menu_pause (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_pause (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_stop
=============================================================================*/
void stationlistmenubar_on_menu_stop (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_stop (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_vol_up
=============================================================================*/
void stationlistmenubar_on_menu_vol_up (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_vol_up (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_vol_down
=============================================================================*/
void stationlistmenubar_on_menu_vol_down (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_vol_down (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_save_selected_down
=============================================================================*/
void stationlistmenubar_on_menu_save_selected (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_save_selected (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_select_all
=============================================================================*/
void stationlistmenubar_on_menu_select_all (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_select_all (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_select_none
=============================================================================*/
void stationlistmenubar_on_menu_select_none (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_select_none (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_select_invert
=============================================================================*/
void stationlistmenubar_on_menu_select_invert (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_select_invert (STATIONLISTCONTROLLER (priv->controller));
  }

/*=============================================================================
  stationlistmenubar_on_menu_edit_station
=============================================================================*/
void stationlistmenubar_on_menu_edit_station (GtkWidget *w, gpointer data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  STATIONLISTCONTROLLER_GET_CLASS (priv->controller)
    ->action_edit_entry (STATIONLISTCONTROLLER (priv->controller));
  }

/*==============================================================================
  _stationlistmenubar_update_from_model
==============================================================================*/
void _stationlistmenubar_update_from_model (BoilerplateMenuBar *self)
  {
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);

  StationListWindow *main_window 
    = STATIONLISTWINDOW (boilerplatemenubar_get_main_window (self));

  GtkMenuItem *deleteMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->edit_menu, SMB_MENU_DELETE);
  GtkMenuItem *editMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->edit_menu, SMB_MENU_EDIT_STATION);

  RadioApplication *application = RADIOAPPLICATION 
     (boilerplatecontroller_get_application (priv->controller));

  if (!radioapplication_is_readonly (application))
    {
    if (stationlistwindow_is_item_selected (main_window))
      {
      gtk_widget_set_sensitive (GTK_WIDGET (deleteMenuItem), TRUE); 
      gtk_widget_set_sensitive (GTK_WIDGET (editMenuItem), TRUE); 
      }
    else
      {
      gtk_widget_set_sensitive (GTK_WIDGET (deleteMenuItem), FALSE); 
      gtk_widget_set_sensitive (GTK_WIDGET (editMenuItem), FALSE); 
      } 
    }

  BOILERPLATEMENUBAR_CLASS (stationlistmenubar_parent_class)
     ->update_from_model (self) ;

  if (radioapplication_is_readonly (application))
    {
    GtkMenu *file_menu = boilerplatemenubar_get_file_menu 
      (BOILERPLATEMENUBAR (self));
    GtkMenuItem *fileSaveAsMenuItem = boilerplatemenubar_menuitem_from_label 
      (file_menu, BMB_MENU_SAVE_AS);
    GtkMenuItem *fileSaveMenuItem = boilerplatemenubar_menuitem_from_label 
      (file_menu, BMB_MENU_SAVE);

    gtk_widget_set_sensitive (GTK_WIDGET (fileSaveAsMenuItem), FALSE); 
    gtk_widget_set_sensitive (GTK_WIDGET (fileSaveMenuItem), FALSE); 
    }
  }

/*=============================================================================
  stationlistmenubar_update_from_controller
=============================================================================*/
void stationlistmenubar_update_from_controller (StationListMenuBar *self)
  {
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);

  GtkMenuItem *playMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->audio_menu, SMB_MENU_PLAY);

  if (stationlistcontroller_can_play 
       (STATIONLISTCONTROLLER (priv->controller)))
    gtk_widget_set_sensitive (GTK_WIDGET (playMenuItem), TRUE); 
  else
    gtk_widget_set_sensitive (GTK_WIDGET (playMenuItem), FALSE); 

  GtkMenuItem *pauseMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->audio_menu, SMB_MENU_PAUSE);

  if (stationlistcontroller_can_pause
       (STATIONLISTCONTROLLER (priv->controller)))
    gtk_widget_set_sensitive (GTK_WIDGET (pauseMenuItem), TRUE); 
  else
    gtk_widget_set_sensitive (GTK_WIDGET (pauseMenuItem), FALSE); 

  GtkMenuItem *stopMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->audio_menu, SMB_MENU_STOP);

  if (stationlistcontroller_can_stop
       (STATIONLISTCONTROLLER (priv->controller)))
    gtk_widget_set_sensitive (GTK_WIDGET (stopMenuItem), TRUE); 
  else
    gtk_widget_set_sensitive (GTK_WIDGET (stopMenuItem), FALSE); 

  GtkMenuItem *volumeUpMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->audio_menu, SMB_MENU_VOL_UP);
  GtkMenuItem *volumeDownMenuItem = boilerplatemenubar_menuitem_from_label 
    (priv->audio_menu, SMB_MENU_VOL_DOWN);

  if (stationlistcontroller_is_connected
       (STATIONLISTCONTROLLER (priv->controller)))
    {
    gtk_widget_set_sensitive (GTK_WIDGET (volumeUpMenuItem), TRUE); 
    gtk_widget_set_sensitive (GTK_WIDGET (volumeDownMenuItem), TRUE); 
    }
  else
    {
    gtk_widget_set_sensitive (GTK_WIDGET (volumeUpMenuItem), FALSE); 
    gtk_widget_set_sensitive (GTK_WIDGET (volumeDownMenuItem), FALSE); 
    }
  }

/*=============================================================================
  stationlistmenubar_on_audio_menu_shown
=============================================================================*/
void stationlistmenubar_on_audio_menu_shown (GtkWidget *w, gpointer data)
  {
  StationListMenuBar *self = STATIONLISTMENUBAR (data);
  stationlistmenubar_update_from_controller (self);
  }

/*=============================================================================
  stationlistmenubar_constructed
=============================================================================*/
static void _stationlistmenubar_constructed (GObject *obj)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  G_OBJECT_CLASS (stationlistmenubar_parent_class)->constructed (obj);
  StationListMenuBar *self = STATIONLISTMENUBAR (obj);
  StationListMenuBarPriv *priv = STATIONLISTMENUBAR_PRIV (self);
  priv->controller = boilerplatemenubar_get_controller 
    (BOILERPLATEMENUBAR (self));

  gtk_widget_set_name (GTK_WIDGET(self), "stationlistmenubar");

  // Update stock file menu with additional items
  GtkMenu *file_menu = boilerplatemenubar_get_file_menu 
    (BOILERPLATEMENUBAR (self));
  GtkMenuItem *connectMenuItem = GTK_MENU_ITEM 
    (gtk_menu_item_new_with_mnemonic (SMB_MENU_CONNECT));
  g_signal_connect (G_OBJECT (connectMenuItem), "activate",
      G_CALLBACK (stationlistmenubar_on_menu_connect), self);
  gtk_menu_shell_insert (GTK_MENU_SHELL (file_menu), 
      GTK_WIDGET (connectMenuItem), 0);
  gtk_menu_shell_insert (GTK_MENU_SHELL (file_menu), 
      GTK_WIDGET (gtk_separator_menu_item_new()), 1);
  gtk_menu_shell_insert (GTK_MENU_SHELL (file_menu), 
      GTK_WIDGET (gtk_separator_menu_item_new()), 6);

  RadioApplication *application = RADIOAPPLICATION 
     (boilerplatecontroller_get_application (priv->controller));

  if (radioapplication_is_readonly (application))
    {
    }
  else 
    {
    GtkMenuItem *saveSelectedMenuItem = GTK_MENU_ITEM 
      (gtk_menu_item_new_with_mnemonic (SMB_MENU_SAVE_SELECTED));
    g_signal_connect (G_OBJECT (saveSelectedMenuItem), "activate",
        G_CALLBACK (stationlistmenubar_on_menu_save_selected), self);
    gtk_menu_shell_insert (GTK_MENU_SHELL (file_menu), 
        GTK_WIDGET (saveSelectedMenuItem), 6);
    }

  GtkAccelGroup *accel_group = boilerplatemenubar_get_accel_group 
    (BOILERPLATEMENUBAR (self)); 

  // Create edit menu
  GtkMenuItem *editMenuItem = GTK_MENU_ITEM 
    (gtk_menu_item_new_with_mnemonic (SMB_MENU_EDIT));
  GtkMenu *edit_menu = GTK_MENU (gtk_menu_new ());
  priv->edit_menu = edit_menu;

  g_signal_connect (G_OBJECT(edit_menu), "show",
     G_CALLBACK (boilerplatemenubar_on_menu_shown),  
       self);       

  if (!radioapplication_is_readonly (application))
    {
    boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
       accel_group, edit_menu, SMB_MENU_DELETE, 'x', 
       G_CALLBACK (stationlistmenubar_on_menu_delete_entry));

    boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
       accel_group, edit_menu, SMB_MENU_ADD, 'a', 
       G_CALLBACK (stationlistmenubar_on_menu_add_entry));

    boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
       accel_group, edit_menu, SMB_MENU_EDIT_STATION, 'e', 
       G_CALLBACK (stationlistmenubar_on_menu_edit_station));

    boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
       accel_group, edit_menu, SMB_MENU_SELECT_ALL, 0, 
       G_CALLBACK (stationlistmenubar_on_menu_select_all));

    boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
       accel_group, edit_menu, SMB_MENU_SELECT_NONE, 0, 
       G_CALLBACK (stationlistmenubar_on_menu_select_none));

    boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
       accel_group, edit_menu, SMB_MENU_SELECT_INVERT, 0, 
       G_CALLBACK (stationlistmenubar_on_menu_select_invert));

    //gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), 
    //  GTK_WIDGET (gtk_separator_menu_item_new()));
    }

  boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
     accel_group, edit_menu, SMB_MENU_PREFS, 0, 
     G_CALLBACK (stationlistmenubar_on_menu_preferences));
  
  // Edit menu add to bar 
  gtk_menu_item_set_submenu (editMenuItem, 
      GTK_WIDGET (edit_menu));
  gtk_menu_shell_insert (GTK_MENU_SHELL (self), 
      GTK_WIDGET (editMenuItem), 1);

  // Create audio menu
  GtkMenuItem *audioMenuItem = GTK_MENU_ITEM 
    (gtk_menu_item_new_with_mnemonic (SMB_MENU_AUDIO));
  GtkMenu *audio_menu = GTK_MENU (gtk_menu_new ());
  priv->audio_menu = audio_menu;

  g_signal_connect (G_OBJECT(audio_menu), "show",
     G_CALLBACK (stationlistmenubar_on_audio_menu_shown),  
       self);       

  boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
     accel_group, audio_menu, SMB_MENU_PLAY, 0, 
     G_CALLBACK (stationlistmenubar_on_menu_play));
  
  boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
     accel_group, audio_menu, SMB_MENU_PAUSE, 0, 
     G_CALLBACK (stationlistmenubar_on_menu_pause));
  
  boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
     accel_group, audio_menu, SMB_MENU_STOP, 0, 
     G_CALLBACK (stationlistmenubar_on_menu_stop));
  
  gtk_menu_shell_append (GTK_MENU_SHELL (audio_menu), 
      GTK_WIDGET (gtk_separator_menu_item_new()));
  
  boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
     accel_group, audio_menu, SMB_MENU_VOL_UP, 'u', 
     G_CALLBACK (stationlistmenubar_on_menu_vol_up));
  
  boilerplatemenubar_append_menu_item (BOILERPLATEMENUBAR (self), 
     accel_group, audio_menu, SMB_MENU_VOL_DOWN, 'd', 
     G_CALLBACK (stationlistmenubar_on_menu_vol_down));
  
  // Audio menu add to bar 
  gtk_menu_item_set_submenu (audioMenuItem, 
      GTK_WIDGET (audio_menu));
  gtk_menu_shell_insert (GTK_MENU_SHELL (self), 
      GTK_WIDGET (audioMenuItem), 2);

  }

/*=============================================================================
  stationlistmenubar_class_init
=============================================================================*/
static void stationlistmenubar_class_init
  (StationListMenuBarClass *klass)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (StationListMenuBarPriv));
  G_OBJECT_CLASS (klass)->constructed = _stationlistmenubar_constructed; 
  BoilerplateMenuBarClass *parent = 
       BOILERPLATEMENUBAR_CLASS (klass);
  parent->update_from_model = _stationlistmenubar_update_from_model;
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  stationlistmenubar_new
=============================================================================*/
StationListMenuBar *stationlistmenubar_new (GtkAccelGroup *accel_group,
     BoilerplateController *controller, BoilerplateModel *model,
     BoilerplateMainWindow *w)
  {
  StationListMenuBar *self = g_object_new (STATIONLISTMENUBAR_TYPE, 
    "accel-group", accel_group, "controller", controller, "model", model, 
    "main-window", w, NULL);
  return self;
  }

