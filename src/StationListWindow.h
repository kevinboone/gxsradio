/*=============================================================================
  
  gxsradio
  StationListWindow.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateMainWindow.h" 
#include "xine-server-api.h"

G_BEGIN_DECLS

#define STATIONLISTWINDOW_TYPE            (stationlistwindow_get_type ())
#define STATIONLISTWINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), STATIONLISTWINDOW_TYPE, StationListWindow))
#define STATIONLISTWINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), STATIONLISTWINDOW_TYPE, StationListWindowClass))
#define IS_STATIONLISTWINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STATIONLISTWINDOW_TYPE))
#define IS_STATIONLISTWINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), STATIONLISTWINDOW_TYPE))
#define STATIONLISTWINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), STATIONLISTWINDOW_TYPE, StationListWindowClass))


typedef struct _StationListWindow StationListWindow;
typedef struct _StationListWindowClass  StationListWindowClass;
typedef struct _StationListWindowPriv  StationListWindowPriv;

#define MW_MENU_SELECT_ALL "Select _all"

struct _StationListWindow 
  {
  BoilerplateMainWindow base;
  };

struct _StationListWindowClass
  {
  BoilerplateMainWindowClass parent_class;
  gpointer padding[12];
  };

GType stationlistwindow_get_type (void);

StationListWindow *stationlistwindow_new (void);

gboolean          stationlistwindow_is_item_selected 
                     (const StationListWindow *self);

GtkTreeView      *stationlistwindow_get_tree_view
                     (const StationListWindow *self);

/** Set up the display to indicate that there is no server connection */
void stationlistwindow_show_disconnected (StationListWindow *self);

/** Update the display with status information from the server */
void stationlistwindow_update_stream_view (StationListWindow *self,
       const XSStatus *status, const XSMetaInfo *mi);

/** Set up the display to indcate the name of a station being played.
    This is purely for display */
void stationlistwindow_set_station (StationListWindow *self, 
       const char *station);

void stationlistwindow_set_single_click_mode (StationListWindow *self,
       gboolean single);

G_END_DECLS



