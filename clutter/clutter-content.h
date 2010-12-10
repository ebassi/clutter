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

#ifndef __CLUTTER_CONTENT_H__
#define __CLUTTER_CONTENT_H__

#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_CONTENT            (clutter_content_get_type ())
#define CLUTTER_CONTENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_CONTENT, ClutterContent))
#define CLUTTER_IS_CONTENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_CONTENT))
#define CLUTTER_CONTENT_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CLUTTER_TYPE_CONTENT, ClutterContentIface))

typedef struct _ClutterContentIface     ClutterContentIface;

/**
 * ClutterContentIface:
 * @setup_material: virtual function, used to set up the material used
 *   by the content
 * @update_geometry: virtual function, used to update the geometry of the
 *   content for the given actor
 * @paint_content: virtual function, used to paint the content for
 *   the given actor
 * @get_paint_volume: virtual function, used to update the paint volume
 *   for the given actor
 * @invalidate: virtual function, used to notify actors of a content
 *   update
 *
 * Interface for #ClutterActor contents.
 *
 * Since: 1.8
 */
struct _ClutterContentIface
{
  /*< private >*/
  GTypeInterface g_iface;

  /*< public >*/
  gboolean (* setup_material)   (ClutterContent     *content,
                                 ClutterActor       *actor);
  void     (* update_geometry)  (ClutterContent     *content,
                                 ClutterActor       *actor);
  void     (* paint_content)    (ClutterContent     *content,
                                 ClutterActor       *actor);

  gboolean (* get_paint_volume) (ClutterContent     *content,
                                 ClutterActor       *actor,
                                 ClutterPaintVolume *volume);

  void     (* invalidate)       (ClutterContent     *content);
};

GType clutter_content_get_type (void) G_GNUC_CONST;

gboolean        clutter_content_setup_material          (ClutterContent     *content,
                                                         ClutterActor       *actor);
void            clutter_content_update_geometry         (ClutterContent     *content,
                                                         ClutterActor       *actor);
void            clutter_content_paint_content           (ClutterContent     *content,
                                                         ClutterActor       *actor);

gboolean        clutter_content_get_paint_volume        (ClutterContent     *content,
                                                         ClutterActor       *actor,
                                                         ClutterPaintVolume *volume);

void            clutter_content_invalidate              (ClutterContent     *content);

G_END_DECLS

#endif /* __CLUTTER_CONTENT_H__ */
