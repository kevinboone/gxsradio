/*=============================================================================
  
  gsxradio
  RadioApplication.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateApplication.h"

G_BEGIN_DECLS


#define RADIOAPPLICATION_TYPE            (radioapplication_get_type ())
#define RADIOAPPLICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RADIOAPPLICATION_TYPE, RadioApplication))
#define RADIOAPPLICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RADIOAPPLICATION_TYPE, RadioApplicationClass))
#define IS_RADIOAPPLICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RADIOAPPLICATION_TYPE))
#define IS_RADIOAPPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RADIOAPPLICATION_TYPE))
#define RADIOAPPLICATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), RADIOAPPLICATION_TYPE, RadioApplicationClass))

typedef struct _RadioApplication RadioApplication;
typedef struct _RadioApplicationClass  RadioApplicationClass;

/** RadioApplication is a subclass of BoilerplateApplication. It handles
    those aspect of configuration management and command-line argument
    passing that are specific to this application. */
struct _RadioApplication
  {
  BoilerplateApplication base;
  };

struct _RadioApplicationClass
  {
  BoilerplateApplicationClass parent_class;
  gpointer padding[12];
  };

GType radioapplication_get_type (void) G_GNUC_CONST;

RadioApplication *radioapplication_new (void);

/** get_port() returns the port number that was set either in the RC file
    or as a command-line argument */
int               radioapplication_get_port (const RadioApplication *self);

/** get_host() returns the host name that was set either in the RC file
    or as a command-line argument */
const char       *radioapplication_get_host (const RadioApplication *self);

/** Get the current font, which should be in CSS format. */
const char       *radioapplication_get_font_name (const RadioApplication *self);

/** Gets the path used in the last file open or file save operation. */
const char       *radioapplication_get_last_pathname
                    (const RadioApplication *self);

/** Set the port of the xine-server host. This does not cause a
   connection to be made -- it just stores the value. */
void              radioapplication_set_port (RadioApplication *self, int port);

/** Set the hostname of the xine-server host. This does not cause a
   connection to be made -- it just stores the value. */
void              radioapplication_set_host (RadioApplication *self, 
                     const char *host);

/** Sets the font name, and applies the font change to the application. */
void              radioapplication_set_font_name (RadioApplication *self, 
                    const char *font_name);

/** Save the user RC file, if not in read-only mode. */
void              radioapplication_save_user_rc (const RadioApplication *self);

/** Save the path of the last file open of file save operation. This is
    stored in the user RC file. */
void              radioapplication_set_last_pathname (RadioApplication *self, 
                    const char *pathname);

/** Returns true if the application was launched with the --readonly
    command-line switch. */
gboolean          radioapplication_is_readonly (const RadioApplication *self);

const char       *radioapplication_get_fixed_directory 
                   (const RadioApplication *self);

gboolean          radioapplication_get_single_click_mode 
                     (const RadioApplication *self);

void              radioapplication_set_single_click_mode 
                     (RadioApplication *self, gboolean single);

G_END_DECLS





