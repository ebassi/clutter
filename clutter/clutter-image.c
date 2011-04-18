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
 * SECTION:clutter-image
 * @Title: ClutterImage
 * @Short_Description: Image data content
 *
 * #ClutterImage is a class that deals with loading image data.
 *
 * #ClutterImage can load data directly from a binary blob or can use
 * a #GFile to synchronously and asynchronously load the image data
 * through GIO.
 *
 * #ClutterImage implements the #ClutterContent interface, and it
 * can be assigned to any actor using clutter_actor_set_content().
 *
 * #ClutterImage is available since Clutter 1.8
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cogl/cogl.h>

#include "clutter-image.h"

#include "clutter-content-private.h"
#include "clutter-debug.h"
#include "clutter-image-loader.h"
#include "clutter-marshal.h"
#include "clutter-private.h"

struct _ClutterImagePrivate
{
  gint image_width;
  gint image_height;

  CoglMaterial *material;
  CoglHandle texture;
};

enum
{
  PROP_0,

  PROP_WIDTH,
  PROP_HEIGHT,

  LAST_PROP
};

static GParamSpec *image_props[LAST_PROP] = { NULL, };

enum
{
  SIZE_CHANGED,
  IMAGE_CHANGED,

  LAST_SIGNAL
};

static guint image_signals[LAST_SIGNAL] = { 0, };

static CoglMaterial *image_template_material = NULL;

static void clutter_content_iface_init (ClutterContentIface *iface);

G_DEFINE_TYPE_WITH_CODE (ClutterImage, clutter_image, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTENT,
                                                clutter_content_iface_init));

static CoglMaterial *
copy_template_material (void)
{
  if (G_UNLIKELY (image_template_material == NULL))
    {
      CoglHandle dummy_tex;

      dummy_tex = cogl_texture_new_with_size (1, 1, COGL_TEXTURE_NO_SLICING,
                                              COGL_PIXEL_FORMAT_RGBA_8888_PRE);

      image_template_material = cogl_material_new ();
      cogl_material_set_layer (image_template_material, 0, dummy_tex);
      cogl_handle_unref (dummy_tex);
    }

  return cogl_material_copy (image_template_material);
}

static CoglMaterial *
_clutter_image_create_material (ClutterImage *image)
{
  return CLUTTER_IMAGE_GET_CLASS (image)->create_material (image);
}

static gboolean
clutter_image_setup_material (ClutterContent *content,
                              ClutterActor   *actor)
{
  ClutterImagePrivate *priv = CLUTTER_IMAGE (content)->priv;

  if (priv->material == NULL)
    return FALSE;

  cogl_set_source (priv->material);

  return TRUE;
}

static void
clutter_content_iface_init (ClutterContentIface *iface)
{
  iface->setup_material = clutter_image_setup_material;
}

static void
clutter_image_dispose (GObject *gobject)
{
  ClutterImagePrivate *priv = CLUTTER_IMAGE (gobject)->priv;

  if (priv->material != NULL)
    {
      cogl_object_unref (priv->material);
      priv->material = NULL;
      priv->texture = NULL;
    }

  G_OBJECT_CLASS (clutter_image_parent_class)->dispose (gobject);
}

static void
clutter_image_real_size_changed (ClutterImage *image,
                                 gint          width,
                                 gint          height)
{
  clutter_content_invalidate (CLUTTER_CONTENT (image));
}

static void
clutter_image_real_image_changed (ClutterImage *image)
{
  clutter_content_invalidate (CLUTTER_CONTENT (image));
}

static CoglMaterial *
clutter_image_real_create_material (ClutterImage *image)
{
  return copy_template_material ();
}

static void
clutter_image_get_property (GObject    *gobject,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  ClutterImagePrivate *priv = CLUTTER_IMAGE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_WIDTH:
      g_value_set_int (value, priv->image_width);
      break;

    case PROP_HEIGHT:
      g_value_set_int (value, priv->image_height);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
clutter_image_class_init (ClutterImageClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ClutterImagePrivate));

  /**
   * ClutterImage:width:
   *
   * The width of the image data, in pixels.
   *
   * Since: 1.8
   */
  image_props[PROP_WIDTH] =
    g_param_spec_int ("width",
                      P_("Width"),
                      P_("The width of the image, in pixels"),
                      0, G_MAXINT,
                      0,
                      CLUTTER_PARAM_READABLE);

  /**
   * ClutterImage:height:
   *
   * The height of the image data, in pixels.
   *
   * Since: 1.8
   */
  image_props[PROP_HEIGHT] =
    g_param_spec_int ("height",
                      P_("Height"),
                      P_("The height of the image, in pixels"),
                      0, G_MAXINT,
                      0,
                      CLUTTER_PARAM_READABLE);

  gobject_class->dispose = clutter_image_dispose;
  gobject_class->get_property = clutter_image_get_property;
  g_object_class_install_properties (gobject_class, LAST_PROP, image_props);

  gobject_class->dispose = clutter_image_dispose;

  /**
   * ClutterImage::size-changed:
   * @image: a #ClutterImage
   * @width: the width of the image data, in pixels
   * @height: the height of the image data, in pixels
   *
   * The ::size-changed signal is emitted each time the size of the
   * image data held by the #ClutterImage changes.
   *
   * The default implementation will automatically invalidate @image
   * and queue a redraw.
   *
   * Since: 1.8
   */
  image_signals[SIZE_CHANGED] =
    g_signal_new (I_("size-changed"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterImageClass, size_changed),
                  NULL, NULL,
                  _clutter_marshal_VOID__INT_INT,
                  G_TYPE_NONE, 2,
                  G_TYPE_INT,
                  G_TYPE_INT);

  /**
   * ClutterImage::image-changed:
   * @image: the #ClutterImage that emitted the signal
   *
   * The ::image-changed signal is emitted each time the image data
   * held by the #ClutterImage changes.
   *
   * The default implementation will automatically invalidate @image
   * and queue a redraw.
   *
   * Since: 1.8
   */
  image_signals[IMAGE_CHANGED] =
    g_signal_new (I_("image-changed"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterImageClass, image_changed),
                  NULL, NULL,
                  _clutter_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  klass->size_changed = clutter_image_real_size_changed;
  klass->image_changed = clutter_image_real_image_changed;
  klass->create_material = clutter_image_real_create_material;
}

static void
clutter_image_init (ClutterImage *image)
{
  image->priv = G_TYPE_INSTANCE_GET_PRIVATE (image, CLUTTER_TYPE_IMAGE,
                                             ClutterImagePrivate);
}

/**
 * clutter_image_new:
 *
 * Creates a new, empty #ClutterImage instance.
 *
 * Image data should be loaded using one of the clutter_image_load* family
 * of functions.
 *
 * Return value: (transfer full): the newly created #ClutterImage instance
 *
 * Since: 1.8
 */
ClutterImage *
clutter_image_new (void)
{
  return g_object_new (CLUTTER_TYPE_IMAGE, NULL);
}

/**
 * clutter_image_get_size:
 * @image: a #ClutterImage
 * @width: (out): return location for the image width, or %NULL
 * @height: (out): return location for the image height, or %NULL
 *
 * Retrieves the size of the image data held by @image.
 *
 * Since: 1.8
 */
void
clutter_image_get_size (ClutterImage *image,
                        gint         *width,
                        gint         *height)
{
  g_return_if_fail (CLUTTER_IS_IMAGE (image));

  if (width)
    *width = image->priv->image_width;

  if (height)
    *height = image->priv->image_height;
}

/**
 * clutter_image_load_from_data:
 * @image: a #ClutterImage
 * @data: image data
 * @format: the pixel format of @data
 * @width: the width of the image data, in pixels
 * @height: the height of the image data, in pixels
 * @rowstride: the distance between rows starting addresses
 * @error: return location for a #GError, or %NULL
 *
 * Sets the image data from an existing array.
 *
 * Return value: %TRUE if the image data was successfully loaded
 *   and %FALSE otherwise
 *
 * Since: 1.8
 */
gboolean
clutter_image_load_from_data (ClutterImage     *image,
                              const guchar     *data,
                              CoglPixelFormat   format,
                              gint              width,
                              gint              height,
                              gint              rowstride,
                              GError          **error)
{
  ClutterImagePrivate *priv;
  CoglHandle texture;
  gboolean width_changed, height_changed;

  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  priv = image->priv;

  texture = cogl_texture_new_from_data (width, height,
                                        COGL_TEXTURE_NONE,
                                        format,
                                        COGL_PIXEL_FORMAT_ANY,
                                        rowstride,
                                        data);
  if (texture == COGL_INVALID_HANDLE)
    {
      g_set_error (error, CLUTTER_IMAGE_ERROR,
                   CLUTTER_IMAGE_ERROR_INVALID_DATA,
                   _("Unable to load image data"));
      return FALSE;
    }

  if (priv->material == NULL)
    priv->material = _clutter_image_create_material (image);

  cogl_material_set_layer (priv->material, 0, texture);
  cogl_handle_unref (texture);

  if (priv->texture != COGL_INVALID_HANDLE)
    cogl_handle_unref (priv->texture);

  priv->texture = texture;

  g_object_freeze_notify (G_OBJECT (image));

  if (width != priv->image_width)
    {
      priv->image_width = width;
      g_object_notify_by_pspec (G_OBJECT (image), image_props[PROP_WIDTH]);
      width_changed = TRUE;
    }
  else
    width_changed = FALSE;

  if (height != priv->image_height)
    {
      priv->image_height = height;
      g_object_notify_by_pspec (G_OBJECT (image), image_props[PROP_HEIGHT]);
      height_changed = TRUE;
    }
  else
    height_changed = FALSE;

  g_signal_emit (image, image_signals[IMAGE_CHANGED], 0);

  if (width_changed || height_changed)
    g_signal_emit (image, image_signals[SIZE_CHANGED], 0,
                   priv->image_width,
                   priv->image_height);

  g_object_thaw_notify (G_OBJECT (image));

  return TRUE;
}

/**
 * clutter_image_load:
 * @image: a #ClutterImage
 * @gfile: a #GFile
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @width: (out): return location for the width of the image, or %NULL
 * @height: (out): return location for the height of the image, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * Synchronously loads image data available to the location pointed
 * by @gfile.
 *
 * This function will block the execution until the image data has
 * been loaded, or an error was encountered.
 *
 * It is possible to cancel the loading by using the @cancellable
 * instance.
 *
 * In case of error, the #GError structure will be filled accordingly.
 *
 * Return value: %TRUE if the loading was successful, and %FALSE otherwise
 *
 * Since: 1.8
 */
gboolean
clutter_image_load (ClutterImage  *image,
                    GFile         *gfile,
                    GCancellable  *cancellable,
                    gint          *width,
                    gint          *height,
                    GError       **error)
{
  ClutterImageLoader *loader;
  ClutterImagePrivate *priv;
  GFileInputStream *stream;
  CoglHandle texture;
  gboolean res;
  gint old_image_width, old_image_height;
  gboolean width_changed, height_changed;

  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (G_IS_FILE (gfile), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable),
                        FALSE);

  priv = image->priv;

  loader = _clutter_image_loader_new ();
  if (loader == NULL)
    return FALSE;

  stream = g_file_read (gfile, cancellable, error);
  if (stream == NULL)
    return FALSE;

  res = _clutter_image_loader_load_stream (loader,
                                           G_INPUT_STREAM (stream),
                                           -1, -1,
                                           CLUTTER_IMAGE_LOAD_NONE,
                                           cancellable,
                                           error);
  if (!res)
    {
      g_object_unref (stream);
      return FALSE;
    }

  old_image_width = priv->image_width;
  old_image_height = priv->image_height;
  _clutter_image_loader_get_image_size (loader,
                                        &priv->image_width,
                                        &priv->image_height);

  texture = _clutter_image_loader_get_texture_handle (loader);
  if (texture == COGL_INVALID_HANDLE)
    {
      g_object_unref (stream);
      return FALSE;
    }

  if (priv->material == NULL)
    priv->material = _clutter_image_create_material (image);

  cogl_material_set_layer (priv->material, 0, texture);

  if (priv->texture != COGL_INVALID_HANDLE)
    cogl_handle_unref (priv->texture);

  priv->texture = texture;

  g_object_freeze_notify (G_OBJECT (image));

  if (priv->image_width != old_image_width)
    {
      g_object_notify_by_pspec (G_OBJECT (image), image_props[PROP_WIDTH]);
      width_changed = TRUE;
    }
  else
    width_changed = FALSE;

  if (priv->image_height != old_image_height)
    {
      g_object_notify_by_pspec (G_OBJECT (image), image_props[PROP_HEIGHT]);
      height_changed = TRUE;
    }
  else
    height_changed = FALSE;

  g_signal_emit (image, image_signals[IMAGE_CHANGED], 0);

  if (width_changed || height_changed)
    g_signal_emit (image, image_signals[SIZE_CHANGED], 0,
                   priv->image_width,
                   priv->image_height);

  g_object_thaw_notify (G_OBJECT (image));

  if (width)
    *width = priv->image_width;

  if (height)
    *height = priv->image_height;

  g_object_unref (loader);
  g_object_unref (stream);

  return TRUE;
}

typedef struct {
  ClutterImageLoader *loader;
  ClutterImage *image;
  GAsyncReadyCallback callback;
  gpointer user_data;
  GInputStream *stream;
  GError *error;
  GCancellable *cancellable;
} AsyncReadClosure;

static void
async_read_closure_free (gpointer data)
{
  if (data != NULL)
    {
      AsyncReadClosure *closure = data;

      if (closure->loader != NULL)
        g_object_unref (closure->loader);

      if (closure->image != NULL)
        g_object_unref (closure->image);

      if (closure->stream != NULL)
        g_object_unref (closure->stream);

      if (closure->cancellable != NULL)
        g_object_unref (closure->cancellable);

      if (closure->error != NULL)
        g_error_free (closure->error);

      g_free (data);
    }
}

static void
async_load_complete (GObject      *gobject,
                     GAsyncResult *result,
                     gpointer      user_data)
{
  AsyncReadClosure *closure = user_data;
  GSimpleAsyncResult *res;
  GError *error = NULL;

  CLUTTER_NOTE (MISC, "Async loading finished");
  _clutter_image_loader_load_stream_finish (closure->loader,
                                            result,
                                            &error);
  if (error)
    closure->error = error;

  res = g_simple_async_result_new (G_OBJECT (closure->image),
                                   closure->callback,
                                   closure->user_data,
                                   clutter_image_load_async);

  g_simple_async_result_set_op_res_gpointer (res,
                                             closure,
                                             async_read_closure_free);

  g_simple_async_result_complete (res);
  g_object_unref (res);
}

static void
async_read_complete (GObject      *gobject,
                     GAsyncResult *result,
                     gpointer      user_data)
{
  AsyncReadClosure *closure = user_data;
  GFileInputStream *stream;
  GError *error = NULL;

  stream = g_file_read_finish (G_FILE (gobject), result, &error);
  if (stream == NULL)
    {
      GSimpleAsyncResult *res =
        g_simple_async_result_new (G_OBJECT (closure->image),
                                   closure->callback,
                                   closure->user_data,
                                   clutter_image_load_async);

      if (error)
        closure->error = error;

      g_simple_async_result_set_op_res_gpointer (res,
                                                 closure,
                                                 async_read_closure_free);
      g_simple_async_result_complete (res);
      g_object_unref (res);
      return;
    }

  if (g_cancellable_is_cancelled (closure->cancellable))
    {
      CLUTTER_NOTE (MISC, "Async image loading cancelled");
      async_read_closure_free (closure);
      return;
    }

  closure->stream = G_INPUT_STREAM (stream);
  g_object_ref (closure->stream);

  CLUTTER_NOTE (MISC, "Loading stream with '%s'",
                G_OBJECT_TYPE_NAME (closure->loader));
  _clutter_image_loader_load_stream_async (closure->loader,
                                           closure->stream,
                                           closure->cancellable,
                                           async_load_complete,
                                           closure);
}

/**
 * clutter_image_load_async:
 * @image: a #ClutterImage
 * @gfile: a #GFile
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @callback: (scope async): a callback function
 * @user_data: closure to pass to @callback
 *
 * Asynchronously loads image data from the resource pointed by @gfile
 * into @image.
 *
 * When the image data has been loaded the @callback function will be
 * called; it is the responsability of the developer to call
 * clutter_image_load_finish() inside @callback.
 *
 * Since: 1.8
 */
void
clutter_image_load_async (ClutterImage        *image,
                          GFile               *gfile,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  AsyncReadClosure *closure;
  ClutterImageLoader *loader;

  g_return_if_fail (CLUTTER_IS_IMAGE (image));
  g_return_if_fail (G_IS_FILE (gfile));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail (callback != NULL);

  loader = _clutter_image_loader_new ();
  if (loader == NULL)
    return;

  closure = g_new0 (AsyncReadClosure, 1);
  closure->loader = loader;
  closure->image = g_object_ref (image);
  closure->cancellable = (cancellable != NULL)
                       ? g_object_ref (cancellable)
                       : NULL;
  closure->callback = callback;
  closure->user_data = user_data;

  g_file_read_async (gfile, G_PRIORITY_DEFAULT, cancellable,
                     async_read_complete,
                     closure);
}

/**
 * clutter_image_load_finish:
 * @image: a #ClutterImage
 * @res: a #GAsyncResult
 * @width: (out): return location for the width of the image data, or %NULL
 * @height: (out): return location for the height of the image data, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * Terminates the asynchronous loading started by clutter_image_load_async()
 *
 * Return value: %TRUE if the image data was loaded successfully
 *   and %FALSE otherwise
 *
 * Since: 1.8
 */
gboolean
clutter_image_load_finish (ClutterImage  *image,
                           GAsyncResult  *res,
                           gint          *width,
                           gint          *height,
                           GError       **error)
{
  AsyncReadClosure *closure;
  GSimpleAsyncResult *simple;
  ClutterImagePrivate *priv;
  CoglHandle texture;
  gint old_image_width, old_image_height;
  gboolean width_changed, height_changed;

  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);

  simple = G_SIMPLE_ASYNC_RESULT (res);

  if (g_simple_async_result_propagate_error (simple, error))
    return FALSE;

  g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == clutter_image_load_async);

  closure = g_simple_async_result_get_op_res_gpointer (simple);
  if (closure->error != NULL)
    {
      if (width)
        *width = 0;

      if (height)
        *height = 0;

      g_propagate_error (error, closure->error);
      closure->error = NULL;

      return FALSE;
    }

  priv = image->priv;

  old_image_width = priv->image_width;
  old_image_height = priv->image_height;
  _clutter_image_loader_get_image_size (closure->loader,
                                        &priv->image_width,
                                        &priv->image_height);

  texture = _clutter_image_loader_get_texture_handle (closure->loader);
  if (texture == NULL)
    return FALSE;

  if (priv->material == NULL)
    priv->material = _clutter_image_create_material (image);

  cogl_material_set_layer (priv->material, 0, texture);

  if (priv->texture != COGL_INVALID_HANDLE)
    cogl_handle_unref (priv->texture);

  priv->texture = texture;

  g_object_freeze_notify (G_OBJECT (image));

  if (priv->image_width != old_image_width)
    {
      g_object_notify_by_pspec (G_OBJECT (image), image_props[PROP_WIDTH]);
      width_changed = TRUE;
    }
  else
    width_changed = FALSE;

  if (priv->image_height != old_image_height)
    {
      g_object_notify_by_pspec (G_OBJECT (image), image_props[PROP_HEIGHT]);
      height_changed = TRUE;
    }
  else
    height_changed = FALSE;

  g_signal_emit (image, image_signals[IMAGE_CHANGED], 0);

  if (width_changed || height_changed)
    g_signal_emit (image, image_signals[SIZE_CHANGED], 0,
                   priv->image_width,
                   priv->image_height);

  g_object_thaw_notify (G_OBJECT (image));

  if (width)
    *width = priv->image_width;

  if (height)
    *height = priv->image_height;

  return TRUE;
}

GQuark
clutter_image_error_quark (void)
{
  return g_quark_from_static_string ("clutter-image-error-quark");
}

/**
 * clutter_image_get_cogl_texture:
 * @image: a #ClutterImage
 *
 * Retrieves the pointer to the Cogl texture containing the data
 * loaded by @image.
 *
 * Return value: (transfer none): a pointer to a Cogl texture handle,
 *   or %NULL
 *
 * Since: 1.8
 */
CoglHandle
clutter_image_get_cogl_texture (ClutterImage *image)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), NULL);

  return image->priv->texture;
}
