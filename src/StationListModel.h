/*============================================================================
  
  gxsradio
  StationListModel.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateModel.h" 

G_BEGIN_DECLS


#define STATIONLISTMODEL_TYPE            (stationlistmodel_get_type ())
#define STATIONLISTMODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), STATIONLISTMODEL_TYPE, StationListModel))
#define STATIONLISTMODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), STATIONLISTMODEL_TYPE, StationListModelClass))
#define IS_STATIONLISTMODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STATIONLISTMODEL_TYPE))
#define IS_STATIONLISTMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), STATIONLISTMODEL_TYPE))
#define STATIONLISTMODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), STATIONLISTMODEL_TYPE, StationListModelClass))

typedef struct _StationListModel StationListModel;
typedef struct _StationListModelClass  StationListModelClass;

enum
   {
   NAME_COLUMN = 0,
   COUNTRY_COLUMN,
   URI_COLUMN,
   SELECTED_COLUMN,
   N_COLUMNS
   };

#define MODEL_COLUMN_0_TITLE "Name"
#define MODEL_COLUMN_1_TITLE "Country"
#define MODEL_COLUMN_2_TITLE "URI"
#define MODEL_COLUMN_3_TITLE "Selected"

struct _StationListModel
  {
  BoilerplateModel base;
  };

struct _StationListModelClass
  {
  BoilerplateModelClass parent_class;

  gpointer padding[12];
  };

GType stationlistmodel_get_type (void) G_GNUC_CONST;

StationListModel  *stationlistmodel_new (void);

GtkListStore *stationlistmodel_get_list_model (const StationListModel *self);

void          stationlistmodel_set_need_save (StationListModel *self);

gboolean      stationlistmodel_do_save_selected
                 (const BoilerplateModel *self, const char *file, char **error);

int           stationlistmodel_count_selected
                 (const BoilerplateModel *self);

void          stationlistmodel_select_none
                 (StationListModel *self);

void          stationlistmodel_select_all
                 (StationListModel *self);

void          stationlistmodel_select_invert
                 (StationListModel *self);

void          stationlistmodel_add_station
                 (StationListModel *self, const char *name, const char *uri, 
		 const char *country, gboolean selected);

G_END_DECLS





