/*=============================================================================
  
  gxsradio
  StreamInfoView.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "xine-server-api.h" 

G_BEGIN_DECLS

#define STREAMINFOVieW_TYPE            (streaminfoview_get_type ())
#define STREAMINFOVieW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), STREAMINFOVieW_TYPE, StreamInfoView))
#define STREAMINFOVieW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), STREAMINFOVieW_TYPE, StreamInfoViewClass))
#define IS_STREAMINFOVieW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STREAMINFOVieW_TYPE))
#define IS_STREAMINFOVieW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), STREAMINFOVieW_TYPE))
#define STREAMINFOVieW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), STREAMINFOVieW_TYPE, StreamInfoViewClass))

typedef struct _StreamInfoView StreamInfoView;
typedef struct _StreamInfoViewClass StreamInfoViewClass;

struct _StreamInfoView
  {
  GtkBin base;
  };

struct _StreamInfoViewClass
  {
  GtkBinClass parent_class;
  gpointer padding[12];
  };

GType streaminfoview_get_type (void) G_GNUC_CONST;

StreamInfoView 
  *streaminfoview_new ();

void 
  streaminfoview_show_disconnected (StreamInfoView *self);

void streaminfoview_show_info (StreamInfoView *self,
       const XSStatus *status, const XSMetaInfo *mi);

void streaminfoview_set_station (StreamInfoView *self, const char *station);

G_END_DECLS




