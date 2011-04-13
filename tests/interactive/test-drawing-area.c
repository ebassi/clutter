#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cairo.h>
#include <clutter/clutter.h>

#define PADDING         18.0f

static gboolean
on_area_draw (ClutterDrawingArea *area,
              ClutterActor       *actor,
              cairo_t            *cr)
{
  GDateTime *now = g_date_time_new_now_local ();
  double hours = g_date_time_get_hour (now) * G_PI / 6.0;
  double minutes = g_date_time_get_minute (now) * G_PI / 30;
  double seconds = g_date_time_get_seconds (now) * G_PI / 30;
  float width = clutter_actor_get_width (actor);
  float height = clutter_actor_get_height (actor);

  g_print ("Time: %02d:%02d:%02.1f\r",
           g_date_time_get_hour (now),
           g_date_time_get_minute (now),
           g_date_time_get_seconds (now));

  g_date_time_unref (now);

  /* clear the current contents */
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

  /* who doesn't want all those new line settings? :-) */
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width (cr, 0.1);

  /* normalize and translate to the center of the rendering context */
  cairo_scale (cr, width, height);
  cairo_translate (cr, 0.5, 0.5);

  /* clock outline */
  cairo_set_source_rgba (cr, 0, 0, 0, 1.0);
  cairo_arc (cr, 0, 0, 0.4, 0, G_PI * 2);
  cairo_stroke (cr);

  /* draw a white dot on the current second */
  cairo_set_source_rgba (cr, 1, 1, 1, 0.6);
  cairo_arc (cr, sin (seconds) * 0.4, -cos (seconds) * 0.4, 0.05, 0, G_PI * 2);
  cairo_fill (cr);

  /* minutes hand */
  cairo_set_source_rgba (cr, 0.2, 0.2, 1, 0.6);
  cairo_move_to (cr, 0, 0);
  cairo_line_to (cr, sin (minutes) * 0.4, -cos (minutes) * 0.4);
  cairo_stroke (cr);

  /* hours hand */
  cairo_move_to (cr, 0, 0);
  cairo_line_to (cr, sin (hours) * 0.2, -cos (hours) * 0.2);
  cairo_stroke (cr);

  return TRUE;
}

static gboolean
on_timeout (gpointer data)
{
  ClutterContent *content = data;

  /* invalidate the content on timeout */
  clutter_content_invalidate (content);

  return TRUE;
}

G_MODULE_EXPORT int
test_drawing_area_main (int argc, char *argv[])
{
  ClutterActor *stage, *actor;
  ClutterContent *area;

  if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    return EXIT_FAILURE;

  stage = clutter_stage_new ();
  clutter_stage_set_title (CLUTTER_STAGE (stage), "2D Drawing");
  clutter_stage_set_color (CLUTTER_STAGE (stage), CLUTTER_COLOR_Aluminium5);
  clutter_stage_set_user_resizable (CLUTTER_STAGE (stage), TRUE);
  g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);
  clutter_actor_show (stage);

  area = clutter_drawing_area_new ();
  g_signal_connect (area, "draw", G_CALLBACK (on_area_draw), NULL);

  actor = clutter_actor_new ();
  clutter_actor_set_content (actor, area);
  clutter_actor_add_constraint (actor, clutter_bind_constraint_new (stage, CLUTTER_BIND_POSITION, PADDING));
  clutter_actor_add_constraint (actor, clutter_bind_constraint_new (stage, CLUTTER_BIND_SIZE, -2.0 * PADDING));
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), actor);

  clutter_threads_add_timeout (1000, on_timeout, area);

  clutter_main ();

  return EXIT_SUCCESS;
}
