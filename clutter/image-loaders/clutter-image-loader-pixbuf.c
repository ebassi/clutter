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

#include "config.h"

#include "clutter-image-loader-pixbuf.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>
#include <cogl/cogl.h>

#include "clutter-debug.h"
#include "clutter-image.h"
#include "clutter-private.h"

struct _ClutterImageLoaderPixbuf
{
  ClutterImageLoader parent_instance;

  GdkPixbufLoader *loader;

  gint image_width;
  gint image_height;
  gint rowstride;

  CoglPixelFormat pixel_format;

  CoglHandle texture;
};

#define clutter_image_loader_pixbuf_get_type    _clutter_image_loader_pixbuf_get_type
G_DEFINE_TYPE_WITH_CODE (ClutterImageLoaderPixbuf,
                         clutter_image_loader_pixbuf,
                         CLUTTER_TYPE_IMAGE_LOADER,
                         g_io_extension_point_implement (CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME,
                                                         g_define_type_id,
                                                         "gdk-pixbuf",
                                                         20));

static void
clutter_image_loader_pixbuf_dispose (GObject *gobject)
{
  ClutterImageLoaderPixbuf *loader;

  loader = CLUTTER_IMAGE_LOADER_PIXBUF (gobject);

  if (loader->loader != NULL)
    {
      gdk_pixbuf_loader_close (loader->loader, NULL);
      g_object_unref (loader->loader);
      loader->loader = NULL;
    }

  if (loader->texture != COGL_INVALID_HANDLE)
    {
      cogl_handle_unref (loader->texture);
      loader->texture = COGL_INVALID_HANDLE;
    }

  G_OBJECT_CLASS (clutter_image_loader_pixbuf_parent_class)->dispose (gobject);
}

static gboolean
clutter_image_loader_pixbuf_is_supported (void)
{
  return TRUE;
}

static gboolean
clutter_image_loader_pixbuf_load_stream (ClutterImageLoader     *loader,
                                         GInputStream           *stream,
                                         gint                    width,
                                         gint                    height,
                                         ClutterImageLoadFlags   flags,
                                         GCancellable           *cancellable,
                                         GError                **error)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);
  gboolean preserve_aspect_ratio;
  GdkPixbuf *pixbuf;

  preserve_aspect_ratio = (flags & CLUTTER_IMAGE_LOAD_PRESERVE_ASPECT) != 0;

  pixbuf = gdk_pixbuf_new_from_stream_at_scale (stream,
                                                width, height,
                                                preserve_aspect_ratio,
                                                cancellable,
                                                error);
  if (pixbuf == NULL)
    return FALSE;

  clutter_threads_enter ();

  self->image_width = gdk_pixbuf_get_width (pixbuf);
  self->image_height = gdk_pixbuf_get_height (pixbuf);
  self->pixel_format = gdk_pixbuf_get_has_alpha (pixbuf)
                     ? COGL_PIXEL_FORMAT_RGBA_8888
                     : COGL_PIXEL_FORMAT_RGB_888;
  self->rowstride = gdk_pixbuf_get_rowstride (pixbuf);

  CLUTTER_NOTE (MISC, "Image: %d x %d (rowstride: %d, has-alpha: %s)",
                self->image_width,
                self->image_height,
                self->rowstride,
                gdk_pixbuf_get_has_alpha (pixbuf) ? "yes" : "no");

  self->texture = cogl_texture_new_from_data (self->image_width,
                                              self->image_height,
                                              COGL_TEXTURE_NONE,
                                              self->pixel_format,
                                              COGL_PIXEL_FORMAT_ANY,
                                              self->rowstride,
                                              gdk_pixbuf_get_pixels (pixbuf));

  g_object_unref (pixbuf);

  if (self->texture == COGL_INVALID_HANDLE &&
      (error != NULL && *error == NULL))
    g_set_error_literal (error, CLUTTER_IMAGE_ERROR,
                         CLUTTER_IMAGE_ERROR_INVALID_DATA,
                         _("Unable to load the image data"));

  clutter_threads_leave ();

  return (self->texture != COGL_INVALID_HANDLE);
}

#define GET_DATA_BLOCK_SIZE     (8192)

typedef struct {
  ClutterImageLoader *loader;
  GInputStream *stream;
  GCancellable *cancellable;
  GAsyncReadyCallback callback;
  gpointer user_data;
  GdkPixbufLoader *pixbuf_loader;
  GByteArray *content;
  gsize pos;
  GError *error;
  gint width;
  gint height;
  guint preserve_aspect_ratio : 1;
} AsyncLoadClosure;

static void
async_load_closure_free (gpointer data)
{
  if (data != NULL)
    {
      AsyncLoadClosure *closure = data;

      if (closure->stream != NULL)
        g_object_unref (closure->stream);

      if (closure->cancellable != NULL)
        g_object_unref (closure->cancellable);

      if (closure->pixbuf_loader != NULL)
        {
          gdk_pixbuf_loader_close (closure->pixbuf_loader, NULL);
          g_object_unref (closure->pixbuf_loader);
        }

      if (closure->error != NULL)
        g_error_free (closure->error);

      if (closure->content != NULL)
        g_byte_array_free (closure->content, TRUE);

      if (closure->loader != NULL)
        g_object_unref (closure->loader);

      g_free (closure);
    }
}

static void
on_loader_size_prepared (GdkPixbufLoader  *loader,
                         gint              width,
                         gint              height,
                         AsyncLoadClosure *closure)
{
  if (closure->preserve_aspect_ratio)
    {
      if (closure->width > 0 || closure->height > 0)
        {
          if (closure->width < 0)
            {
              width = width * (double) closure->height / (double) height;
              height = closure->height;
            }
          else if (closure->height < 0)
            {
              height = height * (double) closure->width / (double) width;
              width = closure->width;
            }
          else if (((double) height * closure->width) > ((double) width * closure->height))
            {
              width = 0.5
                    + (double) width * (double) closure->height / height;
              height = closure->height;
            }
          else
            {
              height = 0.5
                     + (double) height * (double) closure->width / width;
              width = closure->width;
            }
        }
    }
  else
    {
      if (closure->width > 0)
        width = closure->width;

      if (closure->height > 0)
        height = closure->height;
    }

  width = MAX (width, 1);
  height = MAX (height, 1);

  gdk_pixbuf_loader_set_size (loader, width, height);
}

static void
load_stream_data_read_callback (GObject      *gobject,
                                GAsyncResult *result,
                                gpointer      user_data)
{
  GInputStream *stream = G_INPUT_STREAM (gobject);
  AsyncLoadClosure *closure = user_data;
  GError *error = NULL;
  gssize read_size;

  read_size = g_input_stream_read_finish (stream, result, &error);
  if (read_size < 0)
    {
      if (error != NULL)
        closure->error = error;
      else
        {
          GSimpleAsyncResult *res;

          CLUTTER_NOTE (MISC, "Reached EOF");

          res = g_simple_async_result_new (G_OBJECT (closure->loader),
                                           closure->callback,
                                           closure->user_data,
                                           _clutter_image_loader_load_stream_async);
          g_simple_async_result_set_op_res_gpointer (res, closure,
                                                     async_load_closure_free);
          g_simple_async_result_complete_in_idle (res);
          g_object_unref (res);
        }
    }
  else if (read_size > 0)
    {
      closure->pos += read_size;

      CLUTTER_NOTE (MISC, "Read %d bytes", read_size);

      g_byte_array_set_size (closure->content,
                             closure->pos + GET_DATA_BLOCK_SIZE);

      g_input_stream_read_async (stream, closure->content->data + closure->pos,
                                 GET_DATA_BLOCK_SIZE,
                                 0,
                                 closure->cancellable,
                                 load_stream_data_read_callback,
                                 closure);
    }
  else
    {
      GSimpleAsyncResult *res;

      CLUTTER_NOTE (MISC, "Loading finished");

      res = g_simple_async_result_new (G_OBJECT (closure->loader),
                                       closure->callback,
                                       closure->user_data,
                                       _clutter_image_loader_load_stream_async);
      g_simple_async_result_set_op_res_gpointer (res, closure,
                                                 async_load_closure_free);
      g_simple_async_result_complete_in_idle (res);
      g_object_unref (res);
    }
}

static void
clutter_image_loader_pixbuf_load_stream_async (ClutterImageLoader    *loader,
                                               GInputStream          *stream,
                                               gint                   width,
                                               gint                   height,
                                               ClutterImageLoadFlags  flags,
                                               GCancellable          *cancellable,
                                               GAsyncReadyCallback    callback,
                                               gpointer               user_data)
{
  AsyncLoadClosure *closure;

  closure = g_new0 (AsyncLoadClosure, 1);
  closure->loader = g_object_ref (loader);
  closure->stream = g_object_ref (stream);
  closure->width = width;
  closure->height = height;
  closure->preserve_aspect_ratio =
    (flags & CLUTTER_IMAGE_LOAD_PRESERVE_ASPECT) != 0;
  closure->cancellable = cancellable != NULL
                       ? g_object_ref (cancellable)
                       : NULL;
  closure->callback = callback;
  closure->user_data = user_data;
  closure->pixbuf_loader = gdk_pixbuf_loader_new ();
  closure->content = g_byte_array_new ();
  closure->pos = 0;

  g_byte_array_set_size (closure->content, closure->pos + GET_DATA_BLOCK_SIZE);

  g_signal_connect (closure->pixbuf_loader, "size-prepared",
                    G_CALLBACK (on_loader_size_prepared),
                    closure);

  g_input_stream_read_async (stream, closure->content->data + closure->pos,
                             GET_DATA_BLOCK_SIZE, 0,
                             closure->cancellable,
                             load_stream_data_read_callback,
                             closure);
}

static gboolean
clutter_image_loader_pixbuf_load_stream_finish (ClutterImageLoader *loader,
                                                GAsyncResult       *result,
                                                GError            **error)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);
  GSimpleAsyncResult *simple;
  AsyncLoadClosure *data;
  GError *internal_error;
  GdkPixbuf *pixbuf;

  simple = G_SIMPLE_ASYNC_RESULT (result);

  if (g_simple_async_result_propagate_error (simple, error))
    return FALSE;

  g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == _clutter_image_loader_load_stream_async);

  data = g_simple_async_result_get_op_res_gpointer (simple);
  if (data->error != NULL)
    {
      g_propagate_error (error, data->error);
      data->error = NULL;
      return FALSE;
    }

  g_byte_array_set_size (data->content, data->pos + 1);
  data->content->data[data->pos] = 0;
  CLUTTER_NOTE (MISC, "Loaded %d bytes", data->content->len);

  internal_error = NULL;
  gdk_pixbuf_loader_write (data->pixbuf_loader,
                           (const guchar *) data->content->data,
                           data->content->len,
                           &internal_error);
  if (internal_error != NULL)
    {
      g_propagate_error (error, internal_error);
      return FALSE;
    }

  CLUTTER_NOTE (MISC, "Closing GdkPixbufLoader");
  gdk_pixbuf_loader_close (data->pixbuf_loader, &internal_error);
  if (internal_error != NULL)
    {
      g_propagate_error (error, internal_error);
      return FALSE;
    }

  CLUTTER_NOTE (MISC, "Reading GdkPixbuf");
  pixbuf = gdk_pixbuf_loader_get_pixbuf (data->pixbuf_loader);
  if (pixbuf == NULL)
    return FALSE;

  self->image_width = gdk_pixbuf_get_width (pixbuf);
  self->image_height = gdk_pixbuf_get_height (pixbuf);
  self->pixel_format = gdk_pixbuf_get_has_alpha (pixbuf)
                     ? COGL_PIXEL_FORMAT_RGBA_8888
                     : COGL_PIXEL_FORMAT_RGB_888;
  self->rowstride = gdk_pixbuf_get_rowstride (pixbuf);

  CLUTTER_NOTE (MISC, "Image: %d x %d (rowstride: %d, has-alpha: %s)",
                self->image_width,
                self->image_height,
                self->rowstride,
                gdk_pixbuf_get_has_alpha (pixbuf) ? "yes" : "no");

  self->texture = cogl_texture_new_from_data (self->image_width,
                                              self->image_height,
                                              COGL_TEXTURE_NONE,
                                              self->pixel_format,
                                              COGL_PIXEL_FORMAT_ANY,
                                              self->rowstride,
                                              gdk_pixbuf_get_pixels (pixbuf));

  if (self->texture == COGL_INVALID_HANDLE &&
      (error != NULL && *error == NULL))
    g_set_error_literal (error, CLUTTER_IMAGE_ERROR,
                         CLUTTER_IMAGE_ERROR_INVALID_DATA,
                         _("Unable to load the image data"));

  return (self->texture != COGL_INVALID_HANDLE);
}

static void
clutter_image_loader_pixbuf_get_image_size (ClutterImageLoader *loader,
                                            gint               *width,
                                            gint               *height)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);

  if (width)
    *width = self->image_width;

  if (height)
    *height = self->image_height;
}

static CoglHandle
clutter_image_loader_pixbuf_get_texture_handle (ClutterImageLoader *loader)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);

  return self->texture;
}

static void
clutter_image_loader_pixbuf_class_init (ClutterImageLoaderPixbufClass *klass)
{
  ClutterImageLoaderClass *loader_class = CLUTTER_IMAGE_LOADER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = clutter_image_loader_pixbuf_dispose;

  loader_class->is_supported = clutter_image_loader_pixbuf_is_supported;
  loader_class->load_stream = clutter_image_loader_pixbuf_load_stream;
  loader_class->load_stream_async = clutter_image_loader_pixbuf_load_stream_async;
  loader_class->load_stream_finish = clutter_image_loader_pixbuf_load_stream_finish;
  loader_class->get_image_size = clutter_image_loader_pixbuf_get_image_size;
  loader_class->get_texture_handle = clutter_image_loader_pixbuf_get_texture_handle;
}

static void
clutter_image_loader_pixbuf_init (ClutterImageLoaderPixbuf *loader)
{
}
