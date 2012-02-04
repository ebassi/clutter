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

/**
 * SECTION:clutter-actor-model
 * @Title: ClutterActorModel
 * @Short_Description: A model for actors
 *
 * FIXME
 *
 * #ClutterActorModel is available since Clutter 1.10
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-actor-model.h"

typedef struct _ClutterActorModelIface  ClutterActorModelInterface;

G_DEFINE_INTERFACE_TYPE (ClutterActorModel, clutter_actor_model, G_TYPE_OBJECT)

static void
clutter_actor_model_default_init (ClutterActorModelIface *iface)
{
}

void
_clutter_actor_model_attached (ClutterActorModel *model,
                               ClutterActor      *actor)
{
  GHashTable *actors;
  GObject *obj;

  obj = G_OBJECT (model);

  actors = g_object_get_data (obj, "-clutter-actor-model-actors");
  if (actors == NULL)
    {
      actors = g_hash_table_new (NULL, NULL);

      g_object_set_data_full (obj, "-clutter-actor-model-actors",
                              actors,
                              (GDestroyNotify) g_hash_table_unref);
    }

  g_hash_table_insert (actors, actor, actor);
}

void
_clutter_actor_model_detached (ClutterActorModel *model,
                               ClutterActor      *actor)
{
  GHashTable *actors;
  GObject *obj;

  obj = G_OBJECT (model);

  actors = g_object_get_data (obj, "-clutter-actor-model-actors");
  g_assert (actors != NULL);

  g_hash_table_remove (actors, actor);

  if (g_hash_table_size (actors) == 0)
    g_object_set_data (obj, "-clutter-actor-model-actors", NULL);
}

static inline void
clutter_actor_model_foreach_actor (ClutterActorModel *model,
                                   ClutterCallback    func,
                                   gpointer           data)
{
  GHashTableIter iter;
  GHashTable *actors;

  actors = g_object_get_data (G_OBJECT (model), "-clutter-actor-model-actors");
  if (actors == NULL)
    return;

  g_hash_table_iter_init (&iter, actors);
  while (g_hash_table_iter_next (&iter, &key, NULL))
    {
      ClutterActor *actor = key;

      callback (actor, data);
    }
}

/**
 * clutter_actor_model_invalidate_content:
 * @model: a #ClutterActorModel
 *
 * Queues a redraw on every #ClutterActor using @model.
 *
 * This function should be called by #ClutterActorModel implementations
 * whenever the content of the model changes.
 *
 * Since: 1.10
 */
void
clutter_actor_model_invalidate_content (ClutterActorModel *model)
{
  g_return_if_fail (CLUTTER_IS_ACTOR_MODEL (model));
  
  clutter_actor_model_foreach_actor (model,
                                     CLUTTER_CALLBACK (clutter_actor_queue_redraw),
                                     NULL);
}

/**
 * clutter_actor_model_invalidate_layout:
 * @model: a #ClutterActorModel
 *
 * Queues a relayout on every #ClutterActor using @model.
 *
 * This function should be called by #ClutterActorModel implementations
 * whenever the size of the model changes.
 *
 * Since: 1.10
 */
void
clutter_actor_model_invalidate_layout (ClutterActorModel *model)
{
  g_return_if_fail (CLUTTER_IS_ACTOR_MODEL (model));

  clutter_actor_model_foreach_actor (model,
                                     CLUTTER_CALLBACK (clutter_actor_queue_relayout),
                                     NULL);
}
