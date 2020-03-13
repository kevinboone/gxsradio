/*==============================================================================
  
  gxsradio
  StationListController.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the 
  GPL, v3.0

==============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "StationListController.h"
#include "StationListController.h"
#include "boilerplatedialogs.h"
#include "StationListWindow.h"
#include "StationListModel.h"
#include "BoilerplateApplication.h"
#include "ConnectDialog.h"
#include "RadioApplication.h"
#include "PrefsDialog.h"
#include "EditStationDialog.h"
#include "FixedDirFilePickerDialog.h"
#include "xine-server-api.h"

#define NOT_CONNECTED_MSG "Not connected to server"

#define STATIONLISTCONTROLLER_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          STATIONLISTCONTROLLER_TYPE, StationListControllerPriv))

typedef struct _StationListControllerPriv StationListControllerPriv;

struct _StationListControllerPriv
  {
  char *host;
  int port;
  int update_timer;
  XSTransportStatus transport_status; 
  char *font_name;
  };

G_DEFINE_TYPE (StationListController, stationlistcontroller, BOILERPLATECONTROLLER_TYPE);

/*=============================================================================
  stationlistcontroller_prompt_for_open_filename_fixed
=============================================================================*/
gboolean stationlistcontroller_prompt_for_open_filename_fixed
     (BoilerplateController *self, const char *fixed_dir, char **filename)
  {
  g_debug ("%s, fixed_dir=%s",__PRETTY_FUNCTION__, fixed_dir);
  gboolean ret = FALSE;
  
  StationListWindow *main_window = 
    STATIONLISTWINDOW (boilerplatecontroller_get_main_window
       (BOILERPLATECONTROLLER (self)));

  FixedDirFilePickerDialog *ffpd = fixeddirfilepickerdialog_new 
    (GTK_WINDOW (main_window), FIXEDDIRPICKER_OPEN);
  int response = fixeddirfilepickerdialog_run (ffpd, fixed_dir, filename);
  if (response == GTK_RESPONSE_OK)
    ret = TRUE;
  return ret;
  }

/*=============================================================================
  stationlistcontroller_prompt_for_save_filename_fixed
=============================================================================*/
gboolean stationlistcontroller_prompt_for_save_filename_fixed
     (BoilerplateController *self, const char *fixed_dir, char **filename)
  {
  g_debug ("%s, fixed_dir=%s",__PRETTY_FUNCTION__, fixed_dir);
  gboolean ret = FALSE;
  
  StationListWindow *main_window = 
    STATIONLISTWINDOW (boilerplatecontroller_get_main_window
       (BOILERPLATECONTROLLER (self)));

  FixedDirFilePickerDialog *ffpd = fixeddirfilepickerdialog_new 
    (GTK_WINDOW (main_window), FIXEDDIRPICKER_SAVE);
  int response = fixeddirfilepickerdialog_run (ffpd, fixed_dir, filename);
  if (response == GTK_RESPONSE_OK)
    ret = TRUE;
  return ret;
  }

/*=============================================================================
  _stationlistcontroller_prompt_for_open_filename
=============================================================================*/
gboolean _stationlistcontroller_prompt_for_open_filename
     (BoilerplateController *self, char **filename)
  {
  gboolean ret = FALSE;
  g_debug ("%s",__PRETTY_FUNCTION__);
  BoilerplateApplication *_application = 
      boilerplatecontroller_get_application (BOILERPLATECONTROLLER (self));

  RadioApplication *application = RADIOAPPLICATION (_application);
  const char *fixed_directory = 
     radioapplication_get_fixed_directory (application);
  if (fixed_directory)
    ret = stationlistcontroller_prompt_for_open_filename_fixed 
     (self, fixed_directory, filename); 
  else
    ret = BOILERPLATECONTROLLER_CLASS (stationlistcontroller_parent_class)
        ->prompt_for_open_filename (self, filename);
  return ret;
  }

/*=============================================================================
  _stationlistcontroller_prompt_for_save_filename
=============================================================================*/
gboolean _stationlistcontroller_prompt_for_save_filename
     (BoilerplateController *self, char **filename)
  {
  gboolean ret = FALSE;
  g_debug ("%s",__PRETTY_FUNCTION__);
  BoilerplateApplication *_application = 
      boilerplatecontroller_get_application (BOILERPLATECONTROLLER (self));

  RadioApplication *application = RADIOAPPLICATION (_application);
  const char *fixed_directory = 
     radioapplication_get_fixed_directory (application);
  if (fixed_directory)
    ret = stationlistcontroller_prompt_for_save_filename_fixed 
     (self, fixed_directory, filename); 
  else
    ret = BOILERPLATECONTROLLER_CLASS (stationlistcontroller_parent_class)
        ->prompt_for_save_filename (self, filename);
  return ret;
  }

/*=============================================================================
  stationlistcontroller_test_connection
=============================================================================*/
static BOOL stationlistcontroller_test_connection
    (StationListController *self, const char *host, int port, char **error)
  {
  gboolean ret = FALSE;
  g_debug ("%s host=%s, port=%d", __PRETTY_FUNCTION__, host, port);

  int major, minor, error_code;
  ret = xineserver_version (host, port, &major, &minor, &error_code, error);
  
  if (ret)
    {
    char *msg;
    asprintf (&msg, "Server version is %d.%d", major, minor);
    boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), msg);
    free (msg);
    }

  return ret;
  }

/*=============================================================================
  stationlistcontroller_stop_update_timer
=============================================================================*/
void stationlistcontroller_stop_update_timer (StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->update_timer > 0)
    g_source_remove (priv->update_timer);
  priv->update_timer = 0;
  }

/*=============================================================================
  stationlistcontroller_update_from_server
=============================================================================*/
gboolean stationlistcontroller_update_from_server 
    (StationListController *self, char **error)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  gboolean ret = TRUE;
  XSStatus *status = NULL;
  int error_code;
  ret = xineserver_status (priv->host, priv->port, &status, 
       &error_code, error);
  if (ret)
    {
    priv->transport_status = xsstatus_get_transport_status (status);
    XSMetaInfo *mi = NULL;
    if (xineserver_meta_info (priv->host, priv->port, &mi, 
         &error_code, error))
      {
      StationListWindow *main_window = 
        STATIONLISTWINDOW (boilerplatecontroller_get_main_window
          (BOILERPLATECONTROLLER (self)));
      stationlistwindow_update_stream_view (main_window, status, mi); 

      xsmetainfo_destroy (mi); 
      }
    
    xsstatus_destroy (status);
    }
  return ret;
  }

/*=============================================================================
  stationlistcontroller_update_tick
=============================================================================*/
gboolean stationlistcontroller_update_tick (void *data)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListController *self = STATIONLISTCONTROLLER (data);
  gboolean ret = FALSE;
  char *error = NULL;
  if (stationlistcontroller_update_from_server (self, &error))
    ret = TRUE;
  else
    {
    char *msg;
    asprintf (&msg, "Server error: %s", error);
    free (msg);
    boilerplatecontroller_statusbar_message 
       (BOILERPLATECONTROLLER (self), msg);
    ret = FALSE;
    free (error);
    // TODO -- If the server operation fails, perhaps we should just
    //  keep on trying? After all, there's little to lose, and the
    //  server might come back
    stationlistcontroller_stop_update_timer (self);
    StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
    stationlistwindow_show_disconnected (main_window);
    StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
    priv->transport_status = (XSTransportStatus)-1; 
    }
  return ret;
  }

/*=============================================================================
  stationlistcontroller_start_update_timer
=============================================================================*/
void stationlistcontroller_start_update_timer (StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->update_timer)
    stationlistcontroller_stop_update_timer (self);
  priv->update_timer = g_timeout_add_seconds (1, 
         stationlistcontroller_update_tick, self);
  }

/*=============================================================================
  stationlistcontroller_is_connected
=============================================================================*/
gboolean stationlistcontroller_is_connected (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  gboolean ret = FALSE;
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->host && priv->port > 0)
    {
    ret = TRUE; 
    }

  return ret;
  }

/*=============================================================================
  stationlistcontroller_can_pause
=============================================================================*/
gboolean stationlistcontroller_can_pause (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  gboolean ret = FALSE;
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->host && priv->port > 0)
    {
    if (priv->transport_status == XINESERVER_TRANSPORT_PLAYING)
      ret = TRUE; 
    }

  return ret;
  }

/*=============================================================================
  stationlistcontroller_can_play
=============================================================================*/
gboolean stationlistcontroller_can_play (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  gboolean ret = FALSE;
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->host && priv->port > 0)
    {
    if (priv->transport_status == XINESERVER_TRANSPORT_PAUSED)
      ret = TRUE; 
    }

  return ret;
  }

/*=============================================================================
  stationlistcontroller_can_stop
=============================================================================*/
gboolean stationlistcontroller_can_stop (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  gboolean ret = FALSE;
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->host && priv->port > 0)
    {
    if (priv->transport_status == XINESERVER_TRANSPORT_PLAYING 
        || priv->transport_status == XINESERVER_TRANSPORT_PAUSED 
        || priv->transport_status == XINESERVER_TRANSPORT_BUFFERING)
      ret = TRUE; 
    }

  return ret;
  }

/*=============================================================================
  _stationlistcontroller_action_save_selected
=============================================================================*/
 static void _stationlistcontroller_action_save_selected
    (const StationListController *self)
  {
  //StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  g_debug ("%s",__PRETTY_FUNCTION__);

  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));

  BoilerplateModel *model = 
     boilerplatecontroller_get_model (BOILERPLATECONTROLLER (self));

  int count = stationlistmodel_count_selected (model);
  if (count > 0)
    {
    char *filename = NULL;
    if (boilerplatecontroller_prompt_for_save_filename 
          (BOILERPLATECONTROLLER (self), &filename))
      {
      char *error = NULL;
      if (stationlistmodel_do_save_selected
                 (BOILERPLATEMODEL (model), filename, &error))
        {
        boilerplatecontroller_statusbar_message 
          (BOILERPLATECONTROLLER (self), "Selected stations saved");
        }
      else
        {
        boilerplatedialogs_error (GTK_WINDOW (main_window), NAME, 
          "No stations are selected");
        free (error);
        }
      }
    else
      {
      g_debug ("%s: user did not select a filename", __PRETTY_FUNCTION__);
      }
    }
  else
    boilerplatedialogs_error (GTK_WINDOW (main_window), NAME, 
       "No stations are selected");
  
  }

/*=============================================================================
  _stationlistcontroller_action_preferences
=============================================================================*/
 static void _stationlistcontroller_action_preferences
    (const StationListController *self)
  {
  //StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  g_debug ("%s",__PRETTY_FUNCTION__);

  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));

  BoilerplateApplication *_application = 
      boilerplatecontroller_get_application (BOILERPLATECONTROLLER (self));
   RadioApplication *application = RADIOAPPLICATION (_application);

  PrefsDialog *dialog = prefsdialog_new (GTK_WINDOW(main_window));

  char *font_name = NULL;
  const char *init_font_name = radioapplication_get_font_name (application);
  gboolean single;
  gboolean init_single = radioapplication_get_single_click_mode (application); 
  if (prefsdialog_run (dialog, init_single, &single, init_font_name, 
        &font_name) == GTK_RESPONSE_OK)
    {
    if (font_name)
      {
      radioapplication_set_font_name (application, font_name);
      free (font_name);
      }
    radioapplication_set_single_click_mode (application, single);
    radioapplication_save_user_rc (application);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_play
=============================================================================*/
 static void _stationlistcontroller_action_play
    (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
  if (priv->host && priv->port > 0)
    {
    int error_code;
    char *error = NULL;
    if (xineserver_play (priv->host, priv->port, 0, &error_code, &error))
      {
      boilerplatecontroller_statusbar_message 
        (BOILERPLATECONTROLLER (self), "Sent play command");
      }
    else
      {
      boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
        error);
      boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", error);
      free (error);
      }
    }
  else
    {
    boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", 
      NOT_CONNECTED_MSG);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_pause
=============================================================================*/
 static void _stationlistcontroller_action_pause
    (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
  if (priv->host && priv->port > 0)
    {
    int error_code;
    char *error = NULL;
    if (xineserver_pause (priv->host, priv->port, &error_code, &error))
      {
      boilerplatecontroller_statusbar_message 
        (BOILERPLATECONTROLLER (self), "Sent pause command");
      }
    else
      {
      boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
        error);
      boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", error);
      free (error);
      }
    }
  else
    {
    boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", 
      NOT_CONNECTED_MSG);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_stop
=============================================================================*/
 static void _stationlistcontroller_action_stop
    (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
  if (priv->host && priv->port > 0)
    {
    stationlistwindow_set_station (main_window, NULL);
    int error_code;
    char *error = NULL;
    if (xineserver_stop (priv->host, priv->port, &error_code, &error))
      {
      boilerplatecontroller_statusbar_message 
        (BOILERPLATECONTROLLER (self), "Sent stop command");
      }
    else
      {
      boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
        error);
      boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", error);
      free (error);
      }
    }
  else
    {
    boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", 
      NOT_CONNECTED_MSG);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_vol_down
=============================================================================*/
 static void _stationlistcontroller_action_vol_down
    (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
  if (priv->host && priv->port > 0)
    {
    int error_code;
    char *error = NULL;
    int volume = 0;
    if (xineserver_get_volume (priv->host, priv->port, &volume, &error_code, 
          &error))
      {
      volume -= 10;
      if (volume < XINESERVER_MIN_VOLUME)  
        volume = XINESERVER_MIN_VOLUME;
      if (xineserver_set_volume (priv->host, priv->port, volume, &error_code, 
          &error))
        {
        char s[30];
        sprintf (s, "Volume set to %d", volume);
        boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
          s);
        }
      else
        {
        boilerplatedialogs_error (GTK_WINDOW(main_window), 
           NAME " error", error);
        free (error);
        }
      }
    else
      {
      boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
        error);
      boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", error);
      free (error);
      }
    }
  else
    {
    boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", 
      NOT_CONNECTED_MSG);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_vol_up
=============================================================================*/
 static void _stationlistcontroller_action_vol_up
    (const StationListController *self)
  {
  g_debug ("%s",__PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
  if (priv->host && priv->port > 0)
    {
    int error_code;
    char *error = NULL;
    int volume = 0;
    if (xineserver_get_volume (priv->host, priv->port, &volume, &error_code, 
          &error))
      {
      volume += 10;
      if (volume > XINESERVER_MAX_VOLUME)  
        volume = XINESERVER_MAX_VOLUME;
      if (xineserver_set_volume (priv->host, priv->port, volume, &error_code, 
          &error))
        {
        char s[30];
        sprintf (s, "Volume set to %d", volume);
        boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
          s);
        }
      else
        {
        boilerplatedialogs_error (GTK_WINDOW(main_window), 
           NAME " error", error);
        free (error);
        }
      }
    else
      {
      boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self), 
        error);
      boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", error);
      free (error);
      }
    }
  else
    {
    boilerplatedialogs_error (GTK_WINDOW(main_window), NAME " error", 
      NOT_CONNECTED_MSG);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_connect
=============================================================================*/
 static void _stationlistcontroller_action_connect
    (StationListController *self, const char *host, int port)
  {
  g_debug ("%s host=%s, port=%d", __PRETTY_FUNCTION__, host, port);

  stationlistcontroller_stop_update_timer (self);
  StationListWindow *main_window = 
      STATIONLISTWINDOW (boilerplatecontroller_get_main_window
         (BOILERPLATECONTROLLER (self)));
  stationlistwindow_show_disconnected (main_window);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  priv->transport_status = (XSTransportStatus)-1; 
  char *error = NULL;
  BOOL ok = stationlistcontroller_test_connection (self, host, port, &error);
  if (ok)
    {
    StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
    BoilerplateApplication *_application = 
       boilerplatecontroller_get_application (BOILERPLATECONTROLLER (self));
    RadioApplication *application = RADIOAPPLICATION (_application);

    radioapplication_set_port (application, port);
    radioapplication_set_host (application, host);

    if (priv->host) free (priv->host);
    priv->host = strdup (host);
    priv->port = port;
    radioapplication_save_user_rc (application);
    stationlistcontroller_start_update_timer (self);
    }
  else
    {
    BoilerplateMainWindow *main_window = 
      boilerplatecontroller_get_main_window (BOILERPLATECONTROLLER(self));
    char *msg;
    asprintf (&msg, "Connection to server %s failed: %s", host, error);
    boilerplatedialogs_error (GTK_WINDOW (main_window), NAME, msg);
    free (error);
    free (msg);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_play_station
=============================================================================*/
void _stationlistcontroller_action_play_station (StationListController *self,
       const char *name, const char *uri)
  {
  g_debug ("%s name=%s uri=%s", __PRETTY_FUNCTION__, name, uri);

  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  BoilerplateMainWindow *main_window = 
      boilerplatecontroller_get_main_window (BOILERPLATECONTROLLER (self));

  char *error = NULL;
  int error_code;
  gboolean ok = TRUE;
  if (ok)
    {
    boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self),
      "Clearing server playlist");
    if (!xineserver_clear (priv->host, priv->port, &error_code, &error))
      {
      boilerplatedialogs_error (GTK_WINDOW (main_window), 
          NAME " error", error);
      free (error);
      ok = FALSE;
      }
    }
  if (ok)
    {
    boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self),
      "Setting stream on server");
    if (!xineserver_add_single (priv->host, priv->port, uri, 
           &error_code, &error))
      {
      boilerplatedialogs_error (GTK_WINDOW (main_window), 
          NAME " error", error);
      free (error);
      ok = FALSE;
      }
    }
  if (ok)
    {
    boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self),
      "Sending play command");
    if (!xineserver_play (priv->host, priv->port, 0,
           &error_code, &error))
      {
      boilerplatedialogs_error (GTK_WINDOW (main_window), 
          NAME " error", error);
      free (error);
      ok = FALSE;
      }
    }
  if (ok)
    {
    stationlistwindow_set_station (STATIONLISTWINDOW (main_window), name);
    boilerplatecontroller_statusbar_message (BOILERPLATECONTROLLER (self),
      name);
    }
  }

/*=============================================================================
  stationlistcontroller_action_play_station
=============================================================================*/
void stationlistcontroller_action_play_station (StationListController *self,
       const char *name, const char *uri)
  {
  g_debug ("%s name=%s uri=%s", __PRETTY_FUNCTION__, name, uri);
  STATIONLISTCONTROLLER_GET_CLASS (self)->action_play_station (self,
     name, uri);
  }

/*=============================================================================
  _stationlistcontroller_action_prompt_connect
=============================================================================*/
 static void _stationlistcontroller_action_prompt_connect
    (StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  BoilerplateMainWindow *main_window = 
    boilerplatecontroller_get_main_window (BOILERPLATECONTROLLER(self));

  BoilerplateApplication *_application = 
     boilerplatecontroller_get_application (BOILERPLATECONTROLLER (self));
  RadioApplication *application = RADIOAPPLICATION (_application);

  const char *init_host = radioapplication_get_host (application);
  if (init_host == NULL || strlen (init_host) == 0)
    init_host = "localhost";

  int init_port = radioapplication_get_port (application);
  if (init_port == 0) 
    init_port  = XINESERVER_DEF_PORT;

  ConnectDialog *cd = connectdialog_new (GTK_WINDOW(main_window));
  char *host = NULL;
  int port;
  int response = connectdialog_run (cd, init_host, init_port, &host, &port);
  if (response == GTK_RESPONSE_OK)
    {
    STATIONLISTCONTROLLER_GET_CLASS (self)
      ->action_connect (self, host, port);
    }
  if (host) free (host);
  }

/*=============================================================================
  _stationlistcontroller_action_select_all
=============================================================================*/
 static void _stationlistcontroller_action_select_all 
     (const StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModel *model = 
     STATIONLISTMODEL (boilerplatecontroller_get_model 
       (BOILERPLATECONTROLLER (self)));
  
  stationlistmodel_select_all (model);
  }

/*=============================================================================
  _stationlistcontroller_action_select_none
=============================================================================*/
 static void _stationlistcontroller_action_select_none
     (const StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModel *model = 
     STATIONLISTMODEL (boilerplatecontroller_get_model 
       (BOILERPLATECONTROLLER (self)));
  
  stationlistmodel_select_none (model);
  }

/*=============================================================================
  _stationlistcontroller_action_select_invert
=============================================================================*/
 static void _stationlistcontroller_action_select_invert
     (const StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModel *model = 
     STATIONLISTMODEL (boilerplatecontroller_get_model 
       (BOILERPLATECONTROLLER (self)));
  
  stationlistmodel_select_invert (model);
  }

/*=============================================================================
  _stationlistcontroller_action_delete_entry
=============================================================================*/
 static void _stationlistcontroller_action_delete_entry 
     (const StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModel *model = 
     STATIONLISTMODEL (boilerplatecontroller_get_model 
       (BOILERPLATECONTROLLER (self)));
  
  StationListWindow *main_window = 
     STATIONLISTWINDOW (boilerplatecontroller_get_main_window
       (BOILERPLATECONTROLLER (self)));

  GtkTreeView *tree_view = stationlistwindow_get_tree_view (main_window); 

  GtkTreeIter iter;
  GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
  GtkTreeModel *m;

  if (gtk_tree_selection_get_selected (selection, &m, &iter))
    {
    gtk_list_store_remove (GTK_LIST_STORE(m), &iter);
    stationlistmodel_set_need_save (model);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_add_entry
=============================================================================*/
 static void _stationlistcontroller_action_add_entry 
     (const StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModel *model = 
     STATIONLISTMODEL (boilerplatecontroller_get_model 
       (BOILERPLATECONTROLLER (self)));
  
  StationListWindow *main_window = 
     STATIONLISTWINDOW (boilerplatecontroller_get_main_window
       (BOILERPLATECONTROLLER (self)));

  char *name = NULL;
  char *uri = NULL;
  char *country = NULL;

  EditStationDialog *d = editstationdialog_new(GTK_WINDOW (main_window));
  if (editstationdialog_run
         (d, NULL, NULL, NULL,
          &name, &uri, &country) == GTK_RESPONSE_OK)
    {
    stationlistmodel_add_station (model, name, uri, country, FALSE);
    if (uri) free (uri);
    if (name) free (name);
    if (country) free (country);
    }
  }

/*=============================================================================
  _stationlistcontroller_action_edit_entry
=============================================================================*/
 static void _stationlistcontroller_action_edit_entry 
     (const StationListController *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  StationListModel *model = 
     STATIONLISTMODEL (boilerplatecontroller_get_model 
       (BOILERPLATECONTROLLER (self)));
  
  StationListWindow *main_window = 
     STATIONLISTWINDOW (boilerplatecontroller_get_main_window
       (BOILERPLATECONTROLLER (self)));

  GtkTreeView *tree_view = stationlistwindow_get_tree_view (main_window); 

  GtkTreeIter iter;
  GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
  GtkTreeModel *m;

  if (gtk_tree_selection_get_selected (selection, &m, &iter))
    {
    char *init_uri = NULL;
    char *init_name = NULL;
    char *init_country = NULL;
    gboolean selected = FALSE;

    gtk_tree_model_get (m, &iter, URI_COLUMN, &init_uri, -1);
    gtk_tree_model_get (m, &iter, NAME_COLUMN, &init_name, -1);
    gtk_tree_model_get (m, &iter, COUNTRY_COLUMN, &init_country, -1);
    gtk_tree_model_get (m, &iter, SELECTED_COLUMN, &selected, -1);

    char *name = NULL;
    char *uri = NULL;
    char *country = NULL;
    EditStationDialog *d = editstationdialog_new(GTK_WINDOW (main_window));
    if (editstationdialog_run
           (d, init_name, init_uri, init_country,
            &name, &uri, &country) == GTK_RESPONSE_OK)
      {
      gtk_list_store_set (GTK_LIST_STORE (m), &iter,
             NAME_COLUMN, name,
             COUNTRY_COLUMN, country,
             URI_COLUMN, uri,
             SELECTED_COLUMN, selected,
             -1);
      // TODO update model
      stationlistmodel_set_need_save (model);
      if (uri) free (uri);
      if (name) free (name);
      if (country) free (country);
      }

    if (init_uri) free (init_uri);
    if (init_name) free (init_name);
    if (init_country) free (init_country);
    }
  }

/*=============================================================================
  stationlistcontroller_init
=============================================================================*/
 static void stationlistcontroller_init (StationListController *self)
  {
  g_debug ("%s: intialized controller", __PRETTY_FUNCTION__);
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  priv->update_timer = 0;
  priv->transport_status = (XSTransportStatus)-1; 
  }

/*=============================================================================
  stationlistcontroller_dispose
=============================================================================*/
static void stationlistcontroller_dispose (GObject *self)
  {
  StationListControllerPriv *priv = STATIONLISTCONTROLLER_PRIV (self);
  if (priv->host) free (priv->host);
  if (priv->font_name) free (priv->font_name);
  G_OBJECT_CLASS (stationlistcontroller_parent_class)->dispose (self);
  }

/*============================================================================
  stationlistcontroller_class_init
=============================================================================*/
static void stationlistcontroller_class_init
  (StationListControllerClass *klass)
  {
  g_debug ("%s: initializing class", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (StationListControllerPriv));
  G_OBJECT_CLASS (klass)->dispose = stationlistcontroller_dispose;
  klass->action_delete_entry = _stationlistcontroller_action_delete_entry;
  klass->action_edit_entry = _stationlistcontroller_action_edit_entry;
  klass->action_add_entry = _stationlistcontroller_action_add_entry;
  klass->action_prompt_connect = _stationlistcontroller_action_prompt_connect;
  klass->action_play_station = _stationlistcontroller_action_play_station;
  klass->action_connect = _stationlistcontroller_action_connect;
  klass->action_play = _stationlistcontroller_action_play;
  klass->action_pause = _stationlistcontroller_action_pause;
  klass->action_stop = _stationlistcontroller_action_stop;
  klass->action_vol_up = _stationlistcontroller_action_vol_up;
  klass->action_vol_down = _stationlistcontroller_action_vol_down;
  klass->action_preferences = _stationlistcontroller_action_preferences;
  klass->action_save_selected = _stationlistcontroller_action_save_selected;
  klass->action_select_all = _stationlistcontroller_action_select_all;
  klass->action_select_none = _stationlistcontroller_action_select_none;
  klass->action_select_invert = _stationlistcontroller_action_select_invert;
  BoilerplateControllerClass *parent = 
       BOILERPLATECONTROLLER_CLASS (klass);
  parent->prompt_for_open_filename 
                      = _stationlistcontroller_prompt_for_open_filename;
  parent->prompt_for_save_filename 
                      = _stationlistcontroller_prompt_for_save_filename;
  g_debug ("%s: initialized class", __PRETTY_FUNCTION__);
  }


/*============================================================================
  stationlistcontroller_new
=============================================================================*/
StationListController *stationlistcontroller_new (BoilerplateMainWindow *w,
     BoilerplateApplication *application, BoilerplateModel *model)
  {
  g_debug ("%s: calling g_object_new", __PRETTY_FUNCTION__);
  StationListController *self = g_object_new 
     (STATIONLISTCONTROLLER_TYPE, "main-window", w, "application", 
      application, "model", model, NULL);
  g_debug ("%s: called g_object_new", __PRETTY_FUNCTION__);
  return self;
  }

