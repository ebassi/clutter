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

#include "clutter-image-loader-null.h"

#include <cogl/cogl.h>
#include <gio/gio.h>

struct _ClutterImageLoaderNull
{
  ClutterImageLoader parent_instance;
};

#define clutter_image_loader_null_get_type    _clutter_image_loader_null_get_type
G_DEFINE_TYPE_WITH_CODE (ClutterImageLoaderNull,
                         clutter_image_loader_null,
                         CLUTTER_TYPE_IMAGE_LOADER,
                         g_io_extension_point_implement (CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME,
                                                         g_define_type_id,
                                                         "null",
                                                         0));

static void
clutter_image_loader_null_finalize (GObject *gobject)
{
  G_OBJECT_CLASS (clutter_image_loader_null_parent_class)->finalize (gobject);
}

static gboolean
clutter_image_loader_null_is_supported (void)
{
  return FALSE;
}

static void
clutter_image_loader_null_get_image_size (ClutterImageLoader *loader,
                                          gint               *width,
                                          gint               *height)
{
  if (width)
    *width = 0;

  if (height)
    *height = 0;
}

static CoglHandle
clutter_image_loader_null_get_texture_handle (ClutterImageLoader *loader)
{
  return COGL_INVALID_HANDLE;
}

static void
clutter_image_loader_null_class_init (ClutterImageLoaderNullClass *klass)
{
  ClutterImageLoaderClass *loader_class = CLUTTER_IMAGE_LOADER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = clutter_image_loader_null_finalize;

  loader_class->is_supported = clutter_image_loader_null_is_supported;
  loader_class->get_image_size = clutter_image_loader_null_get_image_size;
  loader_class->get_texture_handle = clutter_image_loader_null_get_texture_handle;
}

static void
clutter_image_loader_null_init (ClutterImageLoaderNull *loader)
{
}
