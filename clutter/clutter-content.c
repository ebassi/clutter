/**
 * SECTION:clutter-content
 * @Title: ClutterContent
 * @Short_Description: Retained paint objects
 *
 * #ClutterContent is an interface implemented by objects that provide a way
 * to paint a #ClutterActor.
 *
 * A #ClutterContent implementation should provide the geometry of the paint
 * mask, and the actual changes to the state of the pipeline, as represented
 * by a Cogl material.
 *
 * Each #ClutterActor can use a #ClutterContent implementation to paint
 * itself; multiple #ClutterActor<!-- -->s can share the same #ClutterContent
 * instance.
 *
 * #ClutterContent is available since Clutter 1.8.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-content-private.h"
#include "clutter-private.h"

#include "clutter-actor.h"

/**
 * ClutterContent:
 *
 * <structname>ClutterContent</structname> is an opaque structure whose
 * members cannot be directly accessed.
 *
 * Since: 1.8
 */

static GQuark quark_content_actor = 0;

/* for G_DEFINE_INTERFACE */
typedef ClutterContentIface     ClutterContentInterface;

G_DEFINE_INTERFACE (ClutterContent, clutter_content, G_TYPE_OBJECT);

static void
clutter_content_real_update_geometry (ClutterContent *content,
                                      ClutterActor   *actor)
{
  ClutterActorBox allocation;

  clutter_actor_get_allocation_box (actor, &allocation);

  cogl_rectangle (0, 0,
                  clutter_actor_box_get_width (&allocation),
                  clutter_actor_box_get_height (&allocation));
}

static gboolean
clutter_content_real_setup_material (ClutterContent *content,
                                     ClutterActor   *actor)
{
  guint8 paint_opacity;

  paint_opacity = clutter_actor_get_paint_opacity (actor);

  cogl_set_source_color4f (0.0, 0.0, 0.0, paint_opacity / 255.0);

  return TRUE;
}

static void
clutter_content_real_paint_content (ClutterContent *content,
                                    ClutterActor   *actor)
{
  if (!clutter_content_setup_material (content, actor))
    return;

  clutter_content_update_geometry (content, actor);
}

static gboolean
clutter_content_real_get_paint_volume (ClutterContent     *content,
                                       ClutterActor       *actor,
                                       ClutterPaintVolume *volume)
{
  /* we don't have to maintain backward compatibility, so we can
   * default all Content implementations to the actor's allocation
   * without breaking anything
   */
  clutter_paint_volume_set_from_allocation (volume, actor);

  return TRUE;
}

static void
clutter_content_real_invalidate (ClutterContent *content)
{
  GHashTable *actors = _clutter_content_get_actors (content);
  GHashTableIter iter;
  gpointer key, value;

  if (actors == NULL)
    return;

  key = value = NULL;
  g_hash_table_iter_init (&iter, actors);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      ClutterActor *actor = key;

      g_assert (actor != NULL);

      clutter_actor_queue_redraw (actor);
    }
}

static void
clutter_content_default_init (ClutterContentInterface *iface)
{
  quark_content_actor = g_quark_from_static_string ("clutter-content-actors");

  iface->setup_material = clutter_content_real_setup_material;
  iface->update_geometry = clutter_content_real_update_geometry;
  iface->paint_content = clutter_content_real_paint_content;
  iface->get_paint_volume = clutter_content_real_get_paint_volume;
  iface->invalidate = clutter_content_real_invalidate;
}

/**
 * clutter_content_setup_material:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 *
 * Sets up the material for the given @actor.
 *
 * #ClutterContent implementations that provide a specific material
 * as the source should override this virtual function in the
 * #ClutterContent interface.
 *
 * An example implementation that paints a solid red color is:
 *
 * |[
 * static gboolean
 * setup_material (ClutterContent *content,
 *                 ClutterActor   *actor)
 * {
 *   cogl_set_source_color_4f (1.0, 0.0, 0.0, 1.0);
 *
 *   return TRUE;
 * }
 * ]|
 *
 * <note>If the implementation returns %FALSE, the #ClutterContent default
 * implementation of the <function>paint_content</function> virtual function
 * will not call clutter_content_update_geometry().</note>
 *
 * Return value: %TRUE if the material was set up correctly, and
 *   the @content should be painted, and %FALSE otherwise.
 *
 * Since: 1.8
 */
gboolean
clutter_content_setup_material (ClutterContent *content,
                                ClutterActor   *actor)
{
  g_return_val_if_fail (CLUTTER_IS_CONTENT (content), FALSE);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);

  return CLUTTER_CONTENT_GET_IFACE (content)->setup_material (content, actor);
}

/**
 * clutter_content_update_geometry:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 *
 * Updates the geometry for the given @actor.
 *
 * #ClutterContent implementations that submit a geometry that is
 * not a rectangle filling the @actor's allocation should override
 * this virtual function in the #ClutterContent interface.
 *
 * An example implementation that submits a geometry in the shape
 * of a triangle is:
 *
 * |[
 * static void
 * update_geometry (ClutterContent *content,
 *                  ClutterActor   *actor)
 * {
 *   ClutterActorBox allocation;
 *   float width, height;
 *
 *   clutter_actor_get_allocation_box (actor, &amp;allocation);
 *   clutter_actor_box_get_size (actor, &amp;width, &amp;height);
 *
 *   if (width > 0 && height > 0)
 *     {
 *       cogl_path_new ();
 *
 *       cogl_path_move_to (0, height);
 *       cogl_path_line_to (width / 2, 0);
 *       cogl_path_line_to (width, height);
 *       cogl_path_close ();
 *
 *       /&ast; fill the path with the material set in the
 *        &ast; setup_material() virtual function
 *        &ast;/
 *       cogl_path_fill ();
 *     }
 * }
 * ]|
 *
 * <note>This function will also be called by #ClutterActor when picking,
 * to maintain the geometry used when painting.</note>
 *
 * Since: 1.8
 */
void
clutter_content_update_geometry (ClutterContent *content,
                                 ClutterActor   *actor)
{
  g_return_if_fail (CLUTTER_IS_CONTENT (content));
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  CLUTTER_CONTENT_GET_IFACE (content)->update_geometry (content, actor);
}

/**
 * clutter_content_paint_content:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 *
 * Updates the graphics pipeline for the given @actor.
 *
 * #ClutterContent implementation should seldomly override this
 * virtual function.
 *
 * The default implementation for #ClutterContent will call
 * clutter_content_setup_material() and, depending on the returned
 * value, clutter_content_update_geometry(). Any #ClutterContent
 * implementation might decide to override either of these virtual
 * functions, or override the <function>paint_content</function>
 * virtual function entirely.
 *
 * #ClutterActor will call clutter_content_paint_content() when
 * painting.
 *
 * Since: 1.8
 */
void
clutter_content_paint_content (ClutterContent *content,
                               ClutterActor   *actor)
{
  g_return_if_fail (CLUTTER_IS_CONTENT (content));
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  CLUTTER_CONTENT_GET_IFACE (content)->paint_content (content, actor);
}

/*< private >
 * clutter_content_pick_content:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 * @pick_color: the color used to pick @actor
 *
 * Updates the graphics pipeline to paint the given @actor in pick
 * mode, using the @content's geometry.
 *
 * This is an internal method called by the default pick implementation
 * inside #ClutterActor.
 *
 * Since: 1.8
 */
void
_clutter_content_pick_content (ClutterContent     *content,
                               ClutterActor       *actor,
                               const ClutterColor *pick_color)
{
  cogl_set_source_color4ub (pick_color->red,
                            pick_color->green,
                            pick_color->blue,
                            pick_color->alpha);

  clutter_content_update_geometry (content, actor);
}

/**
 * clutter_content_get_paint_volume:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 * @volume: a #ClutterPaintVolume
 *
 * Asks @content to modify a #ClutterActor's paint volume, in case
 * the #ClutterContent instance should paint outside the allocation
 * of @actor.
 *
 * Return value: %TRUE if the paint volume is valid, and %FALSE if
 *   it is invalid and should be discarded
 *
 * Since: 1.8
 */
gboolean
clutter_content_get_paint_volume (ClutterContent     *content,
                                  ClutterActor       *actor,
                                  ClutterPaintVolume *volume)
{
  g_return_val_if_fail (CLUTTER_IS_CONTENT (content), FALSE);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);
  g_return_val_if_fail (volume != NULL, FALSE);

  return CLUTTER_CONTENT_GET_IFACE (content)->get_paint_volume (content,
                                                                actor,
                                                                volume);
}

/*< private >
 * clutter_content_get_actors:
 * @content: a #ClutterContent
 *
 * Retrieves a pointer to the list of #ClutterActor<!-- -->s using
 * @content to paint themselves.
 *
 * Return value: a #GHashTable of #ClutterActor instances. The list is
 *   owned by the #ClutterContent and it should not be freed
 */
GHashTable *
_clutter_content_get_actors (ClutterContent *content)
{
  return g_object_get_qdata (G_OBJECT (content), quark_content_actor);
}

/*< private >
 * clutter_content_remove_actor:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 *
 * Removes @actor from the list of #ClutterActor<!-- -->s using @content
 * to paint themselves.
 */
void
_clutter_content_remove_actor (ClutterContent *content,
                               ClutterActor   *actor)
{
  GHashTable *actors = _clutter_content_get_actors (content);

  if (G_UNLIKELY (actors == NULL))
    return;

  g_hash_table_remove (actors, actor);
}

/*< private >
 * clutter_content_add_actor:
 * @content: a #ClutterContent
 * @actor: a #ClutterActor
 *
 * Adds @actor to the list of #ClutterActor<!-- -->s using @content
 * when painting themselves.
 */
void
_clutter_content_add_actor (ClutterContent *content,
                            ClutterActor   *actor)
{
  GHashTable *actors = _clutter_content_get_actors (content);

  if (G_UNLIKELY (actors == NULL))
    {
      actors = g_hash_table_new (NULL, NULL);
      g_object_set_qdata_full (G_OBJECT (content), quark_content_actor,
                               actors,
                               (GDestroyNotify) g_hash_table_destroy);
    }

  g_hash_table_insert (actors, actor, GUINT_TO_POINTER (1));
}

/**
 * clutter_content_invalidate:
 * @content: a #ClutterContent
 *
 * Invalidates a #ClutterContent and causes a redraw to be queued on
 * every actor using @content.
 *
 * This function should only be called by #ClutterContent implementations
 * that have parameters affecting the way they are painted.
 *
 * Since: 1.8
 */
void
clutter_content_invalidate (ClutterContent *content)
{
  g_return_if_fail (CLUTTER_IS_CONTENT (content));

  CLUTTER_CONTENT_GET_IFACE (content)->invalidate (content);
}
