/*===========================================================================
  
  gsxradio
  StationListWindow.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <string.h>
#include "BoilerplateApplication.h"
#include "StationListWindow.h"
#include "StationListController.h"
#include "StationListMenuBar.h"
#include "StationListModel.h"
#include "StreamInfoView.h"
#include "RadioApplication.h"

#define STATIONLISTWINDOW_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          STATIONLISTWINDOW_TYPE, StationListWindowPriv))

typedef struct _StationListWindowPriv StationListWindowPriv;

struct _StationListWindowPriv
  {
  GtkTreeView *tree_view;
  StreamInfoView *stream_info_view;
  StationListController *controller;
  StationListModel *model;
  };

G_DEFINE_TYPE (StationListWindow, stationlistwindow, BOILERPLATEMAINWINDOW_TYPE);

/*===========================================================================
  stationlistwindow_get_tree_view 
===========================================================================*/
GtkTreeView *stationlistwindow_get_tree_view (const StationListWindow *self)
  {
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  return priv->tree_view;
  }

/*===========================================================================
  _stationlistwindow_create_controller
===========================================================================*/
static BoilerplateController *_stationlistwindow_create_controller 
        (BoilerplateMainWindow *self, 
         BoilerplateApplication *application)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  BoilerplateModel *model = boilerplatemainwindow_get_model (self);
  StationListController *controller = stationlistcontroller_new 
    (BOILERPLATEMAINWINDOW (self), application, model); 
  priv->controller = controller;
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  return BOILERPLATECONTROLLER (controller);
  }

/*===========================================================================
  _stationlistwindow_create_model
===========================================================================*/
static BoilerplateModel *_stationlistwindow_create_model
        (BoilerplateMainWindow *self)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  StationListModel *model = stationlistmodel_new ();
  g_debug ("%s: out", __PRETTY_FUNCTION__);
  priv->model = model;
  return BOILERPLATEMODEL(model);
  }

/*===========================================================================
  _stationlistwindow_create_main_menu
===========================================================================*/
static GtkMenuBar* _stationlistwindow_create_main_menu 
          (BoilerplateMainWindow *self, GtkAccelGroup *accel_group,
	   BoilerplateController *controller)
  {
  BoilerplateModel *model = boilerplatemainwindow_get_model (self);
  StationListMenuBar *menu_bar = stationlistmenubar_new 
     (accel_group, controller, model, self);
  return GTK_MENU_BAR(menu_bar);
  }

/*===========================================================================
  stationlistwindow_is_item_selected
===========================================================================*/
gboolean stationlistwindow_is_item_selected (const StationListWindow *self)
  {
  gboolean ret = FALSE;
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  GtkTreeIter iter;
  GtkTreeSelection *selection 
    = gtk_tree_view_get_selection (priv->tree_view);
  GtkTreeModel *m;
  if (gtk_tree_selection_get_selected (selection, &m, &iter))
    {
    ret = TRUE;
    }
  else
    {
    ret = FALSE;
    }
  return ret;
  }

/*===========================================================================
  stationlistwindow_on_row_activated
===========================================================================*/
void stationlistwindow_on_row_activated (GtkTreeView *tree_view, 
      GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
  {
  StationListWindow *self = STATIONLISTWINDOW (user_data);
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  GtkTreeIter iter;
  GtkTreeModel *model;
  gchar *uri;
  gchar *name;

  GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
    gtk_tree_model_get (model, &iter, URI_COLUMN, &uri, -1);
    gtk_tree_model_get (model, &iter, NAME_COLUMN, &name, -1);

    stationlistcontroller_action_play_station (priv->controller, 
        name, uri);
    
    g_free (uri);
    g_free (name);
    }
  }

/*============================================================================
  stationlistwindow_show_disconnected
============================================================================*/
void stationlistwindow_show_disconnected (StationListWindow *self)
  {
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  streaminfoview_show_disconnected (priv->stream_info_view);
  }

/*============================================================================
  stationlistwindow_update_stream_view
============================================================================*/
void stationlistwindow_update_stream_view (StationListWindow *self,
       const XSStatus *status, const XSMetaInfo *mi)
  {
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  streaminfoview_show_info (priv->stream_info_view, status, mi);
  }

/*===========================================================================
  stationlistwindow_on_item_toggled
===========================================================================*/
void stationlistwindow_set_station (StationListWindow *self, 
       const char *station)
  {
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  streaminfoview_set_station (priv->stream_info_view, station);
  }

/*===========================================================================
  stationlistwindow_on_item_toggled
===========================================================================*/
void stationlistwindow_on_item_toggled (GtkCellRendererToggle *cell_renderer,
      gchar *path, gpointer user_data)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StationListWindow *self = STATIONLISTWINDOW (user_data);
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);

  GtkTreeModel *list_store = gtk_tree_view_get_model (priv->tree_view); 

  GtkTreeIter iter;
  gtk_tree_model_get_iter_from_string (list_store, &iter, path);
  gboolean ok = FALSE;
  gtk_tree_model_get (list_store, &iter, SELECTED_COLUMN, &ok, -1);

  gtk_list_store_set (GTK_LIST_STORE (list_store), &iter, 
    SELECTED_COLUMN, !ok, -1);

  // Here we see why GTK can be so ugly... how many casts are
  //   needed in one simple method call?
  BoilerplateModel *model = boilerplatemainwindow_get_model 
     (BOILERPLATEMAINWINDOW (self));
  stationlistmodel_set_need_save (STATIONLISTMODEL (model));
  }

/*=============================================================================
  _stationlistwindow_model_file_changed

  Update the main window caption to indicate whether a 
  file is open or not.
==============================================================================*/
void _stationlistwindow_model_file_changed (BoilerplateMainWindow *self)
  {
  g_debug ("%s: enter", __PRETTY_FUNCTION__);
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);

  BOILERPLATEMAINWINDOW_CLASS(stationlistwindow_parent_class)
      ->model_file_changed (self); 
  const char *current_file = 
        boilerplatemodel_get_current_filename (BOILERPLATEMODEL (priv->model));
  if (current_file)
     {
     char *pathname = canonicalize_file_name (current_file);
     if (pathname)
       {
       BoilerplateController *controller = 
          boilerplatemainwindow_get_controller (self);

       BoilerplateApplication *application = 
          boilerplatecontroller_get_application (controller);

       char *_pathname = strdup (pathname);
       radioapplication_set_last_pathname 
          (RADIOAPPLICATION (application), _pathname);
       free (_pathname);

       radioapplication_save_user_rc (RADIOAPPLICATION (application));
 
       free (pathname);
       }
     }
  g_debug ("%s: exit", __PRETTY_FUNCTION__);
  }

/*===========================================================================
  stationlistwindow_set_single_click_mode
===========================================================================*/
void stationlistwindow_set_single_click_mode (StationListWindow *self,
       gboolean single)
  {
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  gtk_tree_view_set_activate_on_single_click (priv->tree_view, single);
  }

/*===========================================================================
  _stationlistwindow_create_main_widget
===========================================================================*/
static GtkWidget* _stationlistwindow_create_main_widget
          (BoilerplateMainWindow *self)
  {
  g_debug ("%s: creating main widget", __PRETTY_FUNCTION__);
  
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);

  BoilerplateController *controller = 
    boilerplatemainwindow_get_controller (self);

  BoilerplateApplication *application = 
    boilerplatecontroller_get_application (controller);

  GtkListStore *list_model = stationlistmodel_get_list_model
        (STATIONLISTMODEL (boilerplatemainwindow_get_model(self)));
  GtkTreeView *v = GTK_TREE_VIEW 
        (gtk_tree_view_new_with_model (GTK_TREE_MODEL (list_model)));

  gtk_widget_set_name (GTK_WIDGET(v), "stationlisttreeview");

  gboolean single = radioapplication_get_single_click_mode 
       (RADIOAPPLICATION (application));
  gtk_tree_view_set_activate_on_single_click (v, single);

  GtkCellRenderer *renderer0 = gtk_cell_renderer_text_new ();
  g_object_set (renderer0, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
  GtkTreeViewColumn *column0 = gtk_tree_view_column_new_with_attributes
    (MODEL_COLUMN_0_TITLE, renderer0, "text", 0, NULL);
  gtk_tree_view_column_set_sort_column_id (column0, 0);
  gtk_tree_view_column_set_min_width (column0, 400);
  gtk_tree_view_column_set_resizable (column0, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (v), column0);

  GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new ();
  GtkTreeViewColumn *column1 = gtk_tree_view_column_new_with_attributes
    (MODEL_COLUMN_1_TITLE, renderer1, "text", 1, NULL);
  gtk_tree_view_column_set_sort_column_id (column1, 1);
  gtk_tree_view_column_set_resizable (column1, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (v), column1);

  if (!radioapplication_is_readonly (RADIOAPPLICATION(application)))
    {
    GtkCellRenderer *renderer3 = gtk_cell_renderer_toggle_new ();
    GtkTreeViewColumn *column3 = gtk_tree_view_column_new_with_attributes
      (MODEL_COLUMN_3_TITLE, renderer3, "active", 3, NULL);
    gtk_tree_view_column_set_sort_column_id (column3, 3);
    gtk_cell_renderer_set_alignment (renderer3, 0, 0);
    gtk_tree_view_append_column (GTK_TREE_VIEW (v), column3);
    g_signal_connect (G_OBJECT (renderer3), "toggled", 
      G_CALLBACK (stationlistwindow_on_item_toggled), self);
    }

  g_signal_connect (G_OBJECT (v), "row-activated", 
    G_CALLBACK (stationlistwindow_on_row_activated), self);

  priv->tree_view = v;
  GtkScrolledWindow *s = GTK_SCROLLED_WINDOW 
    (gtk_scrolled_window_new (NULL, NULL));
  gtk_scrolled_window_set_shadow_type (s, GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (s), GTK_WIDGET (v));
  gtk_widget_set_name (GTK_WIDGET(s), "stationlistscroller");

  StreamInfoView *stream_info_view = 
     streaminfoview_new ();
  priv->stream_info_view = stream_info_view;
  streaminfoview_show_disconnected (stream_info_view);

  GtkGrid *grid = GTK_GRID (gtk_grid_new ());
  gtk_grid_attach (grid, GTK_WIDGET (stream_info_view),
        1, 1, 1, 1);
  gtk_grid_attach (grid, GTK_WIDGET (s),
        1, 2, 1, 1);
  gtk_widget_set_hexpand (GTK_WIDGET(stream_info_view), TRUE);
  gtk_widget_set_hexpand (GTK_WIDGET(v), TRUE);
  gtk_widget_set_vexpand (GTK_WIDGET(v), TRUE);

  gtk_widget_set_name (GTK_WIDGET(grid), "stationlistmainwidget");

  return GTK_WIDGET (grid);
  }

/*===========================================================================
  stationlistwindow_init
===========================================================================*/
static void stationlistwindow_init (StationListWindow *self)
  {
  g_debug ("Initialising main window");
  }

/*=============================================================================
  stationlistwindow_dispose
=============================================================================*/
static void stationlistwindow_dispose (GObject *self)
  {
  StationListWindowPriv *priv = STATIONLISTWINDOW_PRIV (self);
  if (priv->controller)
    {
    g_object_unref (priv->controller);
    priv->controller = NULL;
    }
  if (priv->model)
    {
    g_object_unref (priv->model);
    priv->model = NULL;
    }
  priv->controller = NULL; 
  G_OBJECT_CLASS (stationlistwindow_parent_class)->dispose (self);
  }

/*===========================================================================
  stationlistwindow_class_init
===========================================================================*/

static void stationlistwindow_class_init
  (StationListWindowClass *klass)
  {
  g_type_class_add_private (klass, sizeof (StationListWindowPriv));
  BoilerplateMainWindowClass *parent = 
       BOILERPLATEMAINWINDOW_CLASS (klass);
  parent->create_main_widget = _stationlistwindow_create_main_widget;
  parent->create_controller = _stationlistwindow_create_controller;
  parent->create_main_menu = _stationlistwindow_create_main_menu;
  parent->create_model = _stationlistwindow_create_model;
  parent->model_file_changed = _stationlistwindow_model_file_changed;
  G_OBJECT_CLASS (klass)->dispose = stationlistwindow_dispose;
  }


/*===========================================================================
  stationlistwindow_new
===========================================================================*/
StationListWindow *stationlistwindow_new (void)
  {
  StationListWindow *self = g_object_new 
     (stationlistwindow_get_type(), NULL);
  gtk_widget_set_name (GTK_WIDGET(self), "stationlistwindow");
  return self;
  }


