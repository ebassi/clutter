#include <clutter/clutter.h>
#include "test-conform-common.h"

void
image_sync_loading (void)
{
  ClutterImage *image = clutter_image_new ();
  GError *error;
  GFile *gfile;
  gint width, height;
  gboolean res;

  g_assert (CLUTTER_IS_IMAGE (image));

  gfile = g_file_new_for_path (TESTS_DATADIR "/redhand.png");

  width = height = 0;
  error = NULL;

  res = clutter_image_load (image, gfile, NULL,
                            &width,
                            &height,
                            &error);

  if (g_test_verbose() && error != NULL)
    g_print ("Unexpected error: %s\n", error->message);

  g_assert (error == NULL);
  g_assert (res);
  g_assert_cmpint (width, >, 0);
  g_assert_cmpint (height, >, 0);

  g_object_unref (gfile);
  g_object_unref (image);
}

static void
async_load_done_cb (GObject      *gobject,
                    GAsyncResult *result,
                    gpointer      user_data)
{
  GMainLoop *main_loop = user_data;
  GError *error = NULL;
  gint width, height;
  gboolean res;

  width = height = 0;
  res = clutter_image_load_finish (CLUTTER_IMAGE (gobject), result,
                                   &width,
                                   &height,
                                   &error);

  if (g_test_verbose () && error != NULL)
    g_print ("Unexpected error: %s\n", error->message);

  g_assert (res);
  g_assert (error == NULL);
  g_assert_cmpint (width, >, 0);
  g_assert_cmpint (height, >, 0);

  g_main_loop_quit (main_loop);
}

void
image_async_loading (void)
{
  ClutterImage *image = clutter_image_new ();
  GMainLoop *main_loop;
  GFile *gfile;

  g_assert (CLUTTER_IS_IMAGE (image));

  gfile = g_file_new_for_path (TESTS_DATADIR "/redhand.png");

  main_loop = g_main_loop_new (NULL, FALSE);

  clutter_image_load_async (image, gfile, NULL,
                            async_load_done_cb,
                            main_loop);

  g_main_loop_run (main_loop);

  g_main_loop_unref (main_loop);
  g_object_unref (gfile);
  g_object_unref (image);
}
