#pragma once

#include <gtk/gtk.h>

#define RED 1.0
#define GREEN 0.0
#define BLUE 0.0
#define LINE_WIDTH 3.0
#define PRECISION 0.5

#define MAX_LABEL_SIZE 128
#define WIDTH 800.0
#define HEIGHT 600.0
#define HELPER_DX 5.0
#define HELPER_DY 5.0
#define FONT_SIZE 16.0

typedef struct {
  gboolean gridActive, helperActive;

  GtkWidget *window, *fixed, *graph, *draw, *helper, *grid, *min_x, *max_x,
      *min_y, *max_y, *equation, *messages, *toggle_grid, *toggle_helper;

  gdouble minX, maxX, minY, maxY, helperX, helperY, *points;

  gchar *eq, message[MAX_LABEL_SIZE];
} gui_handler;

void gui_handler_start_gui(int argc, char *argv[]);
