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
  g_assert (width > 0 && height > 0);

  g_object_unref (gfile);
  g_object_unref (image);
}
