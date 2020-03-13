/*=============================================================================
  
  gsxradio
  BoilerplateApplication.h

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

=============================================================================*/
#pragma once

#include <gtk/gtk.h>
#include "BoilerplateMainWindow.h"

G_BEGIN_DECLS


#define BOILERPLATEAPPLICATION_TYPE            (boilerplateapplication_get_type ())
#define BOILERPLATEAPPLICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BOILERPLATEAPPLICATION_TYPE, BoilerplateApplication))
#define BOILERPLATEAPPLICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BOILERPLATEAPPLICATION_TYPE, BoilerplateApplicationClass))
#define IS_BOILERPLATEAPPLICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BOILERPLATEAPPLICATION_TYPE))
#define IS_BOILERPLATEAPPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BOILERPLATEAPPLICATION_TYPE))
#define BOILERPLATEAPPLICATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BOILERPLATEAPPLICATION_TYPE, BoilerplateApplicationClass))

typedef struct _BoilerplateApplication BoilerplateApplication;
typedef struct _BoilerplateApplicationClass  BoilerplateApplicationClass;

/** BoilerplateApplication is the basic main class for the application.
    Its main tasks are to parse the command line, create and initialize a
    top-level window, and then run the GTK event loop. The application's
    lifetime is until the initial window is destroyed; it is envisaged
    that either a controller class or the initial window itself will
    destroy the initial window, after ensuring that it is safe to
    quit. As a minimum, subclasses need to override create_main_window()*/
struct _BoilerplateApplication
  {
  GObject base;
  };

struct _BoilerplateApplicationClass
  {
  GObjectClass parent_class;

  /** Create an initial, top-level window for this application. 
      Subclasses should always override this, and provide their own 
      top-level main window, which should be a subclass of 
      BoilerplateMainWindow */
  BoilerplateMainWindow *(*create_main_window) (BoilerplateApplication *self);
  
  /** Parse the command line. The default version just checks for 
      --version; subclasses could usefully do this also. The run_program
      argument should be set to FALSE if the command line, although
      correctly structured, should result in the GUI not starting
      up. This would be the case for --version, --help, etc. */
  gboolean               (*parse_options) (BoilerplateApplication *self, 
                             int *_argc, char ***_argv, char **error, 
			     gboolean *run_program);
  
  void                   (*read_rc_files) (BoilerplateApplication *self);
  
  void                   (*read_rc_file) (const BoilerplateApplication *self,
                            const char *rc_filename);
  
  /** Get an about message showing the program version. */
  const char            *(*get_about_msg) (const BoilerplateApplication *self);

  /** First idle is called once, just before the main loop starts, and when
      GTK believes it is idle. The user interface may or may not be
      visible on screen at this point -- that depends on the vagaries
      of X. However, it is a safe place to do things like raising an
      initial "connect to server" dialog box, or similar, or to
      open a file that may raise an error. It's not a good place to
      start long-running user interface operations that do not allow
      the UI to redraw */
  void                   (*first_idle) (BoilerplateApplication *self);

  char                  *(*get_system_rc_file) 
                            (const BoilerplateApplication *self);
  char                  *(*get_user_rc_file) 
                            (const BoilerplateApplication *self);
  gpointer padding[12];
  };

GType boilerplateapplication_get_type (void) G_GNUC_CONST;

BoilerplateApplication 
  *boilerplateapplication_new (void);

/** Run this application. This method cannot be overridden. */
int                     
  boilerplateapplication_run (BoilerplateApplication *self, int *argc, 
     char ***argv);

/** Get an "about" or "version" message. Subclasses can override this
    to produce their own messages. The message is internal to the
    class, and should not be freed by callers. */
const char             
  *boilerplateapplication_get_about_msg (const BoilerplateApplication *self);

BoilerplateApplication *boilerplatecontroller_get_application
    (const BoilerplateController *self);

BoilerplateMainWindow *boilerplateapplication_get_main_window
       (const BoilerplateApplication *self);

char                  *boilerplateapplication_get_user_rc_file
                            (const BoilerplateApplication *self);
G_END_DECLS





