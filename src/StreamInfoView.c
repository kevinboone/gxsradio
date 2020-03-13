/*=============================================================================
  
  gsxradio
  StreamInfoView.c

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
#include "StreamInfoView.h"

#define STREAMINFOVieW_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          STREAMINFOVieW_TYPE, StreamInfoViewPriv))

typedef struct _StreamInfoViewPriv StreamInfoViewPriv;

struct _StreamInfoViewPriv
  {
  char *station;
  GtkLabel *station_label;
  GtkLabel *title_label;
  GtkLabel *bitrate_label;
  GtkLabel *status_label;
  };

G_DEFINE_TYPE (StreamInfoView, streaminfoview, GTK_TYPE_BIN);


/*=============================================================================
  streaminfoview_init
=============================================================================*/
static void streaminfoview_init (StreamInfoView *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  StreamInfoViewPriv *priv = STREAMINFOVieW_PRIV (self);
  priv->station = NULL;
  }

/*=============================================================================
  streaminfoview_set_station
=============================================================================*/
void streaminfoview_set_station (StreamInfoView *self, const char *station)
  {
  StreamInfoViewPriv *priv = STREAMINFOVieW_PRIV (self);
  if (priv->station) free (priv->station);
  if (station)
    priv->station = strdup (station);
  else
    priv->station = NULL;
  }

/*=============================================================================
  streaminfoview_show_disconnected
=============================================================================*/
void streaminfoview_show_disconnected (StreamInfoView *self)
  {
  StreamInfoViewPriv *priv = STREAMINFOVieW_PRIV (self);
  if (priv->station) free (priv->station);
  priv->station = NULL;
  gtk_label_set_text (priv->station_label, "[no station]");
  gtk_label_set_text (priv->title_label, "[no title]");
  gtk_label_set_text (priv->bitrate_label, "0 kbps");
  gtk_label_set_text (priv->status_label, "Not connected to server");
  }

/*=============================================================================
  streaminfoview_constructed
=============================================================================*/
void streaminfoview_show_info (StreamInfoView *self,
       const XSStatus *status, const XSMetaInfo *mi)
  {
  StreamInfoViewPriv *priv = STREAMINFOVieW_PRIV (self);
  // TODO save status and check, to reduce redraw overhead
  XSTransportStatus ts = xsstatus_get_transport_status (status);
  const char *new_status;
  switch (ts)
    {
    case XINESERVER_TRANSPORT_STOPPED:
      new_status = "stopped";
      break;
    case XINESERVER_TRANSPORT_PLAYING:
      new_status = "playing";
      break;
    case XINESERVER_TRANSPORT_PAUSED:
      new_status = "paused";
      break;
    case XINESERVER_TRANSPORT_BUFFERING:
      new_status = "buffering...";
      break;
    }

  int new_kbps = xsmetainfo_get_bitrate (mi);
  char s[20];
  snprintf (s, sizeof (s) - 1, "%d kbps", new_kbps/1000);
  const char *new_title = xsmetainfo_get_title (mi);
  if (new_title[0] == '-') new_title = "[no title]";

  if (priv->station)
    gtk_label_set_text (priv->station_label, priv->station);
  else
    gtk_label_set_text (priv->station_label, "[no station]");
  gtk_label_set_text (priv->title_label, new_title);
  gtk_label_set_text (priv->bitrate_label, s);
  gtk_label_set_text (priv->status_label, new_status);
  }

/*=============================================================================
  streaminfoview_constructed
=============================================================================*/
static void _streaminfoview_constructed (GObject *obj)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  G_OBJECT_CLASS (streaminfoview_parent_class)->constructed (obj);
  StreamInfoView *self = STREAMINFOVieW (obj);
  StreamInfoViewPriv *priv = STREAMINFOVieW_PRIV (self);

  GtkGrid *grid = GTK_GRID (gtk_grid_new ());
  GtkWidget *station_label = gtk_label_new ("station"); // TODO
  gtk_grid_attach (grid, GTK_WIDGET (station_label), 1, 1, 2, 1);
  GtkWidget *title_label = gtk_label_new ("title"); // TODO
  gtk_grid_attach (grid, GTK_WIDGET (title_label), 1, 2, 2, 1);
  GtkWidget *bitrate_label = gtk_label_new ("kbps"); // TODO
  gtk_grid_attach (grid, GTK_WIDGET (bitrate_label), 1, 3, 1, 1);
  GtkWidget *status_label = gtk_label_new ("status"); // TODO
  gtk_grid_attach (grid, GTK_WIDGET (status_label), 2, 3, 1, 1);
  gtk_widget_set_margin_start (GTK_WIDGET(grid), 5);
  gtk_widget_set_margin_end (GTK_WIDGET(grid), 5);
  gtk_widget_set_margin_top (GTK_WIDGET(grid), 5);
  gtk_widget_set_margin_bottom (GTK_WIDGET(grid), 5);
  gtk_grid_set_row_spacing (grid, 5);
  gtk_grid_set_column_spacing (grid, 5);
  gtk_container_add (GTK_CONTAINER(self), GTK_WIDGET (grid));
  priv->station_label = GTK_LABEL (station_label);
  priv->title_label = GTK_LABEL (title_label);
  priv->bitrate_label = GTK_LABEL (bitrate_label);
  priv->status_label = GTK_LABEL (status_label);
  gtk_widget_set_hexpand (GTK_WIDGET(station_label), TRUE);
  gtk_widget_set_hexpand (GTK_WIDGET(status_label), TRUE);
  gtk_widget_set_halign (GTK_WIDGET(station_label), GTK_ALIGN_START);
  gtk_widget_set_halign (GTK_WIDGET(title_label), GTK_ALIGN_START);
  gtk_widget_set_halign (GTK_WIDGET(bitrate_label), GTK_ALIGN_START);
  gtk_widget_set_halign (GTK_WIDGET(status_label), GTK_ALIGN_START);
  gtk_widget_set_hexpand (GTK_WIDGET(grid), TRUE);
  gtk_widget_set_name (GTK_WIDGET(grid), "streaminfogrid");
  gtk_widget_set_name (GTK_WIDGET(self), "streaminfo");
  }

/*=============================================================================
  streaminfoview_dispose
=============================================================================*/
static void _streaminfoview_dispose (GObject *self)
  {
  G_OBJECT_CLASS (streaminfoview_parent_class)->dispose (self);
  StreamInfoViewPriv *priv = STREAMINFOVieW_PRIV (self);
  if (priv->station) free (priv->station); 
  priv->station = NULL;
  }

/*=============================================================================
  streaminfoview_class_init
=============================================================================*/
static void streaminfoview_class_init
  (StreamInfoViewClass *klass)
  {
  g_debug ("%s: in", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (StreamInfoViewPriv));
  G_OBJECT_CLASS (klass)->constructed = _streaminfoview_constructed; 
  G_OBJECT_CLASS (klass)->dispose = _streaminfoview_dispose;
  //GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  //gtk_widget_class_set_css_name (widget_class, "streaminfowidget");

  g_debug ("%s: out", __PRETTY_FUNCTION__);
  }

/*=============================================================================
  streaminfoview_new
=============================================================================*/
StreamInfoView *streaminfoview_new (void)
  {
  StreamInfoView *self = g_object_new (STREAMINFOVieW_TYPE, NULL);
  return self;
  }

