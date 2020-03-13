/*=============================================================================
  
  gxsradio
  StationListMenuBar.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateController.h" 
#include "BoilerplateMenuBar.h" 
#include "BoilerplateModel.h" 

#define SMB_MENU_CONNECT "Connec_t..."
#define SMB_MENU_SAVE_SELECTED "Sa_ve selected..."

#define SMB_MENU_AUDIO "_Audio"
#define SMB_MENU_VOL_DOWN "Volume _down"
#define SMB_MENU_PLAY "_Play"
#define SMB_MENU_PAUSE "P_ause"
#define SMB_MENU_STOP "_Stop"
#define SMB_MENU_VOL_UP "Volume _up"
#define SMB_MENU_VOL_DOWN "Volume _down"

#define SMB_MENU_EDIT "_Edit"
#define SMB_MENU_ADD "_Add station"
#define SMB_MENU_DELETE "_Delete station"
#define SMB_MENU_EDIT_STATION "_Edit station"
#define SMB_MENU_SELECT_ALL "Select a_ll"
#define SMB_MENU_SELECT_NONE "Select _none"
#define SMB_MENU_SELECT_INVERT "_Invert selection"
#define SMB_MENU_PREFS "_Preferences"

G_BEGIN_DECLS

#define STATIONLISTMENUBAR_TYPE            (stationlistmenubar_get_type ())
#define STATIONLISTMENUBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), STATIONLISTMENUBAR_TYPE, StationListMenuBar))
#define STATIONLISTMENUBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), STATIONLISTMENUBAR_TYPE, StationListMenuBarClass))
#define IS_STATIONLISTMENUBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STATIONLISTMENUBAR_TYPE))
#define IS_STATIONLISTMENUBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), STATIONLISTMENUBAR_TYPE))
#define STATIONLISTMENUBAR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), STATIONLISTMENUBAR_TYPE, StationListMenuBarClass))

typedef struct _StationListMenuBar StationListMenuBar;
typedef struct _StationListMenuBarClass StationListMenuBarClass;
struct _BoilerplateMainWindow;

struct _StationListMenuBar
  {
  BoilerplateMenuBar base;
  };

struct _StationListMenuBarClass
  {
  BoilerplateMenuBarClass parent_class;
  gpointer padding[12];
  };

GType stationlistmenubar_get_type (void) G_GNUC_CONST;

StationListMenuBar 
  *stationlistmenubar_new (GtkAccelGroup *accel, BoilerplateController *controller,
   BoilerplateModel *model, struct _BoilerplateMainWindow *w);

G_END_DECLS




