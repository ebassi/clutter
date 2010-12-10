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

#ifndef __CLUTTER_RGBA_H__
#define __CLUTTER_RGBA_H__

#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_RGBA               (clutter_rgba_get_type ())
#define CLUTTER_RGBA(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_RGBA, ClutterRGBA))
#define CLUTTER_IS_RGBA(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_RGBA))

/**
 * ClutterRGBA:
 *
 * <structname>ClutterRGBA</structname> is an opaque structure whose
 * members cannot be directly accessed.
 *
 * Since: 1.6
 */
typedef struct _ClutterRGBA             ClutterRGBA;

GType clutter_rgba_get_type (void) G_GNUC_CONST;

ClutterContent *clutter_rgba_new                (gdouble             red,
                                                 gdouble             green,
                                                 gdouble             blue,
                                                 gdouble             alpha);
ClutterContent *clutter_rgba_new_from_color     (const ClutterColor *color);
ClutterContent *clutter_rgba_new_from_string    (const gchar        *string);

void            clutter_rgba_set_red            (ClutterRGBA        *self,
                                                 gdouble             red);
gdouble         clutter_rgba_get_red            (ClutterRGBA        *self);
void            clutter_rgba_set_green          (ClutterRGBA        *self,
                                                 gdouble             green);
gdouble         clutter_rgba_get_green          (ClutterRGBA        *self);
void            clutter_rgba_set_blue           (ClutterRGBA        *self,
                                                 gdouble             blue);
gdouble         clutter_rgba_get_blue           (ClutterRGBA        *self);
void            clutter_rgba_set_alpha          (ClutterRGBA        *self,
                                                 gdouble             alpha);
gdouble         clutter_rgba_get_alpha          (ClutterRGBA        *self);

void            clutter_rgba_set_color          (ClutterRGBA        *self,
                                                 const ClutterColor *color);
void            clutter_rgba_set_string         (ClutterRGBA        *self,
                                                 const gchar        *string);

G_END_DECLS

#endif /* __CLUTTER_RGBA_H__ */
