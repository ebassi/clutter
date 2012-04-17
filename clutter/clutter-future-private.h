/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2012 Intel Corp.
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
 */

#ifndef __CLUTTER_FUTURE_PRIVATE_H__
#define __CLUTTER_FUTURE_PRIVATE_H__

#include <clutter/clutter-types.h>

G_BEGIN_DECLS

typedef struct _ClutterFuture   ClutterFuture;

struct _ClutterFuture
{
  /* symbolic name of the feature, for debugging purposes */
  const char *name;

  /* Clutter versions encoded using CLUTTER_ENCODE_VERSION */
  guint optional_version;
  guint mandatory_version;

  guint is_enabled : 1;
};

G_GNUC_INTERNAL
const char *    clutter_future_get_name                 (ClutterFutureFeature feature);

G_GNUC_INTERNAL
guint           clutter_future_get_optional_version     (ClutterFutureFeature feature);

G_GNUC_INTERNAL
guint           clutter_future_get_mandatory_version    (ClutterFutureFeature feature);

G_GNUC_INTERNAL
gboolean        clutter_future_is_enabled               (ClutterFutureFeature feature);

G_GNUC_INTERNAL
void            clutter_future_enable                   (ClutterFutureFeature feature);

G_END_DECLS

#endif /* __CLUTTER_FUTURE_PRIVATE_H__ */
