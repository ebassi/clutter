/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2010  Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *   Emmanuele Bassi <ebassi@linux.intel.com>
 */

/**
 * SECTION:clutter-image-loader
 * @Title: ClutterImageLoader
 * @Short_Description: Image loading abstraction
 *
 * #ClutterImageLoader is a class implemented through GIO extension
 * points. It is an opaque class that delegates the logic for loading
 * image data synchronously and asynchronously to #ClutterImage.
 *
 * #ClutterImageLoader is available since Clutter 1.8
 */

#include "config.h"

#include "clutter-image-loader.h"

#include <string.h>
#include <gio/gio.h>

#include "clutter-private.h"

G_DEFINE_ABSTRACT_TYPE (ClutterImageLoader, clutter_image_loader, G_TYPE_OBJECT);

static void
clutter_image_loader_class_init (ClutterImageLoaderClass *klass)
{
}

static void
clutter_image_loader_init (ClutterImageLoader *loader)
{
}

static gpointer
get_default_image_loader (gpointer data)
{
  ClutterImageLoaderClass *chosen_class;
  ClutterImageLoaderClass **ret = data;
  GIOExtensionPoint *ep;
  GList *extensions, *l;

  _clutter_io_modules_ensure_loaded ();

  ep = g_io_extension_point_lookup (CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME);

  extensions = g_io_extension_point_get_extensions (ep);

  chosen_class = NULL;
  for (l = extensions; l != NULL; l = l->next)
    {
      GIOExtension *extension = l->data;
      ClutterImageLoaderClass *klass;

      klass = CLUTTER_IMAGE_LOADER_CLASS (g_io_extension_ref_class (extension));

      if (klass->is_supported ())
        {
          chosen_class = klass;
          break;
        }
      else
        g_type_class_unref (klass);
    }

  if (chosen_class)
    {
      *ret = chosen_class;
      return (gpointer) G_TYPE_FROM_CLASS (chosen_class);
    }
  else
    return (gpointer) G_TYPE_INVALID;
}

/*< private >
 * clutter_image_loader_new:
 *
 * Creates a new #ClutterImageLoader instance, proxying the
 * real instance created through GIO extension points.
 *
 * Return value: the newly created #ClutterImageLoader proxy
 *
 * Since: 1.8
 */
ClutterImageLoader *
_clutter_image_loader_new (void)
{
  static GOnce once_init = G_ONCE_INIT;
  GTypeClass *type_class = NULL;
  ClutterImageLoader *loader;
  GType type = G_TYPE_INVALID;

  g_once (&once_init, get_default_image_loader, &type_class);
  type = (GType) once_init.retval;
  if (type == G_TYPE_INVALID)
    {
      g_critical ("Unable to load the default image loader. Check the "
                  "compilation options of Clutter.");
      return NULL;
    }

  loader = g_object_new (type, NULL);

  if (type_class != NULL)
    g_type_class_unref (type_class);

  return loader;
}

/*< private >
 * clutter_image_loader_get_image_size:
 * @loader: a #ClutterImageLoader
 * @width: (out): return location for the image width, or %NULL
 * @height: (out): return location for the image height, or %NULL
 *
 * Queries the size of the loaded image.
 *
 * Since: 1.8
 */
void
_clutter_image_loader_get_image_size (ClutterImageLoader *loader,
                                      gint               *width,
                                      gint               *height)
{
  g_return_if_fail (CLUTTER_IS_IMAGE_LOADER (loader));

  CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->get_image_size (loader,
                                                           width,
                                                           height);
}

/*< private >
 * clutter_image_loader_get_texture_handle:
 * @loader: a #ClutterImageLoader
 *
 * Retrieves the Cogl handle for the texture containing the loaded image.
 *
 * Return value: (transfer none): a pointer to the texture handle.
 *   The #ClutterImageLoader owns the returned handle
 *
 * Since: 1.8
 */
CoglHandle
_clutter_image_loader_get_texture_handle (ClutterImageLoader *loader)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE_LOADER (loader), NULL);

  return CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->get_texture_handle (loader);
}

/*< private >
 * clutter_image_loader_load_stream:
 * @loader: a #ClutterImageLoader
 * @stream: a #GInputStream
 * @width: the width to use when loading the image, or -1 to use the
 *   image's own size; if @flags is %CLUTTER_IMAGE_LOAD_PRESERVE_ASPECT
 *   and width is -1 then the width of the image will be set to preserve
 *   the aspect ratio with the given @height
 * @height: the height to use when loading the image, or -1 to use the
 *   image's own size; if @flags is %CLUTTER_IMAGE_LOAD_PRESERVE_ASPECT
 *   and height is -1 then the height of the image will be set to
 *   preserve the aspect ratio with the given @width
 * @flags: flags to control the image loading
 * @cancellable: (allow-none): a #GCancellable or %NULL
 * @error: return location for a #GError or %NULL
 *
 * Synchronously loads image data from an input @stream.
 *
 * Return value: %TRUE if the image data was successfully loaded
 *
 * Since: 1.8
 */
gboolean
_clutter_image_loader_load_stream (ClutterImageLoader     *loader,
                                   GInputStream           *stream,
                                   gint                    width,
                                   gint                    height,
                                   ClutterImageLoadFlags   flags,
                                   GCancellable           *cancellable,
                                   GError                **error)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE_LOADER (loader), FALSE);
  g_return_val_if_fail (G_IS_INPUT_STREAM (stream), FALSE);
  g_return_val_if_fail (cancellable == NULL ||
                        G_IS_CANCELLABLE (cancellable), FALSE);


  return CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->load_stream (loader, stream,
                                                               width, height,
                                                               flags,
                                                               cancellable,
                                                               error);
}

/*< private >
 * clutter_image_loader_load_stream_async:
 * @loader: a #ClutterImageLoader
 * @stream: a #GInputStream
 * @cancellable: (allow-none): a #GCancellable or %NULL
 * @callback: (scope async): a callback
 * @user_data: closure for @callback
 *
 * Starts an asynchronous load of image data from an input @stream.
 *
 * When done, @callback will be invoked.
 *
 * Since: 1.8
 */
void
_clutter_image_loader_load_stream_async (ClutterImageLoader  *loader,
                                         GInputStream        *stream,
                                         GCancellable        *cancellable,
                                         GAsyncReadyCallback  callback,
                                         gpointer             user_data)
{
  g_return_if_fail (CLUTTER_IS_IMAGE_LOADER (loader));
  g_return_if_fail (G_IS_INPUT_STREAM (stream));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail (callback != NULL);

  CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->load_stream_async (loader, stream,
                                                              cancellable,
                                                              callback,
                                                              user_data);
}

/*< private >
 * clutter_image_loader_load_stream_finish:
 * @loader: a #ClutterImageLoader
 * @result: a #GAsyncResult
 * @error: return location for a #GError or %NULL
 *
 * Finishes the asynchronous loading started by
 * clutter_image_loader_load_stream_async(). This function must be
 * called in the callback function.
 *
 * Return value: %TRUE if the image loading was successful
 *   and %FALSE otherwise
 *
 * Since: 1.8
 */
gboolean
_clutter_image_loader_load_stream_finish (ClutterImageLoader  *loader,
                                          GAsyncResult        *result,
                                          GError             **error)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE_LOADER (loader), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  return CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->load_stream_finish (loader,
                                                                      result,
                                                                      error);
}
