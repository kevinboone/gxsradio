/*=============================================================================
  
  gxsradio 
  PrefsDialog.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
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
#include "PrefsDialog.h"
#include "StationListController.h"
#include "StationListModel.h"
#include "boilerplatedialogs.h"

#define PREFSDIALOG_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
          PREFSDIALOG_TYPE, PrefsDialogPriv))

typedef struct _PrefsDialogPriv PrefsDialogPriv;

struct _PrefsDialogPriv
  {
  GtkFontButton *font_button;
  GtkCheckButton *single_click_button;
  };

G_DEFINE_TYPE (PrefsDialog, prefsdialog, GTK_TYPE_DIALOG);

/*======================================================================
  prefsdialog_response_event
======================================================================*/
static void prefsdialog_response_event (GtkDialog *d,
  int id, gpointer user_data)
  {
  }

/*======================================================================
  prefsdialog_pango_to_css
  I can scarcely believe that I have to do this transformation myself :/
  Whatever were the GTK developers thinking?
======================================================================*/
char *prefsdialog_pango_to_css (const PangoFontDescription *font_desc)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  const char *family
        = pango_font_description_get_family (font_desc);

  int size = pango_font_description_get_size (font_desc) / PANGO_SCALE;

  PangoStyle style = pango_font_description_get_style (font_desc); 
  const char *ss = "normal";
  switch (style)
    {
    case PANGO_STYLE_NORMAL:
      ss = ""; break;
    case PANGO_STYLE_OBLIQUE:
      ss = "oblique"; break;
    case PANGO_STYLE_ITALIC:
      ss = "italic"; break;
    }

  PangoWeight weight = pango_font_description_get_weight (font_desc); 
  const char *sw = "normal";
  switch (weight)
    {
    case PANGO_WEIGHT_LIGHT: 
    case PANGO_WEIGHT_THIN: 
    case PANGO_WEIGHT_ULTRALIGHT: 
    case PANGO_WEIGHT_SEMILIGHT: 
       sw = "lighter"; break;
    case PANGO_WEIGHT_NORMAL: 
    case PANGO_WEIGHT_MEDIUM: 
    case PANGO_WEIGHT_BOOK: 
       sw = "normal"; break;
    case PANGO_WEIGHT_BOLD: 
    case PANGO_WEIGHT_SEMIBOLD: 
       sw = "bold"; break;
    case PANGO_WEIGHT_ULTRABOLD: 
    case PANGO_WEIGHT_HEAVY: 
    case PANGO_WEIGHT_ULTRAHEAVY: 
       sw = "bolder"; break;
    }
    
  char *css = NULL;
  asprintf (&css, "%s %s %dpt \"%s\"", ss, sw, size, family);
  g_debug ("Font css is \"%s\"", css);
  return css;
  }

/*=============================================================================
  prefsdialog_run
=============================================================================*/
int prefsdialog_run (const PrefsDialog *self, gboolean init_single, 
     gboolean *single, const char *init_font, char **font)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);

  PrefsDialogPriv *priv = PREFSDIALOG_PRIV (self);

  gtk_widget_show_all (GTK_WIDGET(self));
  int response;
  gboolean valid;

  PangoFontDescription *font_desc = NULL;
  GtkStyleContext *style_context = gtk_widget_get_style_context  
      (GTK_WIDGET (self));
  gtk_style_context_set_state (style_context, 0);
  gtk_style_context_get (style_context, 
        gtk_style_context_get_state (style_context), "font", &font_desc, NULL);
  if (font_desc)
    {
    char *font_name = pango_font_description_to_string (font_desc);
    gtk_font_chooser_set_font (GTK_FONT_CHOOSER (priv->font_button), 
      font_name);
    free (font_name);
    pango_font_description_free (font_desc);
    }

  gtk_style_context_save (style_context);    

  gtk_toggle_button_set_active 
    (GTK_TOGGLE_BUTTON (priv->single_click_button), init_single);

  do
    {
    valid = TRUE;
    response = gtk_dialog_run (GTK_DIALOG (self));
    if (response == GTK_RESPONSE_OK)
      {
      valid = TRUE;

      PangoFontDescription *font_desc 
        = gtk_font_chooser_get_font_desc (GTK_FONT_CHOOSER(priv->font_button));

      char *css_font = prefsdialog_pango_to_css (font_desc);
          
      pango_font_description_free (font_desc);

      if (valid)
        {
        *font = strdup (css_font);
        *single = gtk_toggle_button_get_active 
           (GTK_TOGGLE_BUTTON (priv->single_click_button));
        }

      free (css_font);
      }
    } while (response == GTK_RESPONSE_OK && !valid);

  gtk_widget_destroy (GTK_WIDGET (self));

  return response;
  }

/*=============================================================================
  prefsdialog_init
=============================================================================*/
static void prefsdialog_init (PrefsDialog *self)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  PrefsDialogPriv *priv = PREFSDIALOG_PRIV (self);

  gtk_window_set_title (GTK_WINDOW (self), NAME ": preferences");

  gtk_dialog_add_buttons (GTK_DIALOG (self),
       "_Cancel", GTK_RESPONSE_CANCEL, "C_onfirm", GTK_RESPONSE_OK, NULL);

  GtkBox *content = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (self)));
  GtkGrid *content_box = GTK_GRID (gtk_grid_new ());
  gtk_grid_set_row_spacing (content_box, 10);
  gtk_widget_set_margin_start (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_end (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_top (GTK_WIDGET(content_box), 0);
  gtk_widget_set_margin_bottom (GTK_WIDGET(content_box), 0);

  priv->font_button = GTK_FONT_BUTTON (gtk_font_button_new ());
  priv->single_click_button = GTK_CHECK_BUTTON (gtk_check_button_new ());

  GtkLabel *l;
  l = GTK_LABEL (gtk_label_new_with_mnemonic ("_Font: "));
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET (priv->font_button));
  gtk_grid_attach (content_box, GTK_WIDGET(l), 
        1, 1, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(priv->font_button), 
        2, 1, 1, 1);

  l = GTK_LABEL (gtk_label_new_with_mnemonic ("_Single click: "));
  gtk_label_set_mnemonic_widget (GTK_LABEL (l), GTK_WIDGET 
       (priv->single_click_button));
  gtk_grid_attach (content_box, GTK_WIDGET(l), 
        1, 2, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(priv->single_click_button), 
        2, 2, 1, 1);

  gtk_box_pack_start (GTK_BOX (content),
         GTK_WIDGET(content_box), FALSE, FALSE, 5);
  gtk_box_pack_start (GTK_BOX (content),
         gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  GtkWidget *action_area = gtk_dialog_get_action_area (GTK_DIALOG(self));
  gtk_widget_set_margin_start (action_area, 5);
  gtk_widget_set_margin_end (action_area, 5);
  gtk_widget_set_margin_top (action_area, 5);
  gtk_widget_set_margin_bottom (action_area, 5);
#pragma GCC diagnostic pop

  g_signal_connect (G_OBJECT(self), "response",
     G_CALLBACK (prefsdialog_response_event), self);
  gtk_dialog_set_default_response (GTK_DIALOG (self), GTK_RESPONSE_OK);

  }

/*=============================================================================
  prefsdialog_class_init
=============================================================================*/

static void prefsdialog_class_init (PrefsDialogClass *klass)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  g_type_class_add_private (klass, sizeof (PrefsDialogPriv));
  //PrefsDialogClass *parent = GTK_DIALOG_CLASS (klass);
  }

/*=============================================================================
  prefsdialog_new
=============================================================================*/
PrefsDialog *prefsdialog_new (GtkWindow *parent)
  {
  g_debug ("%s", __PRETTY_FUNCTION__);
  PrefsDialog *self = g_object_new 
     (prefsdialog_get_type(), NULL);
  gtk_window_set_transient_for (GTK_WINDOW(self), GTK_WINDOW (parent));
  return self;
  }


