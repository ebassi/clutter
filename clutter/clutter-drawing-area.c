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
 */

/**
 * SECTION:clutter-drawing-area
 * @Title: #ClutterDrawingArea
 * @Short_Desc: A content for 2D drawing
 *
 * #ClutterDrawingArea is an implementation of the #ClutterContent interface
 * aimed at 2D drawing using the Cairo API.
 *
 * A #ClutterDrawingArea instance should be used when a #ClutterActor should
 * display some content that has to be programmatically drawn, as opposed to
 * text or image data.
 *
 * Users of #ClutterDrawingArea should connect to the #ClutterDrawingArea::draw
 * signal. Signal handlers will be provided with the #ClutterActor currently
 * being painted, to access the scene graph attributes like size; and a
 * #cairo_t context, already set up to cover the allocation of the actor.
 *
 * #ClutterDrawingArea is available since Clutter 1.8
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

#include <cairo-gobject.h>

#include "clutter-drawing-area.h"

#include "clutter-debug.h"
#include "clutter-marshal.h"
#include "clutter-private.h"

struct _ClutterDrawingAreaPrivate
{
  CoglMaterial *material;
  CoglHandle texture;

  cairo_t *context;

  guint needs_repaint : 1;
  guint in_repaint    : 1;
};

enum
{
  CREATE_SURFACE,
  DRAW,

  LAST_SIGNAL
};

static guint area_signals[LAST_SIGNAL] = { 0, };
static ClutterContentIface *clutter_content_parent_iface = NULL;

static void clutter_content_iface_init (ClutterContentIface *iface);

G_DEFINE_TYPE_WITH_CODE (ClutterDrawingArea,
                         clutter_drawing_area,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTENT,
                                                clutter_content_iface_init));

static cairo_surface_t *
clutter_drawing_area_real_create_surface (ClutterDrawingArea *area,
                                          ClutterActor       *actor,
                                          guint               width,
                                          guint               height)
{
  return cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
}

static cairo_surface_t *
clutter_drawing_area_create_surface (ClutterDrawingArea *area,
                                     ClutterActor       *actor,
                                     guint               width,
                                     guint               height)
{
  cairo_surface_t *retval;

  g_signal_emit (area, area_signals[CREATE_SURFACE], 0,
                 actor,
                 width,
                 height,
                 &retval);

  return retval;
}

static gboolean
clutter_drawing_area_setup_material (ClutterContent *content,
                                     ClutterActor   *actor)
{
  ClutterDrawingArea *area = CLUTTER_DRAWING_AREA (content);
  ClutterDrawingAreaPrivate *priv = area->priv;
  ClutterActorBox box;
  gint width, height;

  /* avoid calling setup_material() from within draw() */
  if (priv->in_repaint)
    return FALSE;

  clutter_actor_get_allocation_box (actor, &box);

  /* round up to the nearest pixel */
  width = (gint) ceilf (clutter_actor_box_get_width (&box));
  height = (gint) ceilf (clutter_actor_box_get_height (&box));

  if (width == 0 || height == 0)
    return FALSE;

  if (priv->texture != NULL)
    {
      gint tex_width, tex_height;

      tex_width = cogl_texture_get_width (priv->texture);
      tex_height = cogl_texture_get_height (priv->texture);

      if (tex_width != width || tex_height != height)
        {
          cogl_object_unref (priv->texture);
          priv->texture = NULL;
        }
    }

  if (G_UNLIKELY (priv->material == NULL))
    priv->material = cogl_material_new ();

  if (priv->texture == NULL)
    {
      priv->texture = cogl_texture_new_with_size (width, height,
                                                  COGL_TEXTURE_NONE,
                                                  CLUTTER_CAIRO_PIXEL_FORMAT);
      priv->needs_repaint = TRUE;
    }

  if (priv->needs_repaint)
    {
      cairo_surface_t *surface;
      gboolean retval;

      surface = clutter_drawing_area_create_surface (area, actor,
                                                     width,
                                                     height);
      if (surface == NULL)
        return FALSE;

      priv->context = cairo_create (surface);
      priv->in_repaint = TRUE;
      priv->needs_repaint = FALSE;

      /* help out Cairo discard geometry that goes outside the
       * allocation, since our default paint volume matches the
       * allocation
       */
      cairo_rectangle (priv->context, 0, 0, width, height);
      cairo_clip (priv->context);

      g_signal_emit (area, area_signals[DRAW], 0,
                     actor,
                     priv->context,
                     &retval);

      cairo_destroy (priv->context);
      priv->context = NULL;
      priv->in_repaint = FALSE;

      if (cairo_surface_get_type (surface) == CAIRO_SURFACE_TYPE_IMAGE)
        {
          cogl_texture_set_region (priv->texture,
                                   0, 0, 0, 0,
                                   width, height,
                                   width, height,
                                   CLUTTER_CAIRO_PIXEL_FORMAT,
                                   cairo_image_surface_get_stride (surface),
                                   cairo_image_surface_get_data (surface));
        }

      cairo_surface_destroy (surface);
    }

  cogl_material_set_layer (priv->material, 0, priv->texture);

  if (priv->texture != NULL)
    {
      guint8 paint_opacity = clutter_actor_get_paint_opacity (actor);
      CoglColor *color = cogl_color_new ();

      cogl_color_init_from_4ub (color,
                                paint_opacity,
                                paint_opacity,
                                paint_opacity,
                                paint_opacity);

      cogl_material_set_color (priv->material, color);

      cogl_set_source (priv->material);

      cogl_color_free (color);
    }

  return TRUE;
}

static void
clutter_drawing_area_update_geometry (ClutterContent *content,
                                      ClutterActor   *actor)
{
  ClutterDrawingAreaPrivate *priv = CLUTTER_DRAWING_AREA (content)->priv;

  if (priv->texture != NULL)
    {
      ClutterActorBox box;
      gfloat width, height;

      clutter_actor_get_allocation_box (actor, &box);
      clutter_actor_box_get_size (&box, &width, &height);

      cogl_rectangle_with_texture_coords (0, 0, width, height,
                                          0.0f, 0.0f,
                                          1.0f, 1.0f);
    }
}

static void
clutter_drawing_area_invalidate (ClutterContent *content)
{
  /* invalidate() should recreate the contents */
  CLUTTER_DRAWING_AREA (content)->priv->needs_repaint = TRUE;

  /* chain up for the default implementation */
  clutter_content_parent_iface->invalidate (content);
}

static void
clutter_drawing_area_finalize (GObject *gobject)
{
  ClutterDrawingAreaPrivate *priv = CLUTTER_DRAWING_AREA (gobject)->priv;

  if (priv->texture)
    cogl_object_unref (priv->texture);

  if (priv->material)
    cogl_object_unref (priv->material);

  G_OBJECT_CLASS (clutter_drawing_area_parent_class)->finalize (gobject);
}

static void
clutter_content_iface_init (ClutterContentIface *iface)
{
  /* store a pointer to the default implementation, for chaining up */
  clutter_content_parent_iface =
    g_type_default_interface_peek (CLUTTER_TYPE_CONTENT);

  iface->setup_material = clutter_drawing_area_setup_material;
  iface->update_geometry = clutter_drawing_area_update_geometry;
  iface->invalidate = clutter_drawing_area_invalidate;
}

/* note: this won't save us from unbalanced save/restore
 * pairs in the handlers; but, then again, nothing will.
 */
static void
clutter_safe_draw_marshaller (GClosure     *closure,
                              GValue       *return_value,
                              guint         n_param_values,
                              const GValue *param_values,
                              gpointer      invocation_hint,
                              gpointer      marshal_data)
{
  cairo_t *cr = g_value_get_boxed (&param_values[2]);

  cairo_save (cr);

  _clutter_marshal_BOOLEAN__OBJECT_BOXED (closure,
                                          return_value,
                                          n_param_values,
                                          param_values,
                                          invocation_hint,
                                          marshal_data);

  cairo_restore (cr);
}

static void
clutter_drawing_area_class_init (ClutterDrawingAreaClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ClutterDrawingAreaPrivate));

  gobject_class->finalize = clutter_drawing_area_finalize;

  /**
   * ClutterDrawingArea::draw:
   * @area: the #ClutterDrawingArea that emitted the signal
   * @actor: the #ClutterActor currently being painted
   * @cr: the #cairo_t for the drawing operation
   *
   * The ::draw signal is emitted each time the @area should be
   * painted.
   *
   * The passed Cairo context is owned by the #ClutterDrawingArea
   * instance and should not be destroyed.
   *
   * The ::draw signal marshaller will take care of saving and
   * restoring the context for each handler, so it is not necessary
   * to call cairo_save() and cairo_restore() manually around the
   * drawing code.
   *
   * Return value: %FALSE if the signal emission should continue,
   *   or %TRUE if the signal emission should stop
   *
   * Since: 1.8
   */
  area_signals[DRAW] =
    g_signal_new (I_("draw"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterDrawingAreaClass, draw),
                  _clutter_boolean_handled_accumulator, NULL,
                  clutter_safe_draw_marshaller,
                  G_TYPE_BOOLEAN, 2,
                  CLUTTER_TYPE_ACTOR,
                  CAIRO_GOBJECT_TYPE_CONTEXT);

  /**
   * ClutterDrawingArea:create-surface:
   * @area: the #ClutterDrawingArea that emitted the signal
   * @actor: the #ClutterActor currently being painted
   * @width: the width of the surface
   * @height: the height of the surface
   *
   * The ::create-surface signal is emitted each the @area needs
   * to create a Cairo surface.
   *
   * The first signal handler returning a non-%NULL, valid
   * #cairo_surface_t will stop the signal emission.
   *
   * Return value: the newly created #cairo_surface_t
   *
   * Since: 1.8
   */
  area_signals[CREATE_SURFACE] =
    g_signal_new (I_("create-surface"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterDrawingAreaClass, create_surface),
                  _clutter_create_surface_accumulator, NULL,
                  _clutter_marshal_BOXED__OBJECT_UINT_UINT,
                  CAIRO_GOBJECT_TYPE_SURFACE, 3,
                  CLUTTER_TYPE_ACTOR,
                  G_TYPE_UINT,
                  G_TYPE_UINT);

  klass->create_surface = clutter_drawing_area_real_create_surface;
}

static void
clutter_drawing_area_init (ClutterDrawingArea *area)
{
  area->priv = G_TYPE_INSTANCE_GET_PRIVATE (area, CLUTTER_TYPE_DRAWING_AREA,
                                            ClutterDrawingAreaPrivate);

  area->priv->needs_repaint = TRUE;
}

/**
 * clutter_drawing_area_new:
 *
 * Creates a new #ClutterDrawingArea instance.
 *
 * Drawing on the returned #ClutterDrawingArea can be performed by
 * connecting to the #ClutterDrawingArea::draw signal.
 *
 * The returned #ClutterDrawingArea can be assigned to any #ClutterActor
 * using clutter_actor_set_content().
 *
 * Return value: (transfer full): the newly created #ClutterDrawingArea.
 *   Use g_object_unref() when done.
 *
 * Since: 1.8
 */
ClutterContent *
clutter_drawing_area_new (void)
{
  return g_object_new (CLUTTER_TYPE_DRAWING_AREA, NULL);
}
