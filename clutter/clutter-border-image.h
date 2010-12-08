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

#ifndef __CLUTTER_BORDER_IMAGE_H__
#define __CLUTTER_BORDER_IMAGE_H__

#include <clutter/clutter-image.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_BORDER_IMAGE               (clutter_border_image_get_type ())
#define CLUTTER_BORDER_IMAGE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_BORDER_IMAGE, ClutterBorderImage))
#define CLUTTER_IS_BORDER_IMAGE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_BORDER_IMAGE))
#define CLUTTER_BORDER_IMAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_BORDER_IMAGE, ClutterBorderImageClass))
#define CLUTTER_IS_BORDER_IMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_BORDER_IMAGE))
#define CLUTTER_BORDER_IMAGE_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_BORDER_IMAGE, ClutterBorderImageClass))

typedef struct _ClutterBorderImage              ClutterBorderImage;
typedef struct _ClutterBorderImagePrivate       ClutterBorderImagePrivate;
typedef struct _ClutterBorderImageClass         ClutterBorderImageClass;

/**
 * ClutterBorderImage:
 *
 * The <structname>ClutterBorderImage</structname> structure contains
 * private data and should only be accessed using the provided API.
 *
 * Since: 1.8
 */
struct _ClutterBorderImage
{
  /*< private >*/
  ClutterImage parent_instance;

  ClutterBorderImagePrivate *priv;
};

/**
 * ClutterBorderImage:
 *
 * The <structname>ClutterBorderImageClass</structname> structure contains
 * only private data.
 *
 * Since: 1.8
 */
struct _ClutterBorderImageClass
{
  /*< private >*/
  ClutterImageClass parent_class;

  void (* _clutter_border_image_0) (void);
  void (* _clutter_border_image_1) (void);
  void (* _clutter_border_image_2) (void);
  void (* _clutter_border_image_3) (void);
  void (* _clutter_border_image_4) (void);
  void (* _clutter_border_image_5) (void);
  void (* _clutter_border_image_6) (void);
  void (* _clutter_border_image_7) (void);
};

GType clutter_border_image_get_type (void) G_GNUC_CONST;

ClutterContent *        clutter_border_image_new                (void);

void                    clutter_border_image_set_slices         (ClutterBorderImage      *image,
                                                                 gdouble                  top,
                                                                 gdouble                  right,
                                                                 gdouble                  bottom,
                                                                 gdouble                  left);
void                    clutter_border_image_get_slices         (ClutterBorderImage      *image,
                                                                 gdouble                 *top,
                                                                 gdouble                 *right,
                                                                 gdouble                 *bottom,
                                                                 gdouble                 *left);

G_END_DECLS

#endif /* __CLUTTER_BORDER_IMAGE_H__ */
