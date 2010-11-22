#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>

#define STAGE_WIDTH     400
#define STAGE_HEIGHT    400
#define PADDING         10
#define SPACING         2
#define RECT_SIZE       64

G_MODULE_EXPORT int
test_rgba_content_main (int   argc,
                        char *argv[])
{
  ClutterActor *stage, *group;
  ClutterContent *content;
  int i, j, n_cols, n_rows;
  float last_x, last_y;

  if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    return EXIT_FAILURE;

  stage = clutter_stage_new ();
  clutter_stage_set_title (CLUTTER_STAGE (stage), "RGBA Content");
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

  content = clutter_rgba_new (g_random_double_range (0.0, 1.0),
                              g_random_double_range (0.0, 1.0),
                              g_random_double_range (0.0, 1.0),
                              1.0);

  n_cols = (STAGE_WIDTH  - (2 * PADDING)) / (RECT_SIZE + (2 * SPACING));
  n_rows = (STAGE_HEIGHT - (2 * PADDING)) / (RECT_SIZE + (2 * SPACING));

  last_y = PADDING + SPACING;
  for (i = 0; i < n_rows; i++)
    {
      last_x = PADDING + SPACING;
      for (j = 0; j < n_cols; j++)
        {
          ClutterActor *rect = g_object_new (CLUTTER_TYPE_ACTOR, NULL);

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
