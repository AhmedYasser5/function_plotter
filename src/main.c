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

#define MAX_LABEL_SIZE 128
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

static gboolean atolf(const gchar *snum, double *num) {
  int i = 0;
  if (snum[0] == '-' || snum[0] == '+')
    i++;
  *num = calculator_parse_number(snum, &i);
  if (snum[0] == '-')
    *num *= -1;
  return snum[i + 1] == '\0';
}

static gboolean set_min_max() {
  const gchar *tmp = gtk_entry_get_text(GTK_ENTRY(min));
  if (!atolf(tmp, &minX)) {
    gtk_label_set_text(GTK_LABEL(errors), "Invalid Minimum x");
    return FALSE;
  }
  tmp = gtk_entry_get_text(GTK_ENTRY(max));
  if (!atolf(tmp, &maxX)) {
    gtk_label_set_text(GTK_LABEL(errors), "Invalid Maximum x");
    return FALSE;
  }
  if (isless(maxX, minX)) {
    gtk_label_set_text(GTK_LABEL(errors),
                       "Minimum x should be less than Maximum x");
    return FALSE;
  }
  printf("min = %lf, max = %lf\n", minX, maxX);
  return TRUE;
}

void on_draw_clicked(GtkButton *draw) {
  if (!set_min_max())
    return;
  const gchar *eq = gtk_entry_get_text(GTK_ENTRY(equation));
  printf("equation = %s\n", eq);
  gdouble step = PRECISION * (maxX - minX) / 800, cur = minX;
  maxY = -INFINITY;
  minY = INFINITY;
  stack_double_clear(&x);
  stack_double_clear(&y);
  gchar *tmp = (char *)malloc(sizeof(char) * MAX_LABEL_SIZE);
  while (!isless(maxX, cur)) {
    stack_double_push(&x, cur);
    stack_double_push(&y, calculator_eval(eq, cur, tmp));
    gtk_label_set_text(GTK_LABEL(errors), tmp);
    if (tmp[0] != '\0') {
      free(tmp);
      return;
    }
    minY = fmin(minY, y->top);
    maxY = fmax(maxY, y->top);
    // printf("(%lf, %lf) ", cur, y->top);
    cur += step;
  }
  free(tmp);
  if (!isless(minY, maxY) && !isless(maxY, minY)) {
    minY--;
    maxY++;
  }
  // printf("\n");
  gtk_widget_queue_draw(graph);
}
