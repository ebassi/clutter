/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2012  Intel Corporation.
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

#ifndef __CLUTTER_ACTOR_MODEL_H__
#define __CLUTTER_ACTOR_MODEL_H__

#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_ACTOR_MODEL                (clutter_actor_model_get_type ())
#define CLUTTER_ACTOR_MODEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_ACTOR_MODEL, ClutterActorModel))
#define CLUTTER_IS_ACTOR_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_ACTOR_MODEL))
#define CLUTTER_ACTOR_MODEL_GET_IFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CLUTTER_TYPE_ACTOR_MODEL, ClutterActorModelIface))

typedef struct _ClutterActorModelIface          ClutterActorModelIface;

struct _ClutterActorModelIface
{
  /*< private >*/
  GTypeInterface g_iface;

  /*< public >*/
  void (* get_preferred_width)  (ClutterActorModel      *self,
                                 gfloat                  for_height,
                                 gfloat                 *minimum_width_p,
                                 gfloat                 *natural_width_p);
  void (* get_preferred_height) (ClutterActorModel      *self,
                                 gfloat                  for_width,
                                 gfloat                 *minimum_height_p,
                                 gfloat                 *natural_height_p);
  void (* allocate)             (ClutterActorModel      *model,
                                 const ClutterActorBox  *allocation,
                                 ClutterAllocationFlags  flags);

  void (* paint_node)           (ClutterActorModel      *model,
                                 ClutterPaintNode       *node);

  void (* apply_transform)      (ClutterActorModel      *model,
                                 CoglMatrix             *ctm);

  void (* attached)             (ClutterActorModel      *model,
                                 ClutterActor           *actor);
  void (* detached)             (ClutterActorModel      *model,
                                 ClutterActor           *actor);
};

GType clutter_actor_model_get_type (void) G_GNUC_CONST;

void            clutter_actor_model_invalidate_content          (ClutterActorModel *model);
void            clutter_actor_model_invalidate_layout           (ClutterActorModel *model);

G_END_DECLS

#endif /* __CLUTTER_ACTOR_MODEL_H__ */
