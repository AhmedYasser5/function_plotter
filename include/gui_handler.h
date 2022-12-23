#pragma once

#include <gtk/gtk.h>

#define RED 1.0
#define GREEN 0.0
#define BLUE 0.0
#define LINE_WIDTH 3.0
#define PRECISION 0.25

#define WIDTH 800.0
#define HEIGHT 600.0
#define HELPER_DX 5.0
#define HELPER_DY 5.0
#define FONT_SIZE 14

typedef struct {
  gboolean gridActive, helperActive;

  GtkWidget *restrict window;
  GtkWidget *restrict fixed;
  GtkWidget *restrict graph;
  GtkWidget *restrict draw;
  GtkWidget *restrict helper;
  GtkWidget *restrict grid;
  GtkWidget *restrict min_x;
  GtkWidget *restrict max_x;
  GtkWidget *restrict min_y;
  GtkWidget *restrict max_y;
  GtkWidget *restrict equation;
  GtkWidget *restrict messages;
  GtkWidget *restrict toggle_grid;
  GtkWidget *restrict toggle_helper;

  gdouble minX, maxX, minY, maxY, helperX, helperY;
  gdouble *restrict points;

  gchar *restrict eq;
} gui_handler;

void gui_handler_start_gui(int argc, char *argv[]);
