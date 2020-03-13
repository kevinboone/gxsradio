/*======================================================================

  gsxradio
  BoilerplateDialogs.h

  Various basic dialog boxes that most applications will use.

======================================================================*/

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/** Show a dialog box with 'yes', 'no' cancel buttons. The
    return value is one of the GTK_RESPONSE_XXX codes */
gint   boilerplatedialogs_yesnocancel (GtkWindow *parent, 
           const char *caption, const char *message, gint deflt);

/** Show a dialog box with 'yes' and 'no' buttons. The
    return value is one of the GTK_RESPONSE_XXX codes */
gint   boilerplatedialogs_yesno (GtkWindow *parent, 
           const char *caption, const char *message, gint deflt);

/** Show an error dialog box */ 
void   boilerplatedialogs_error (GtkWindow *parent, 
           const char *caption, const char *message);

/** Show a general message dialog box */
void   boilerplatedialogs_message (GtkWindow *parent, 
           const char *caption, const char *message);

G_END_DECLS




