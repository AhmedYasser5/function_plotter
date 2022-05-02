#include "calculator.h"
#include <ctype.h>
#include <gtk/gtk.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PRECISION 0.05

GtkWidget *window;
GtkWidget *fixed;
GtkWidget *graph;
GtkWidget *draw;
GtkWidget *min;
GtkWidget *max;
GtkWidget *equation;
GtkWidget *errors;

gdouble minX, maxX, minY, maxY;
stack_double *x, *y;

void on_destroy();

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv); // init Gtk

  GtkBuilder *builder = gtk_builder_new_from_file("plotter.glade");

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

  g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

  gtk_builder_connect_signals(builder, NULL);

  fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
  graph = GTK_WIDGET(gtk_builder_get_object(builder, "graph"));
  draw = GTK_WIDGET(gtk_builder_get_object(builder, "draw"));
  min = GTK_WIDGET(gtk_builder_get_object(builder, "min"));
  max = GTK_WIDGET(gtk_builder_get_object(builder, "max"));
  equation = GTK_WIDGET(gtk_builder_get_object(builder, "equation"));
  errors = GTK_WIDGET(gtk_builder_get_object(builder, "errors"));

  g_object_unref(builder);

  gtk_widget_show(window);

  gtk_main();

  return EXIT_SUCCESS;
}

void on_destroy() { gtk_main_quit(); }

static gdouble convert_to_display(gdouble p, const gdouble *min,
                                  const gdouble *max, const guint *len) {
  p -= *min;
  p *= *len;
  p /= *max - *min;
  return p;
}

gboolean on_graph_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {

  guint width = 800, height = 600;

  // width = gtk_widget_get_allocated_width(widget);
  // height = gtk_widget_get_allocated_height(widget);

  if (x == NULL)
    return FALSE;

  cairo_set_line_width(cr, 3.0);
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);

  stack_double *curX = x, *curY = y;

  while (curX->next != NULL) {
    cairo_move_to(cr, convert_to_display(curX->top, &minX, &maxX, &width),
                  height -
                      convert_to_display(curY->top, &minY, &maxY, &height));
    cairo_line_to(
        cr, convert_to_display(curX->next->top, &minX, &maxX, &width),
        height - convert_to_display(curY->next->top, &minY, &maxY, &height));
    cairo_stroke(cr);
    curX = curX->next;
    curY = curY->next;
  }

  return FALSE;
}

void on_draw_clicked(GtkButton *draw) {
  int i = 0;
  const gchar *tmp = gtk_entry_get_text(GTK_ENTRY(min));
  if (tmp[0] == '-' || tmp[0] == '+')
    i++;
  minX = parse_number(tmp, &i);
  if (tmp[0] == '-')
    minX *= -1;
  i = 0;
  tmp = gtk_entry_get_text(GTK_ENTRY(max));
  if (tmp[0] == '-' || tmp[0] == '+')
    i++;
  maxX = parse_number(tmp, &i);
  if (tmp[0] == '-')
    maxX *= -1;
  if (isless(maxX, minX))
    return;
  printf("min = %lf, max = %lf\n", minX, maxX);
  const gchar *eq = gtk_entry_get_text(GTK_ENTRY(equation));
  printf("equation = %s\n", eq);
  gdouble step = PRECISION * (maxX - minX) / 800, cur = minX;
  maxY = -INFINITY;
  minY = INFINITY;
  clear_double(&x);
  clear_double(&y);
  while (!isless(maxX, cur)) {
    push_double(&x, cur);
    gchar *tmp = eval(eq, cur);
    if ((tmp[0] >= 'A' && tmp[0] <= 'Z') || (tmp[0] >= 'a' && tmp[0] <= 'z')) {
      gtk_label_set_text(GTK_LABEL(errors), tmp);
      free(tmp);
      return;
    }
    gtk_label_set_text(GTK_LABEL(errors), "");
    i = 0;
    if (tmp[0] == '-')
      i++;
    gdouble ans = parse_number(tmp, &i);
    if (tmp[0] == '-')
      ans *= -1;
    push_double(&y, ans);
    free(tmp);
    minY = fmin(minY, y->top);
    maxY = fmax(maxY, y->top);
    // printf("(%lf, %lf) ", cur, y->top);
    cur += step;
  }
  if (!isless(minY, maxY) && !isless(maxY, minY)) {
    minY--;
    maxY++;
  }
  // printf("\n");
  gtk_widget_queue_draw(graph);
}
