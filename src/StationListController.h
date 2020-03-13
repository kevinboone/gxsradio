/*============================================================================
  
  gxsradio
  StationListController.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateController.h" 

G_BEGIN_DECLS


#define STATIONLISTCONTROLLER_TYPE            (stationlistcontroller_get_type ())
#define STATIONLISTCONTROLLER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), STATIONLISTCONTROLLER_TYPE, StationListController))
#define STATIONLISTCONTROLLER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), STATIONLISTCONTROLLER_TYPE, StationListControllerClass))
#define IS_STATIONLISTCONTROLLER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STATIONLISTCONTROLLER_TYPE))
#define IS_STATIONLISTCONTROLLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), STATIONLISTCONTROLLER_TYPE))
#define STATIONLISTCONTROLLER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), STATIONLISTCONTROLLER_TYPE, StationListControllerClass))

typedef struct _StationListController StationListController;
typedef struct _StationListControllerClass  StationListControllerClass;

struct _BoilerplateApplication;
struct _BoilerplateMainWindow;
struct _BoilerplateModel;

/** StationListController controls the interaction between a the
    user interface (StationListWindow and StationListMenuBar) and 
    the station list data file (managed by StationListModel). It
    acts as a facade to the audio management functions provided by
    the xine-server API. */
struct _StationListController
  {
  BoilerplateController base;
  };

struct _StationListControllerClass
  {
  BoilerplateControllerClass parent_class;

  /** Delete the currently-selected entry from the station list model. */
  void  (*action_delete_entry) (const StationListController *self);

  /** Edit the currently-selected entry by raising a dialog box, and
      storing any changes back in the model. */
  void  (*action_edit_entry) (const StationListController *self);

  /** Raise a dialog box to allow the user to enter new station details,
      then store the changes in the model. */
  void  (*action_add_entry) (const StationListController *self);

  /** Prompt the user for connection details for a xine-server
      server. If the user confirms, call action_connect to make
      the connection. */
  void  (*action_prompt_connect) (StationListController *self);

  /** Connect to the specified server. In fact, all this method does
      is test that the connection can be made. If it can, the
      host/port settings are saved for later user. Otherwise and
      error dialog is raised. xine-server connections are not long-lived,
      and 'connect' is a bit of a misnomer here. */

  void  (*action_connect) (StationListController *self,
            const char *host, int port);

  /** Play the station with the specified name and URI. The URI is
      passed to the xine-server API, while the name is used to update
      the StationListWindow. We can always get the name from the URI
      by querying the model but, since we have it already when this
      method is called, we can avoid that small overhead. */
  void  (*action_play_station) (StationListController *self,
            const char *name, const char *uri);

  /** User selected the Play menu. Send a play command to xine-server. */
  void  (*action_play) (const StationListController *self);

  /** User selected the Pause menu. Send a pause command to xine-server. */
  void  (*action_pause) (const StationListController *self);

  /** User selected the Stop menu. Send a stop command to xine-server. */
  void  (*action_stop) (const StationListController *self);

  /** User selected the volume up menu. Send a volume up command 
      to xine-server. */
  void  (*action_vol_up) (const StationListController *self);

  /** User selected the volume down menu. Send a volume down command 
      to xine-server. */
  void  (*action_vol_down) (const StationListController *self);

  /** Raise the preferences dialog box and, if confirmed, send the
      changes to the RadioApplication for storage and processing. */
  void  (*action_preferences) (const StationListController *self);

  /** Raise a Save as... dialog box, then call the save_selected
      function in the model. */
  void  (*action_save_selected) (const StationListController *self);

  /** Set the 'selected' flag in all rows in the model, which will cause
      the station list UI to update. */
  void  (*action_select_all) (const StationListController *self);

  /** Reset the 'selected' flag in all rows in the model, which will cause
      the station list UI to update. */
  void  (*action_select_none) (const StationListController *self);

  /** Invert the 'selected' flag in all rows in the model, which will cause
      the station list UI to update. */
  void  (*action_select_invert) (const StationListController *self);
  gpointer padding[12];
  };

GType stationlistcontroller_get_type (void) G_GNUC_CONST;

StationListController *stationlistcontroller_new 
                   (struct _BoilerplateMainWindow *w, 
                   struct _BoilerplateApplication *application,
		   struct _BoilerplateModel *model);

void stationlistcontroller_action_play_station (StationListController *self,
       const char *name, const char *uri);

gboolean stationlistcontroller_can_play (const StationListController *self);
gboolean stationlistcontroller_can_pause (const StationListController *self);
gboolean stationlistcontroller_can_stop (const StationListController *self);
gboolean stationlistcontroller_is_connected (const StationListController *self);

G_END_DECLS





