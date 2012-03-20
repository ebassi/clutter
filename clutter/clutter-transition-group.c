/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2012 Intel Corporation
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
 * Author: Emmanuele Bassi <ebassi@linux.intel.com>
 */

/**
 * SECTION:clutter-transition-group
 * @Title: ClutterTransitionGroup
 * @Short_Description: Group transitions together
 *
 * The #ClutterTransitionGroup allows running multiple #ClutterTransition
 * instances concurrently.
 *
 * The transitions inside a group will run within the boundaries of the
 * group; for instance, if a transition has a duration of 10 seconds, and
 * the group that contains it has a duration of 5 seconds, only the first
 * 5 seconds of the transition will be played.
 *
 * #ClutterTransitionGroup is available since Clutter 1.12
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-transition-group.h"

#include "clutter-debug.h"
#include "clutter-private.h"

struct _ClutterTransitionGroupPrivate
{
  GPtrArray *transitions;
};

static GQuark quark_transition_group = 0;

G_DEFINE_TYPE (ClutterTransitionGroup, clutter_transition_group, CLUTTER_TYPE_TRANSITION)

static void
clutter_transition_group_new_frame (ClutterTimeline *timeline,
                                    gint             elapsed)
{
  ClutterTransitionGroupPrivate *priv;
  gint64 msecs;
  guint i;

  priv = CLUTTER_TRANSITION_GROUP (timeline)->priv;

  /* get the time elapsed since the last ::new-frame... */
  msecs = clutter_timeline_get_delta (timeline);

  for (i = 0; i < priv->transitions->len; i++)
    {
      ClutterTimeline *t = g_ptr_array_index (priv->transitions, i);

      /* ... and advance every timeline */
      _clutter_timeline_advance (t, msecs);
    }
}

static void
clutter_transition_group_finalize (GObject *gobject)
{
  ClutterTransitionGroupPrivate *priv;

  priv = CLUTTER_TRANSITION_GROUP (gobject)->priv;

  g_ptr_array_unref (priv->transitions);

  G_OBJECT_CLASS (clutter_transition_group_parent_class)->finalize (gobject);
}

static void
clutter_transition_group_class_init (ClutterTransitionGroupClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterTimelineClass *timeline_class = CLUTTER_TIMELINE_CLASS (klass);

  quark_transition_group = g_quark_from_static_string ("-clutter-transition-group");

  gobject_class->finalize = clutter_transition_group_finalize;

  timeline_class->new_frame = clutter_transition_group_new_frame;
}

static void
clutter_transition_group_init (ClutterTransitionGroup *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            CLUTTER_TYPE_TRANSITION_GROUP,
                                            ClutterTransitionGroupPrivate);

  self->priv->transitions = g_ptr_array_new ();
  g_ptr_array_set_free_func (self->priv->transitions, g_object_unref);
}

/**
 * clutter_transition_group_new:
 *
 * Creates a new #ClutterTransitionGroup instance.
 *
 * Return value: the newly created #ClutterTransitionGroup. Use
 *   g_object_unref() when done to deallocate the resources it
 *   uses
 *
 * Since: 1.12
 */
ClutterTransition *
clutter_transition_group_new (void)
{
  return g_object_new (CLUTTER_TYPE_TRANSITION_GROUP, NULL);
}

/**
 * clutter_transition_group_add_transition:
 * @group: a #ClutterTransitionGroup
 * @transition: a #ClutterTransition
 *
 * Adds @transition to @group.
 *
 * This function acquires a reference on @transition that will be released
 * when calling clutter_transition_group_remove_transition().
 *
 * Since: 1.12
 */
void
clutter_transition_group_add_transition (ClutterTransitionGroup *group,
                                         ClutterTransition      *transition)
{
  gpointer old_group;

  g_return_if_fail (CLUTTER_IS_TRANSITION_GROUP (group));
  g_return_if_fail (CLUTTER_IS_TRANSITION (transition));

  old_group = g_object_get_qdata (G_OBJECT (transition), quark_transition_group);
  if (old_group != NULL)
    {
      g_critical ("The transition of type '%s' already is inside "
                  "a transition group.",
                  G_OBJECT_TYPE_NAME (transition));
      return;
    }

  g_ptr_array_add (group->priv->transitions, g_object_ref (transition));

  g_object_set_qdata (G_OBJECT (transition),
                      quark_transition_group,
                      group);
}

/**
 * clutter_transition_group_remove_transition:
 * @group: a #ClutterTransitionGroup
 * @transition: a #ClutterTransition
 *
 * Removes @transition from @group.
 *
 * This function releases the reference acquired on @transition when
 * calling clutter_transition_group_add_transition().
 *
 * Since: 1.12
 */
void
clutter_transition_group_remove_transition (ClutterTransitionGroup *group,
                                            ClutterTransition      *transition)
{
  gpointer old_group;

  g_return_if_fail (CLUTTER_IS_TRANSITION_GROUP (group));

  old_group = g_object_get_qdata (G_OBJECT (transition), quark_transition_group);
  if (old_group != group)
    {
      g_critical ("The transition of type '%s' does not match the "
                  "transition that is being removed from.",
                  G_OBJECT_TYPE_NAME (transition));
      return;
    }

  g_object_set_qdata (G_OBJECT (transition),
                      quark_transition_group,
                      group);

  g_ptr_array_remove (group->priv->transitions, transition);
}

/**
 * clutter_transition_group_remove_all:
 * @group: a #ClutterTransitionGroup
 *
 * Removes all transitions from @group.
 *
 * This function releases the reference acquired when calling
 * clutter_transition_group_add_transition().
 *
 * Since: 1.12
 */
void
clutter_transition_group_remove_all (ClutterTransitionGroup *group)
{
  g_return_if_fail (CLUTTER_IS_TRANSITION_GROUP (group));

  g_ptr_array_remove_range (group->priv->transitions,
                            0,
                            group->priv->transitions->len);
}
