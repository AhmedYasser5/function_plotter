#include "gui_handler.h"
#include "calculator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* unrefs any created widgets */
void on_destroy() { gtk_main_quit(); }

/* prints errors in red color */
static void print_label_error(GtkWidget *messages, const char *message) {
  char *output = (char *)malloc(sizeof(char) * MAX_LABEL_SIZE * 2);
  sprintf(
      output,
      "<span font=\"Sans 14\" weight=\"semibold\" color=\"#FF0000\">%s</span>",
      message);
  gtk_label_set_markup(GTK_LABEL(messages), output);
  free(output);
}

/* prints information in default color */
static void print_label_info(GtkWidget *messages, const char *message) {
  char *output = (char *)malloc(sizeof(char) * MAX_LABEL_SIZE * 2);
  sprintf(output, "<span font=\"Sans 14\" weight=\"semibold\">%s</span>",
          message);
  gtk_label_set_markup(GTK_LABEL(messages), output);
  free(output);
}

/* toggles the appearance of the grid */
void on_toggle_grid_toggled(GtkToggleButton *toggle_grid, gpointer data) {
  gui_handler *handler = data;
  handler->gridActive = gtk_toggle_button_get_active(toggle_grid);
  gtk_widget_queue_draw(handler->grid);
}

/* toggles the appearance of the helper */
void on_toggle_helper_toggled(GtkToggleButton *toggle_helper, gpointer data) {
  gui_handler *handler = data;
  handler->helperActive = gtk_toggle_button_get_active(toggle_helper);
  gtk_widget_queue_draw(handler->helper);
}

/* converts real coordinates from gui coordinates */
static gdouble convert_to_display(gdouble p, const gdouble *min,
                                  const gdouble *max, const guint len) {
  p -= *min;
  p *= len;
  p /= *max - *min;
  return p;
}

/* converts gui coordinates to real coordinates */
static gdouble convert_from_display(gdouble p, const gdouble *min,
                                    const gdouble *max, const guint len) {
  p *= *max - *min;
  p /= len;
  p += *min;
  return p;
}

/* tracks the mouse in the helper */
static gboolean mouse_tracking(gui_handler *handler) {
  // make sure the helper is turned on
  if (handler->eq == NULL || !handler->helperActive)
    return FALSE;

  GtkWidget *messages = handler->messages;
  gchar *message = handler->message;
  gdouble *minX = &handler->minX, *maxX = &handler->maxX,
          *minY = &handler->minY, *maxY = &handler->maxY;
  gdouble *helperX = &handler->helperX, *helperY = &handler->helperY;

  // we should subtract the border's width "HELPER_DX"
  gdouble approxX =
      convert_from_display(*helperX - HELPER_DX, minX, maxX, WIDTH);
  gdouble step = PRECISION * (*maxX - *minX) / WIDTH;
  // approximating the location of the point that the mouse is pointing at
  int index = (approxX - *minX) / step;
  approxX =
      index * step + *minX; // get the x-coordinate that correspond to approxY
  gdouble approxY = handler->points[index];

  // handling out-of-bounds points
  if (isless(approxX, *minX) || isless(*maxX, approxX)) {
    approxX = approxY = NAN;
    *helperX = NAN;
  }

  if (isnan(approxY))
    *helperY = NAN;
  else
    *helperY =
        HEIGHT - convert_to_display(approxY, minY, maxY, HEIGHT) +
        HELPER_DY; // the y-axis is inverted since it starts from the top.. we
                   // should also add "HELPER_DY" to avoid drawing at the border

  gtk_widget_queue_draw(handler->helper);
  sprintf(message, "(x, y) = (%lf, %lf)", approxX, approxY);
  print_label_info(messages, message);
  return TRUE;
}

/* tracks mouse clicks in the helper */
gboolean on_helper_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data) {
  // make sure it is a left click of the mouse
  if (event->button != 1)
    return FALSE;

  gui_handler *handler = data;
  handler->helperX = event->x;
  return mouse_tracking(handler);
}

/* tracks mouse motion (while clicked) in the helper */
gboolean on_helper_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
                                       gpointer data) {
  gui_handler *handler = data;
  // make sure it is a left click of the mouse, and the last point is somewhere
  // far from this new point (this decreases the load on the cpu so that it does
  // not draw on every little move of the mouse)
  if (!(event->state & GDK_BUTTON1_MASK) ||
      isless(fabs(handler->helperX - event->x), PRECISION))
    return FALSE;

  handler->helperX = event->x;
  return mouse_tracking(handler);
}

/* draws moving axes to identify the point on the graph */
gboolean on_helper_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  gui_handler *handler = data;
  // make sure the helper is turned on
  if (isnan(handler->helperX) || !handler->helperActive)
    return FALSE;

  cairo_set_line_width(cr, LINE_WIDTH / 3.0);
  cairo_set_source_rgb(cr, 1 - RED, 1 - GREEN, 1 - BLUE);

  if (!isnan(handler->helperX)) {
    cairo_move_to(cr, handler->helperX, 0);
    cairo_line_to(cr, handler->helperX, HEIGHT + 2 * HELPER_DY);
  }

  if (!isnan(handler->helperY)) {
    cairo_move_to(cr, 0, handler->helperY);
    cairo_line_to(cr, WIDTH + 2 * HELPER_DX, handler->helperY);
  }

  cairo_stroke(cr);
  return TRUE;
}

/* draws static axes to identify the ranges of x and y axes */
gboolean on_grid_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  gui_handler *handler = data;

  // make sure the grid is turned on
  if (handler->gridActive) {
    // the whole height and width:
    gdouble height = HEIGHT + 2 * HELPER_DY, width = WIDTH + 2 * HELPER_DX;
    cairo_set_line_width(cr, LINE_WIDTH / 3.0);
    cairo_set_source_rgba(cr, 1, 1, 1, 0.75);

    // drawing x-axis ticks
    for (gdouble x = 0, step = width / 8.0; x <= width; x += step) {
      cairo_move_to(cr, x, height);
      cairo_line_to(cr, x, height - HELPER_DY);
    }

    // drawing y-axis ticks
    for (gdouble y = 0, step = height / 6.0; y <= height; y += step) {
      cairo_move_to(cr, 0, height - y);
      cairo_line_to(cr, HELPER_DX, height - y);
    }

    cairo_stroke(cr);

    // if there is no equation, don't continue
    if (handler->eq == NULL)
      return TRUE;

    cairo_set_font_size(cr, FONT_SIZE);
    char *coordinates = (char *)malloc(sizeof(char) * 15);

    // write down x-axis numbers
    for (gdouble x = 0, step = width / 8.0; x <= width; x += step) {
      int num = sprintf(coordinates, "%.3g",
                        convert_from_display(x - HELPER_DX, &handler->minX,
                                             &handler->maxX, WIDTH));
      cairo_move_to(cr, x - num * FONT_SIZE / 4.0, height - 2 * HELPER_DY);
      cairo_text_path(cr, coordinates);
    }

    // write down y-axis numbers
    for (gdouble y = 0, step = height / 6.0; y <= height; y += step) {
      sprintf(coordinates, "%.3g",
              convert_from_display(y - HELPER_DY, &handler->minY,
                                   &handler->maxY, HEIGHT));
      cairo_move_to(cr, 2 * HELPER_DX, height - y + FONT_SIZE / 4.0);
      cairo_text_path(cr, coordinates);
    }

    cairo_fill(cr);
    free(coordinates);
  }
  return TRUE;
}

/* draws the given equation */
gboolean on_graph_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  gui_handler *handler = data;

  if (handler->eq == NULL)
    return FALSE;

  cairo_set_line_width(cr, LINE_WIDTH);
  cairo_set_source_rgb(cr, RED, GREEN, BLUE);

  gdouble *p = handler->points;
  gdouble *minX = &handler->minX, *maxX = &handler->maxX,
          *minY = &handler->minY, *maxY = &handler->maxY;
  gdouble cur = *minX, step = PRECISION * (*maxX - *minX) / WIDTH;

  for (int i = 0, n = WIDTH / PRECISION; i + 1 < n; i++, cur += step) {

    if (!isnan(p[i]) && !isnan(p[i + 1])) {
      // we should subtract from "HEIGHT" since the y-axis is inverted
      cairo_move_to(cr, convert_to_display(cur, minX, maxX, WIDTH),
                    HEIGHT - convert_to_display(p[i], minY, maxY, HEIGHT));

      cairo_line_to(cr, convert_to_display(cur + step, minX, maxX, WIDTH),
                    HEIGHT - convert_to_display(p[i + 1], minY, maxY, HEIGHT));

      cairo_stroke(cr);
    }
  }

  return TRUE;
}

/* handles different errors in the ranges of x and y given in entry boxes */
static char set_min_max(GtkWidget *min, GtkWidget *max, gdouble *min_co,
                        gdouble *max_co, const char c, GtkWidget *messages,
                        gchar *message) {
  char state = 0;

  // read Minimum x or y
  const gchar *tmp = gtk_entry_get_text(GTK_ENTRY(min));
  *min_co = calculator_atolf(tmp);
  if (isnan(*min_co)) {
    state = 1;
    if (!strcmp(tmp, ""))
      state *= -1;
  }

  // read Maximum x or y
  tmp = gtk_entry_get_text(GTK_ENTRY(max));
  *max_co = calculator_atolf(tmp);
  if (state <= 0 && isnan(*max_co)) {
    if (!strcmp(tmp, "")) {
      if (!state)
        state = -2;
    } else
      state = 2;
  }

  // make sure Minimum x or y is less than Maximum x or y
  if (!state && isless(*max_co, *min_co)) {
    state = 3;
    *min_co = *max_co = NAN;
  }

  // make sure Minimum x or y is not equal to Maximum x or y
  if (!state && isgreaterequal(*min_co, *max_co) &&
      isgreaterequal(*max_co, *min_co)) {
    state = 4;
    *min_co = *max_co = NAN;
  }

  // report min and max for debugging
  printf("min_%c = %lf, max_%c = %lf\n", c, *min_co, c, *max_co);
  // handling errors
  if (state) {
    if (abs(state) == 1)
      sprintf(message, "Invalid Minimum %c", c);
    else if (abs(state) == 2)
      sprintf(message, "Invalid Maximum %c", c);
    else if (state == 3)
      sprintf(message, "Minimum %c should be less than Maximum %c", c, c);
    else
      sprintf(message, "Minimum %c cannot be equal to Maximum %c", c, c);
    print_label_error(messages, message);
  }

  return state;
}

/* prepares data to be drawn on the graph */
void on_draw_clicked(GtkButton *draw, gpointer data) {
  gui_handler *handler = data;

  GtkWidget *min_x = handler->min_x, *max_x = handler->max_x,
            *min_y = handler->min_y, *max_y = handler->max_y;
  gdouble *minX = &handler->minX, *maxX = &handler->maxX,
          *minY = &handler->minY, *maxY = &handler->maxY;

  GtkWidget *messages = handler->messages;
  gchar *message = handler->message;

  // errors of x values are intolerable, while empty y values are allowed
  if (set_min_max(min_x, max_x, minX, maxX, 'x', messages, message) ||
      set_min_max(min_y, max_y, minY, maxY, 'y', messages, message) > 0)
    return;

  // setting auto y values; for example, if isMaxYNan is true, the maxY will be
  // set to auto
  gboolean isMaxYNan = FALSE, isMinYNan = FALSE;
  if (isnan(*minY)) {
    isMinYNan = TRUE;
    *minY = INFINITY;
  }
  if (isnan(*maxY)) {
    isMaxYNan = TRUE;
    *maxY = -INFINITY;
  }

  const gchar *originalEq = gtk_entry_get_text(GTK_ENTRY(handler->equation));

  if (handler->eq)
    free(handler->eq);
  handler->eq = (char *)malloc(sizeof(char) * strlen(originalEq) + 1);

  gchar *eq = handler->eq;
  strcpy(eq, originalEq);
  printf("function = %s\n", eq);

  if (!strcmp(eq, "")) {
    print_label_error(messages, "Function cannot be empty");
    return;
  }

  gtk_label_set_text(GTK_LABEL(messages), "");
  gdouble *p = handler->points;
  gdouble step = PRECISION * (*maxX - *minX) / WIDTH, cur = *minX;
  for (int i = 0, n = WIDTH / PRECISION; i < n; i++, cur += step) {
    // calculating the corresponding y for x = cur
    calculator_eval(eq, cur, p + i, message);

    if (isnan(p[i])) { // handling errors
      print_label_error(messages, message);
    } else if ((!isMinYNan && isless(p[i], *minY) ||
                (!isMaxYNan && isless(*maxY, p[i])))) {
      // out-of-bounds' values should be NANs
      p[i] = NAN;
    } else {
      // otherwise, y values that are set to auto should be updated
      if (isMinYNan)
        *minY = fmin(*minY, p[i]);
      if (isMaxYNan)
        *maxY = fmax(*maxY, p[i]);
    }
  }

  // equal limits makes the line appear as if there is no drawing
  if (isgreaterequal(*minY, *maxY) && isgreaterequal(*maxY, *minY)) {
    *minY -= 0.01;
    *maxY += 0.01;
  }

  gtk_widget_queue_draw(handler->graph);
}

/* handles all gui stuff using a gui_handler */
void gui_handler_start_gui(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  gui_handler handler;

  // initializing variables
  handler.gridActive = handler.helperActive = TRUE;
  handler.helperX = WIDTH / 2.0 + HELPER_DX;
  handler.helperY = HEIGHT / 2.0 + HELPER_DY;
  handler.eq = NULL;
  handler.points =
      (gdouble *)malloc(sizeof(gdouble) * (int)(WIDTH / PRECISION));

  GtkBuilder *builder = gtk_builder_new_from_file("plotter.glade");
  gtk_builder_connect_signals(builder, NULL);

  // getting widgets from glade file
  handler.window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  handler.fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
  handler.graph = GTK_WIDGET(gtk_builder_get_object(builder, "graph"));
  handler.helper = GTK_WIDGET(gtk_builder_get_object(builder, "helper"));
  handler.grid = GTK_WIDGET(gtk_builder_get_object(builder, "grid"));
  handler.draw = GTK_WIDGET(gtk_builder_get_object(builder, "draw"));
  handler.min_x = GTK_WIDGET(gtk_builder_get_object(builder, "min_x"));
  handler.max_x = GTK_WIDGET(gtk_builder_get_object(builder, "max_x"));
  handler.min_y = GTK_WIDGET(gtk_builder_get_object(builder, "min_y"));
  handler.max_y = GTK_WIDGET(gtk_builder_get_object(builder, "max_y"));
  handler.equation = GTK_WIDGET(gtk_builder_get_object(builder, "equation"));
  handler.messages = GTK_WIDGET(gtk_builder_get_object(builder, "messages"));
  handler.toggle_grid =
      GTK_WIDGET(gtk_builder_get_object(builder, "toggle_grid"));
  handler.toggle_helper =
      GTK_WIDGET(gtk_builder_get_object(builder, "toggle_helper"));

  g_object_unref(builder);

  g_signal_connect(handler.window, "destroy", G_CALLBACK(on_destroy), NULL);

  // passing the handler to every event
  g_signal_connect(handler.graph, "draw", G_CALLBACK(on_graph_draw), &handler);
  g_signal_connect(handler.draw, "clicked", G_CALLBACK(on_draw_clicked),
                   &handler);

  g_signal_connect(handler.toggle_helper, "toggled",
                   G_CALLBACK(on_toggle_helper_toggled), &handler);
  g_signal_connect(handler.toggle_grid, "toggled",
                   G_CALLBACK(on_toggle_grid_toggled), &handler);

  g_signal_connect(handler.grid, "draw", G_CALLBACK(on_grid_draw), &handler);

  g_signal_connect(handler.helper, "button-press-event",
                   G_CALLBACK(on_helper_button_press_event), &handler);
  g_signal_connect(handler.helper, "draw", G_CALLBACK(on_helper_draw),
                   &handler);
  g_signal_connect(handler.helper, "motion-notify-event",
                   G_CALLBACK(on_helper_motion_notify_event), &handler);

  gtk_widget_show(handler.window);

  gtk_main();

  // freeing manually allocated memory
  if (handler.eq)
    free(handler.eq);
  free(handler.points);
}
