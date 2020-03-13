/*======================================================================

  boilerplate-gtk-c
  BoilerplateDialogs.c

======================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "boilerplatedialogs.h"

typedef enum _IconCode
  {
  BPD_ICON_ERROR = 0,
  BPD_ICON_QUESTION = 1,
  BPD_ICON_INFO = 2
  } IconCode;

extern const char *xpm_dialog_error;
extern const char *xpm_dialog_question;
extern const char *xpm_dialog_info;

/*======================================================================

  boilerplatediaologs_set_content_area 

======================================================================*/
static void boilerplatedialogs_set_content_area (GtkDialog *dialog, 
     const char *message, IconCode icon_code)
  {
  GtkBox *content = GTK_BOX (gtk_dialog_get_content_area (dialog));

  GtkGrid *content_box = GTK_GRID (gtk_grid_new ());

  GtkLabel *l = GTK_LABEL (gtk_label_new (NULL));
  //gtk_label_set_line_wrap (l, TRUE);
  gtk_label_set_use_markup (l, TRUE);
  gtk_label_set_markup (l, message);

  const char **pb_data = NULL; 
  switch (icon_code)
    {
    case BPD_ICON_ERROR:
      pb_data = &xpm_dialog_error;
      break;
    case BPD_ICON_QUESTION:
      pb_data = &xpm_dialog_question;
      break;
    case BPD_ICON_INFO:
      pb_data = &xpm_dialog_info;
      break;
    }

  GdkPixbuf *pb = gdk_pixbuf_new_from_xpm_data (pb_data);
  GtkImage *icon = GTK_IMAGE (gtk_image_new_from_pixbuf (pb));
  gtk_widget_set_margin_start (GTK_WIDGET (icon), 10);
  gtk_widget_set_margin_end (GTK_WIDGET (icon), 10);

  gtk_grid_attach (content_box, GTK_WIDGET (icon),
        1, 1, 1, 1);
  gtk_grid_attach (content_box, GTK_WIDGET(l),
        2, 1, 1, 1);

  gtk_box_pack_start (GTK_BOX (content),
        GTK_WIDGET (content_box), FALSE, FALSE, 5);

  gtk_box_pack_start (GTK_BOX (content),
         gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  GtkWidget *action_area = gtk_dialog_get_action_area (dialog);
  gtk_widget_set_margin_start (action_area, 5);
  gtk_widget_set_margin_end (action_area, 5);
  gtk_widget_set_margin_top (action_area, 5);
  gtk_widget_set_margin_bottom (action_area, 5);
#pragma GCC diagnostic pop

  gtk_widget_set_vexpand (GTK_WIDGET(content_box), TRUE);
  gtk_widget_set_margin_top (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_bottom (GTK_WIDGET(content_box), 5);
  gtk_widget_set_margin_start (GTK_WIDGET(content_box), 10);
  gtk_widget_set_margin_end (GTK_WIDGET(content_box), 10);

  g_object_unref (pb);
  }


/*======================================================================

  boilerplatediaologs_yesnocancel 

======================================================================*/
gint boilerplatedialogs_yesnocancel (GtkWindow *parent, 
     const char *caption, const char *message, gint deflt) 
  {
  GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkDialog *dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (caption,
     parent,
     flags,
     _("_Yes"),
     GTK_RESPONSE_YES,
     _("_No"),
     GTK_RESPONSE_NO,
     _("_Cancel"),
     GTK_RESPONSE_CANCEL,
     NULL));

  boilerplatedialogs_set_content_area (dialog, message, BPD_ICON_QUESTION);

  gtk_dialog_set_default_response (dialog, deflt);

  gtk_widget_show_all (GTK_WIDGET (dialog));
  int response = gtk_dialog_run (dialog);
  gtk_widget_destroy (GTK_WIDGET (dialog));
  return response;
  }


/*======================================================================

  boilerplatediaologs_yesno

======================================================================*/
gint boilerplatedialogs_yesno (GtkWindow *parent, 
     const char *caption, const char *message, gint deflt) 
  {
  GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkDialog *dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (caption,
     parent,
     flags,
     _("_Yes"),
     GTK_RESPONSE_YES,
     _("_No"),
     GTK_RESPONSE_NO,
     NULL));

  boilerplatedialogs_set_content_area (dialog, message, BPD_ICON_QUESTION);

  gtk_dialog_set_default_response (dialog, deflt);

  gtk_widget_show_all (GTK_WIDGET (dialog));
  int response = gtk_dialog_run (dialog);
  gtk_widget_destroy (GTK_WIDGET (dialog));
  return response;
  }

/*======================================================================

  boilerplatediaologs_error

======================================================================*/
void boilerplatedialogs_error (GtkWindow *parent, 
     const char *caption, const char *message) 
  {
  GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkDialog *dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (caption,
     parent,
     flags,
     _("_OK"),
     GTK_RESPONSE_OK,
     NULL));

  boilerplatedialogs_set_content_area (dialog, message, BPD_ICON_ERROR);

  gtk_widget_show_all (GTK_WIDGET (dialog));
  gtk_dialog_run (dialog);
  gtk_widget_destroy (GTK_WIDGET (dialog));
  }


/*======================================================================

  boilerplatediaologs_message

======================================================================*/
void boilerplatedialogs_message (GtkWindow *parent, 
     const char *caption, const char *message) 
  {
  GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkDialog *dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (caption,
     parent,
     flags,
     _("_OK"),
     GTK_RESPONSE_OK,
     NULL));

  boilerplatedialogs_set_content_area (dialog, message, BPD_ICON_INFO);

  gtk_widget_show_all (GTK_WIDGET (dialog));
  gtk_dialog_run (dialog);

  gtk_widget_destroy (GTK_WIDGET (dialog));
  }



