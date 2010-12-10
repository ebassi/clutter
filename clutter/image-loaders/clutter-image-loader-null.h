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

#ifndef __CLUTTER_IMAGE_LOADER_NULL_H__
#define __CLUTTER_IMAGE_LOADER_NULL_H__

#include <clutter/clutter-image-loader.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_IMAGE_LOADER_NULL          (_clutter_image_loader_null_get_type ())
#define CLUTTER_IMAGE_LOADER_NULL(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_IMAGE_LOADER_NULL, ClutterImageLoaderNull))
#define CLUTTER_IS_IMAGE_LOADER_NULL(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_IMAGE_LOADER_NULL))

typedef struct _ClutterImageLoaderNull          ClutterImageLoaderNull;
typedef struct _ClutterImageLoaderClass         ClutterImageLoaderNullClass;

GType _clutter_image_loader_null_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __CLUTTER_IMAGE_LOADER_NULL_H__ */
