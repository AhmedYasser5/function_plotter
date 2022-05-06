#include "calculator.h"
#include "stack.h"
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 1.0
#define GREEN 0.0
#define BLUE 0.0
#define LINE_WIDTH 3.0
#define PRECISION 0.05

#define MAX_LABEL_SIZE 128
#define WIDTH 800.0
#define HEIGHT 600.0
#define HELPER_HEIGHT 30.0

GtkWidget *window, *fixed, *graph, *helper, *draw, *min_x, *max_x, *min_y,
    *max_y, *equation, *messages;

gdouble minX, maxX, minY, maxY, helperX = WIDTH / 2.0;

gchar eq[MAX_LABEL_SIZE];

stack_point *p;

void on_destroy() { gtk_main_quit(); }

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  GtkBuilder *builder = gtk_builder_new_from_file("plotter.glade");

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

  g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

  gtk_builder_connect_signals(builder, NULL);

  fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
  graph = GTK_WIDGET(gtk_builder_get_object(builder, "graph"));
  helper = GTK_WIDGET(gtk_builder_get_object(builder, "helper"));
  draw = GTK_WIDGET(gtk_builder_get_object(builder, "draw"));
  min_x = GTK_WIDGET(gtk_builder_get_object(builder, "min_x"));
  max_x = GTK_WIDGET(gtk_builder_get_object(builder, "max_x"));
  min_y = GTK_WIDGET(gtk_builder_get_object(builder, "min_y"));
  max_y = GTK_WIDGET(gtk_builder_get_object(builder, "max_y"));
  equation = GTK_WIDGET(gtk_builder_get_object(builder, "equation"));
  messages = GTK_WIDGET(gtk_builder_get_object(builder, "messages"));

  g_object_unref(builder);

  gtk_widget_show(window);

  gtk_main();

  return EXIT_SUCCESS;
}

static gdouble convert_to_display(gdouble p, const gdouble *min,
                                  const gdouble *max, const guint len) {
  p -= *min;
  p *= len;
  p /= *max - *min;
  return p;
}

static gdouble convert_from_display(gdouble p, const gdouble *min,
                                    const gdouble *max, const guint len) {
  p *= *max - *min;
  p /= len;
  p += *min;
  return p;
}

static gboolean mouse_tracking() {
  if (p == NULL)
    return FALSE;
  char *message = (char *)malloc(sizeof(char) * MAX_LABEL_SIZE);
  gdouble y, x = convert_from_display(helperX, &minX, &maxX, WIDTH);
  if (calculator_eval(eq, x, &y, message)) {
    gtk_label_set_text(GTK_LABEL(messages), message);
    free(message);
    return FALSE;
  }
  sprintf(message, "(x, y) = (%lf, %lf)", x, y);
  gtk_label_set_text(GTK_LABEL(messages), message);
  free(message);
  gtk_widget_queue_draw(helper);
  return TRUE;
}

gboolean on_helper_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data) {
  if (event->button != 1)
    return FALSE;
  helperX = event->x;
  return mouse_tracking();
}

gboolean on_graph_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                     gpointer data) {
  if (event->button != 1)
    return FALSE;
  helperX = event->x;
  return mouse_tracking();
}

gboolean on_helper_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
                                       gpointer data) {
  if (!(event->state & GDK_BUTTON1_MASK))
    return FALSE;
  helperX = event->x;
  return mouse_tracking();
}

gboolean on_graph_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
                                      gpointer data) {
  if (!(event->state & GDK_BUTTON1_MASK))
    return FALSE;
  helperX = event->x;
  return mouse_tracking();
}

gboolean on_helper_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  if (isnan(helperX))
    return FALSE;
  cairo_set_line_width(cr, LINE_WIDTH);
  cairo_set_source_rgb(cr, 1 - RED, 1 - GREEN, 1 - BLUE);
  cairo_move_to(cr, helperX, HELPER_HEIGHT);
  cairo_line_to(cr, helperX, HELPER_HEIGHT / 3.0);
  cairo_stroke(cr);
  gdouble dx = HELPER_HEIGHT / (3 * tan(M_PI / 3.0));
  cairo_move_to(cr, helperX, 0);
  cairo_line_to(cr, helperX - dx, HELPER_HEIGHT / 3.0);
  cairo_line_to(cr, helperX + dx, HELPER_HEIGHT / 3.0);
  cairo_close_path(cr);
  cairo_fill(cr);
  return TRUE;
}

gboolean on_graph_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  if (p == NULL)
    return FALSE;

  cairo_set_line_width(cr, LINE_WIDTH);
  cairo_set_source_rgb(cr, RED, GREEN, BLUE);

  stack_point *curP = p;

  while (curP->next != NULL) {
    if (!isnan(curP->top_y) && !isnan(curP->next->top_y)) {
      cairo_move_to(cr, convert_to_display(curP->top_x, &minX, &maxX, WIDTH),
                    HEIGHT -
                        convert_to_display(curP->top_y, &minY, &maxY, HEIGHT));
      cairo_line_to(
          cr, convert_to_display(curP->next->top_x, &minX, &maxX, WIDTH),
          HEIGHT - convert_to_display(curP->next->top_y, &minY, &maxY, HEIGHT));
      cairo_stroke(cr);
    }
    curP = curP->next;
  }

  return TRUE;
}

static char set_min_max(GtkWidget *min, GtkWidget *max, gdouble *min_co,
                        gdouble *max_co, const char c) {
  char state = 0;
  const gchar *tmp = gtk_entry_get_text(GTK_ENTRY(min));
  *min_co = calculator_atolf(tmp);
  if (isnan(*min_co)) {
    state = 1;
    if (!strcmp(tmp, ""))
      state *= -1;
  }
  tmp = gtk_entry_get_text(GTK_ENTRY(max));
  *max_co = calculator_atolf(tmp);
  if (state <= 0 && isnan(*max_co)) {
    if (!strcmp(tmp, "")) {
      if (!state)
        state = -2;
    } else
      state = 2;
  }
  if (!state && isless(*max_co, *min_co)) {
    state = 3;
    *min_co = *max_co = NAN;
  }
  printf("min_%c = %lf, max_%c = %lf\n", c, *min_co, c, *max_co);
  if (state) {
    char *message = (char *)malloc(sizeof(char) * MAX_LABEL_SIZE);
    if (abs(state) == 1)
      sprintf(message, "Invalid Minimum %c", c);
    else if (abs(state) == 2)
      sprintf(message, "Invalid Maximum %c", c);
    else
      sprintf(message, "Minimum %c should be less than Maximum %c", c, c);
    gtk_label_set_text(GTK_LABEL(messages), message);
    free(message);
  }
  return state;
}

void on_draw_clicked(GtkButton *draw) {
  if (set_min_max(min_x, max_x, &minX, &maxX, 'x') ||
      set_min_max(min_y, max_y, &minY, &maxY, 'y') > 0)
    return;
  gboolean isMaxYNan = FALSE, isMinYNan = FALSE;
  if (isnan(minY)) {
    isMinYNan = TRUE;
    minY = INFINITY;
  }
  if (isnan(maxY)) {
    isMaxYNan = TRUE;
    maxY = -INFINITY;
  }
  strcpy(eq, gtk_entry_get_text(GTK_ENTRY(equation)));
  printf("equation = %s\n", eq);
  if (!strcmp(eq, "")) {
    gtk_label_set_text(GTK_LABEL(messages), "Equation cannot be empty");
    return;
  }
  gdouble step = PRECISION * (maxX - minX) / WIDTH, cur = minX;
  stack_point_clear(&p);
  gtk_label_set_text(GTK_LABEL(messages), "");
  gchar *tmp = (char *)malloc(sizeof(char) * MAX_LABEL_SIZE);
  while (isgreaterequal(maxX, cur)) {
    stack_point_push(&p, cur, NAN);
    calculator_eval(eq, cur, &p->top_y, tmp);
    if (isnan(p->top_y) || (!isMinYNan && isless(p->top_y, minY) ||
                            (!isMaxYNan && isless(maxY, p->top_y)))) {
      p->top_y = NAN;
      gtk_label_set_text(GTK_LABEL(messages), tmp);
    } else {
      if (isMinYNan)
        minY = fmin(minY, p->top_y);
      if (isMaxYNan)
        maxY = fmax(maxY, p->top_y);
    }
    cur += step;
  }
  free(tmp);
  if (isgreaterequal(minY, maxY) && isgreaterequal(maxY, minY)) {
    minY--;
    maxY++;
  }
  gtk_widget_queue_draw(graph);
}
