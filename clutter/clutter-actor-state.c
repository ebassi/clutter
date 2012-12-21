#include "config.h"

#include "clutter-actor-private.h"
#include "clutter-private.h"

static const ClutterTransformInfo default_transform_info = {
  0.0, { 0, },                  /* rotation-x XXX:2.0 - remove center */
  0.0, { 0, },                  /* rotation-y XXX:2.0 - remove center */
  0.0, { 0, },                  /* rotation-z XXX:2.0 - remove center */

  1.0, 1.0, 1.0, { 0, },        /* scale XXX:2.0 - remove center */

  { 0, },                       /* anchor XXX:2.0 - remove*/

  CLUTTER_VERTEX_INIT_ZERO,     /* translation */

  0.f,                          /* z-position */

  CLUTTER_POINT_INIT_ZERO,      /* pivot */
  0.f,                          /* pivot-z */

  CLUTTER_MATRIX_INIT_IDENTITY,
  FALSE,                        /* transform */

  CLUTTER_MATRIX_INIT_IDENTITY,
  FALSE,                        /* child-transform */
};

static const ClutterLayoutInfo default_layout_info = {
  CLUTTER_POINT_INIT_ZERO,      /* fixed-pos */

  { 0.f, 0.f, 0.f, 0.f },       /* margin */

  CLUTTER_ACTOR_ALIGN_FILL,     /* x-align */
  CLUTTER_ACTOR_ALIGN_FILL,     /* y-align */
  
  FALSE,                        /* x-expand */
  FALSE,                        /* y-expand */

  CLUTTER_SIZE_INIT_ZERO,       /* minimum size */
  CLUTTER_SIZE_INIT_ZERO,       /* natural size */
};

static const ClutterAnimationInfo default_animation_info = {
  NULL,         /* transitions */
  NULL,         /* states */
  NULL,         /* cur_state */
};

static void
clutter_animation_info_clear (gpointer data)
{
  if (data != NULL)
    {
      ClutterAnimationInfo *info = data;

      if (info->transitions != NULL)
        g_hash_table_unref (info->transitions);

      if (info->states != NULL)
        g_array_unref (info->states);
    }
}

ClutterActorState *
_clutter_actor_state_alloc (void)
{
  return g_new (ClutterActorState, 1);
}

ClutterActorState *
_clutter_actor_state_init (ClutterActorState *state,
                           ClutterActor      *actor)
{
  state->layout = default_layout_info;
  state->transform = default_transform_info;
  state->animation = default_animation_info;

  state->actor = actor;

  return state;
}

void
_clutter_actor_state_free (ClutterActorState *state)
{
  clutter_animation_info_clear (&state->animation);

  g_free (state);
}

/*< private >
 * _clutter_actor_get_layout_info:
 * @self: a #ClutterActor
 *
 * Retrieves a pointer to the ClutterLayoutInfo structure.
 *
 * If the actor does not have a ClutterLayoutInfo associated to it, one
 * will be created and initialized to the default values.
 *
 * This function should be used for setters.
 *
 * For getters, you should use _clutter_actor_get_layout_info_or_defaults()
 * instead.
 *
 * Return value: (transfer none): a pointer to the ClutterLayoutInfo structure
 */
ClutterLayoutInfo *
_clutter_actor_get_layout_info (ClutterActor *self)
{
  ClutterActorState *state;

  state = _clutter_actor_get_state (self);

  return &(state->layout);
}

/*< private >
 * _clutter_actor_peek_layout_info:
 * @self: a #ClutterActor
 *
 * Retrieves the ClutterLayoutInfo structure associated to an actor.
 *
 * This function should only be used for getters.
 *
 * Return value: a const pointer to the ClutterLayoutInfo structure
 */
const ClutterLayoutInfo *
_clutter_actor_peek_layout_info (ClutterActor *self)
{
  return _clutter_actor_get_layout_info (self);
}

/*< private >
 * _clutter_actor_get_transform_info:
 * @self: a #ClutterActor
 *
 * Retrieves a pointer to the ClutterTransformInfo structure.
 *
 * If the actor does not have a ClutterTransformInfo associated to it, one
 * will be created and initialized to the default values.
 *
 * This function should be used for setters.
 *
 * For getters, you should use _clutter_actor_get_transform_info_or_defaults()
 * instead.
 *
 * Return value: (transfer none): a pointer to the ClutterTransformInfo
 *   structure
 */
ClutterTransformInfo *
_clutter_actor_get_transform_info (ClutterActor *self)
{
  ClutterActorState *state;

  state = _clutter_actor_get_state (self);

  return &(state->transform);
}

/*< private >
 * _clutter_actor_peek_transform_info:
 * @self: a #ClutterActor
 *
 * Retrieves the ClutterTransformInfo structure associated to an actor.
 *
 * If the actor does not have a ClutterTransformInfo structure associated
 * to it, then the default structure will be returned.
 *
 * This function should only be used for getters.
 *
 * Return value: a const pointer to the ClutterTransformInfo structure
 */
const ClutterTransformInfo *
_clutter_actor_peek_transform_info (ClutterActor *self)
{
  return _clutter_actor_get_transform_info (self);
}

ClutterAnimationInfo *
_clutter_actor_get_animation_info (ClutterActor *self)
{
  ClutterActorState *state;

  state = _clutter_actor_get_state (self);

  return &(state->animation);
}

const ClutterAnimationInfo *
_clutter_actor_peek_animation_info (ClutterActor *self)
{
  return _clutter_actor_get_animation_info (self);
}
