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

#if !defined(__CLUTTER_H_INSIDE__) && !defined(CLUTTER_COMPILATION)
#error "Only <clutter/clutter.h> can be included directly."
#endif

#ifndef __CLUTTER_IMAGE_LOADER_H__
#define __CLUTTER_IMAGE_LOADER_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <cogl/cogl.h>
#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_IMAGE_LOADER               (clutter_image_loader_get_type ())
#define CLUTTER_IMAGE_LOADER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_IMAGE_LOADER, ClutterImageLoader))
#define CLUTTER_IS_IMAGE_LOADER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_IMAGE_LOADER))
#define CLUTTER_IMAGE_LOADER_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_IMAGE_LOADER, ClutterImageLoaderClass))
#define CLUTTER_IS_IMAGE_LOADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_IMAGE_LOADER))
#define CLUTTER_IMAGE_LOADER_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_IMAGE_LOADER, ClutterImageLoaderClass))

/**
 * CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME:
 *
 * Evaluates to the extension point name that can be used by modules
 * to provide the implementation of image loading logic.
 *
 * Since: 1.8
 */
#define CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME       "clutter-image-loader"

typedef struct _ClutterImageLoader              ClutterImageLoader;
typedef struct _ClutterImageLoaderClass         ClutterImageLoaderClass;

/**
 * ClutterImageLoader:
 *
 * The <structname>ClutterImageLoader</structname> structure contains
 * only private data and should be accessed using the provided API.
 *
 * Since: 1.8
 */
struct _ClutterImageLoader
{
  /*< private >*/
  GObject parent_instance;
};

/**
 * ClutterImageLoaderClass:
 *
 * The <structname>ClutterImageLoaderClass</structname> structure
 * contains only private data.
 *
 * Since: 1.8
 */
struct _ClutterImageLoaderClass
{
  /*< private >*/
  GObjectClass parent_class;

  gboolean   (* is_supported)       (void);

  gboolean   (* load_stream)        (ClutterImageLoader     *loader,
                                     GInputStream           *stream,
                                     gint                    width,
                                     gint                    height,
                                     ClutterImageLoadFlags   flags,
                                     GCancellable           *cancellable,
                                     GError                **error);

  void       (* load_stream_async)  (ClutterImageLoader     *loader,
                                     GInputStream           *stream,
                                     gint                    width,
                                     gint                    height,
                                     ClutterImageLoadFlags   flags,
                                     GCancellable           *cancellable,
                                     GAsyncReadyCallback     callback,
                                     gpointer                user_data);
  gboolean   (* load_stream_finish) (ClutterImageLoader     *loader,
                                     GAsyncResult           *result,
                                     GError                **error);

  void       (* get_image_size)     (ClutterImageLoader     *loader,
                                     gint                   *width,
                                     gint                   *height);

  CoglHandle (* get_texture_handle) (ClutterImageLoader     *loader);

  void (* _clutter_image_loader__1) (void);
  void (* _clutter_image_loader__2) (void);
  void (* _clutter_image_loader__3) (void);
  void (* _clutter_image_loader__4) (void);
  void (* _clutter_image_loader__5) (void);
  void (* _clutter_image_loader__6) (void);
  void (* _clutter_image_loader__7) (void);
  void (* _clutter_image_loader__8) (void);
  void (* _clutter_image_loader__9) (void);
  void (* _clutter_image_loader_10) (void);
  void (* _clutter_image_loader_11) (void);
  void (* _clutter_image_loader_12) (void);
  void (* _clutter_image_loader_13) (void);
};

GType clutter_image_loader_get_type (void) G_GNUC_CONST;

/*
 * private
 */

ClutterImageLoader *    _clutter_image_loader_new                       (void);

void                    _clutter_image_loader_get_image_size            (ClutterImageLoader     *loader,
                                                                         gint                   *width,
                                                                         gint                   *height);
CoglHandle              _clutter_image_loader_get_texture_handle        (ClutterImageLoader     *loader);

gboolean                _clutter_image_loader_load_stream               (ClutterImageLoader     *loader,
                                                                         GInputStream           *stream,
                                                                         gint                    width,
                                                                         gint                    height,
                                                                         ClutterImageLoadFlags   flags,
                                                                         GCancellable           *cancellable,
                                                                         GError                **error);

void                    _clutter_image_loader_load_stream_async         (ClutterImageLoader     *loader,
                                                                         GInputStream           *stream,
                                                                         gint                    width,
                                                                         gint                    height,
                                                                         ClutterImageLoadFlags   flags,
                                                                         GCancellable           *cancellable,
                                                                         GAsyncReadyCallback     callback,
                                                                         gpointer                user_data);
gboolean                _clutter_image_loader_load_stream_finish        (ClutterImageLoader     *loader,
                                                                         GAsyncResult           *result,
                                                                         GError                **error);

G_END_DECLS

#endif /* __CLUTTER_IMAGE_LOADER_H__ */
