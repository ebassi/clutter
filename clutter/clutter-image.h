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

#ifndef __CLUTTER_IMAGE_H__
#define __CLUTTER_IMAGE_H__

#include <gio/gio.h>
#include <cogl/cogl.h>
#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_IMAGE              (clutter_image_get_type ())
#define CLUTTER_IMAGE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_IMAGE, ClutterImage))
#define CLUTTER_IS_IMAGE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_IMAGE))
#define CLUTTER_IMAGE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_IMAGE, ClutterImageClass))
#define CLUTTER_IS_IMAGE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_IMAGE))
#define CLUTTER_IMAGE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_IMAGE, ClutterImageClass))

/**
 * CLUTTER_IMAGE_ERROR:
 *
 * Error domain for #ClutterImage errors.
 *
 * Since: 1.6
 */
#define CLUTTER_IMAGE_ERROR             (clutter_image_error_quark ())

typedef struct _ClutterImage            ClutterImage;
typedef struct _ClutterImagePrivate     ClutterImagePrivate;
typedef struct _ClutterImageClass       ClutterImageClass;

/**
 * ClutterImageError:
 * @CLUTTER_IMAGE_ERROR_INVALID_DATA: Unable to load the image data
 *
 * Error enumeration for #ClutterImage
 *
 * Since: 1.6
 */
typedef enum {
  CLUTTER_IMAGE_ERROR_INVALID_DATA
} ClutterImageError;

/**
 * ClutterImage:
 *
 * The <structname>ClutterImage</structname> structure contains only private
 * data and should be accessed using the provided API.
 *
 * Since: 1.6
 */
struct _ClutterImage
{
  /*< private >*/
  GObject parent_instance;

  ClutterImagePrivate *priv;
};

/**
 * ClutterImageClass:
 * @size_changed: class handler for the #ClutterImage::size-changed signal
 * @image_changed: class handler for the #ClutterImage::image-changed
 *   signal
 * @create_material: virtual functions for creating the material to be
 *   painted
 *
 * The <structname>ClutterImageClass</structname> structure contains only
 * private data.
 *
 * Since: 1.6
 */
struct _ClutterImageClass
{
  /*< private >*/
  GObjectClass parent_class;

  /*< public >*/
  void (* size_changed) (ClutterImage *image,
                         gint          new_width,
                         gint          new_height);

  void (* image_changed) (ClutterImage *image);

  CoglMaterial *(* create_material) (ClutterImage *image);

  /*< private >*/
  void (* _clutter_image_padding1) (void);
  void (* _clutter_image_padding2) (void);
  void (* _clutter_image_padding3) (void);
  void (* _clutter_image_padding4) (void);
  void (* _clutter_image_padding5) (void);
  void (* _clutter_image_padding6) (void);
  void (* _clutter_image_padding7) (void);
  void (* _clutter_image_padding8) (void);
};

GQuark clutter_image_error_quark (void);
GType clutter_image_get_type (void) G_GNUC_CONST;

ClutterImage *  clutter_image_new               (void);

gboolean        clutter_image_load_from_data    (ClutterImage         *image,
                                                 const guchar         *data,
                                                 CoglPixelFormat       format,
                                                 gint                  width,
                                                 gint                  height,
                                                 gint                  rowstride,
                                                 GError              **error);

gboolean        clutter_image_load              (ClutterImage         *image,
                                                 GFile                *gfile,
                                                 GCancellable         *cancellable,
                                                 gint                 *width,
                                                 gint                 *height,
                                                 GError              **error);
void            clutter_image_load_async        (ClutterImage         *image,
                                                 GFile                *gfile,
                                                 GCancellable         *cancellable,
                                                 GAsyncReadyCallback   callback,
                                                 gpointer              user_data);
gboolean        clutter_image_load_finish       (ClutterImage         *image,
                                                 GAsyncResult         *res,
                                                 gint                 *width,
                                                 gint                 *height,
                                                 GError              **error);

void            clutter_image_get_size          (ClutterImage         *image,
                                                 gint                 *width,
                                                 gint                 *height);

CoglHandle      clutter_image_get_cogl_texture  (ClutterImage         *image);

G_END_DECLS

#endif /* __CLUTTER_IMAGE_H__ */
