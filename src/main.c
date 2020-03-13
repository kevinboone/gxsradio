/*============================================================================
  
  gxsradio
  main.c

  Copyright (c)2020 Kevin Boone. Distributed under the terms of the GPL, v3.0

============================================================================*/
#include <gtk/gtk.h>
#include "RadioApplication.h"

int main (int argc, char **argv)
  {
  RadioApplication *app = radioapplication_new ();
  int ret = boilerplateapplication_run 
        (BOILERPLATEAPPLICATION (app), &argc, &argv);
  g_object_unref (app);
  return ret;
  }

