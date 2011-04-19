#include <stdlib.h>
#include <gmodule.h>
#include <gio/gio.h>
#include <clutter/clutter.h>

#define STAGE_WIDTH     400
#define STAGE_HEIGHT    400
#define PADDING         10
#define SPACING         2
#define RECT_SIZE       64

static void
load_async_done (GObject      *gobject,
                 GAsyncResult *result,
                 gpointer      dummy G_GNUC_UNUSED)
{
  GError *error = NULL;
  gboolean res;

  res = clutter_image_load_finish (CLUTTER_IMAGE (gobject), result, &error);
  if (!res)
    {
      g_print ("Unable to load 'redhand.png': %s", error->message);
      g_error_free (error);
      return;
    }
}

G_MODULE_EXPORT int
test_image_content_main (int   argc,
                         char *argv[])
{
  ClutterActor *stage, *group;
  ClutterContent *content;
  int i, j, n_cols, n_rows;
  float last_x, last_y;
  GFile *gfile;

  if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    return EXIT_FAILURE;

  stage = clutter_stage_new ();
  clutter_stage_set_title (CLUTTER_STAGE (stage), "Image Content");
  clutter_stage_set_user_resizable (CLUTTER_STAGE (stage), TRUE);
  clutter_actor_set_size (stage, 400, 400);
  clutter_actor_show (stage);

  g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

  content = clutter_rgba_new (g_random_double_range (0.0, 1.0),
                              g_random_double_range (0.0, 1.0),
                              g_random_double_range (0.0, 1.0),
                              1.0);

  group = clutter_group_new ();
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), group);
  clutter_actor_add_constraint (group, clutter_align_constraint_new (stage, CLUTTER_ALIGN_X_AXIS, 0.5));
  clutter_actor_add_constraint (group, clutter_align_constraint_new (stage, CLUTTER_ALIGN_Y_AXIS, 0.5));
  clutter_actor_set_content (group, content);
  g_object_unref (content);

  content = CLUTTER_CONTENT (clutter_image_new ());
  gfile = g_file_new_for_path (TESTS_DATADIR "/redhand.png");
  clutter_image_load_async (CLUTTER_IMAGE (content), gfile, NULL,
                            load_async_done,
                            NULL);
  g_object_unref (gfile);

  n_cols = (STAGE_WIDTH  - (2 * PADDING)) / (RECT_SIZE + (2 * SPACING));
  n_rows = (STAGE_HEIGHT - (2 * PADDING)) / (RECT_SIZE + (2 * SPACING));

  last_y = PADDING + SPACING;
  for (i = 0; i < n_rows; i++)
    {
      last_x = PADDING + SPACING;
      for (j = 0; j < n_cols; j++)
        {
          ClutterActor *rect = clutter_actor_new ();

          clutter_actor_set_position (rect, last_x, last_y);
          clutter_actor_set_size (rect, RECT_SIZE, RECT_SIZE);
          clutter_actor_set_content (rect, content);

          clutter_container_add_actor (CLUTTER_CONTAINER (group), rect);

          last_x += RECT_SIZE + SPACING;
        }

      last_y += RECT_SIZE + SPACING;
    }

  clutter_actor_set_size (group, last_x + PADDING, last_y + PADDING);

  g_object_unref (content);

  clutter_main ();

  return EXIT_SUCCESS;
}
