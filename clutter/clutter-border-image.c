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

/**
 * SECTION:clutter-border-image
 * @Title: ClutterBorderImage
 * @Short_Description: A border image content
 *
 * #ClutterBorderImage is a #ClutterImage sub-class that is suitable for
 * displaying backgrounds and borders that fit a given area.
 *
 * #ClutterBorderImage paints the image data using nine slices, defined
 * by the area it has to fit and by four inward offsets from the edges
 * of the image, expressed in floating point values between 0.0 and 1.0.
 *
 * #ClutterBorderImage is available since Clutter 1.8
 */

#include "config.h"

#include <math.h>

#include "clutter-border-image.h"

#include "clutter-content.h"
#include "clutter-private.h"

enum
{
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,

  N_SLICES
};

struct _ClutterBorderImagePrivate
{
  gdouble slices[N_SLICES];
};

enum
{
  PROP_0,

  PROP_SLICE_TOP,
  PROP_SLICE_RIGHT,
  PROP_SLICE_BOTTOM,
  PROP_SLICE_LEFT,

  LAST_PROP
};

static GParamSpec *border_props[LAST_PROP] = { NULL, };

static CoglMaterial *border_template_material = NULL;

static void clutter_content_iface_init (ClutterContentIface *iface);

G_DEFINE_TYPE_WITH_CODE (ClutterBorderImage,
                         clutter_border_image,
                         CLUTTER_TYPE_IMAGE,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTENT,
                                                clutter_content_iface_init));

static void
clutter_border_image_update_geometry (ClutterContent *content,
                                      ClutterActor   *actor)
{
  ClutterBorderImagePrivate *priv = CLUTTER_BORDER_IMAGE (content)->priv;
  ClutterImage *image = CLUTTER_IMAGE (content);
  ClutterActorBox allocation;
  gfloat width, height;
  gint image_width, image_height;
  float tx1, ty1, tx2, ty2;
  float ex, ey;
  float top, bottom, right, left;

  clutter_image_get_size (image, &image_width, &image_height);
  if (image_width == 0 && image_height == 0)
    return;

  clutter_actor_get_allocation_box (actor, &allocation);
  clutter_actor_box_get_size (&allocation, &width, &height);

  /* round to pixel-aligned positions */
  top = ceilf (image_height * priv->slices[TOP]);
  bottom = floorf (image_height * (1.0 - priv->slices[BOTTOM]));
  left = ceilf (image_width * priv->slices[LEFT]);
  right = floorf (image_width * (1.0 - priv->slices[RIGHT]));

  tx1 = priv->slices[LEFT];
  tx2 = 1.0 - priv->slices[RIGHT];
  ty1 = priv->slices[TOP];
  ty2 = 1.0 - priv->slices[BOTTOM];

  ex = width - (image_width - right);
  if (ex < left)
    ex = left;

  ey = height - (image_height - bottom);
  if (ey < left)
    ey = left;

  {
    GLfloat rectangles[] = {
      /* top left corner */
      0, 0, left, top,
      0.0, 0.0,
      tx1, ty1,

      /* top middle */
      left, 0, ex, top,
      tx1, 0.0,
      tx2, ty1,

      /* top right */
      ex, 0, width, top,
      tx2, 0.0,
      1.0, ty1,

      /* mid left */
      0, top, left, ey,
      0.0, ty1,
      tx1, ty2,

      /* center */
      left, top, ex, ey,
      tx1, ty1,
      tx2, ty2,

      /* mid right */
      ex, top, width, ey,
      tx2, ty1,
      1.0, ty2,

      /* bottom left */
      0, ey, left, height,
      0.0, ty2,
      tx1, 1.0,

      /* bottom center */
      left, ey, ex, height,
      tx1, ty2,
      tx2, 1.0,

      /* bottom right */
      ex, ey, width, height,
      tx2, ty2,
      1.0, 1.0
    };

    cogl_rectangles_with_texture_coords (rectangles, 9);
  }
}

static void
clutter_content_iface_init (ClutterContentIface *iface)
{
  iface->update_geometry = clutter_border_image_update_geometry;
}

static CoglMaterial *
clutter_border_image_create_material (ClutterImage *image)
{
  if (border_template_material == NULL)
    {
      CoglHandle dummy;

      dummy = cogl_texture_new_with_size (1, 1,
                                          COGL_TEXTURE_NO_SLICING,
                                          COGL_PIXEL_FORMAT_RGBA_8888_PRE);

      border_template_material = cogl_material_new ();
      cogl_material_set_layer (border_template_material, 0, dummy);
      cogl_material_set_layer_filters (border_template_material, 0,
                                       COGL_MATERIAL_FILTER_NEAREST,
                                       COGL_MATERIAL_FILTER_NEAREST);
      cogl_handle_unref (dummy);
    }

  return cogl_material_copy (border_template_material);
}

static inline void
clutter_border_image_set_slice_internal (ClutterBorderImage *image,
                                         int                 position,
                                         gdouble             new_value,
                                         GParamSpec         *pspec)
{
  ClutterBorderImagePrivate *priv = image->priv;

  g_assert (position >= TOP && position < N_SLICES);

  if (priv->slices[position] != new_value)
    {
      priv->slices[position] = new_value;
      g_object_notify_by_pspec (G_OBJECT (image), pspec);
    }
}

static void
clutter_border_image_set_property (GObject      *gobject,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  ClutterBorderImage *image = CLUTTER_BORDER_IMAGE (gobject);

  switch (prop_id)
    {
    case PROP_SLICE_TOP:
      clutter_border_image_set_slice_internal (image, TOP,
                                               g_value_get_double (value),
                                               pspec);
      break;

    case PROP_SLICE_RIGHT:
      clutter_border_image_set_slice_internal (image, RIGHT,
                                               g_value_get_double (value),
                                               pspec);
      break;

    case PROP_SLICE_BOTTOM:
      clutter_border_image_set_slice_internal (image, BOTTOM,
                                               g_value_get_double (value),
                                               pspec);
      break;

    case PROP_SLICE_LEFT:
      clutter_border_image_set_slice_internal (image, LEFT,
                                               g_value_get_double (value),
                                               pspec);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
clutter_border_image_get_property (GObject    *gobject,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{  ClutterBorderImagePrivate *priv = CLUTTER_BORDER_IMAGE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_SLICE_TOP:
      g_value_set_double (value, priv->slices[TOP]);
      break;

    case PROP_SLICE_RIGHT:
      g_value_set_double (value, priv->slices[RIGHT]);
      break;

    case PROP_SLICE_BOTTOM:
      g_value_set_double (value, priv->slices[BOTTOM]);
      break;

    case PROP_SLICE_LEFT:
      g_value_set_double (value, priv->slices[LEFT]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
clutter_border_image_class_init (ClutterBorderImageClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterImageClass *image_class = CLUTTER_IMAGE_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ClutterBorderImagePrivate));

  /**
   * ClutterBorderImage:slice-top:
   *
   * The inward offset of the vertical slices from the top edge.
   *
   * Since: 1.8
   */
  border_props[PROP_SLICE_TOP] =
    g_param_spec_double ("slice-top",
                         P_("Slice Top"),
                         P_("The relative size of the top slice"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterBorderImage:slice-right:
   *
   * The inward offset of the horizontal slices from the right edge.
   *
   * Since: 1.8
   */
  border_props[PROP_SLICE_RIGHT] =
    g_param_spec_double ("slice-right",
                         P_("Slice Right"),
                         P_("The relative size of the right slice"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterBorderImage:slice-bottom:
   *
   * The inward offset of the vertical slices from the bottom edge.
   *
   * Since: 1.8
   */
  border_props[PROP_SLICE_BOTTOM] =
    g_param_spec_double ("slice-bottom",
                         P_("Slice Bottom"),
                         P_("The relative size of the bottom slice"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterBorderImage:slice-left:
   *
   * The inward offset of the horizontal slices from the left edge.
   *
   * Since: 1.8
   */
  border_props[PROP_SLICE_LEFT] =
    g_param_spec_double ("slice-left",
                         P_("Slice Left"),
                         P_("The relative size of the left slice"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  gobject_class->set_property = clutter_border_image_set_property;
  gobject_class->get_property = clutter_border_image_get_property;
  g_object_class_install_properties (gobject_class,
                                     LAST_PROP,
                                     border_props);

  image_class->create_material = clutter_border_image_create_material;
}

static void
clutter_border_image_init (ClutterBorderImage *image)
{
  image->priv = G_TYPE_INSTANCE_GET_PRIVATE (image, CLUTTER_TYPE_BORDER_IMAGE,
                                             ClutterBorderImagePrivate);

  memset (image->priv->slices, 0, sizeof (gdouble) * N_SLICES);
}

/**
 * clutter_border_image_new:
 *
 * Creates a new #ClutterBorderImage
 *
 * Return value: (transfer full): the newly created #ClutterBorderImage
 *
 * Since: 1.8
 */
ClutterContent *
clutter_border_image_new (void)
{
  return g_object_new (CLUTTER_TYPE_BORDER_IMAGE, NULL);
}

/**
 * clutter_border_image_set_slices:
 * @image: a #ClutterBorderImage
 * @top: the offset from the top, between 0.0 and 1.0
 * @right: the offset from the right, between 0.0 and 1.0
 * @bottom: the offset from the bottom, between 0.0 and 1.0
 * @left: the offset from the left, between 0.0 and 1.0
 *
 * Sets the inward offsets of @image. The offsets are relative
 * to the current size of the image data.
 *
 * Since: 1.8
 */
void
clutter_border_image_set_slices (ClutterBorderImage *image,
                                 gdouble             top,
                                 gdouble             right,
                                 gdouble             bottom,
                                 gdouble             left)
{
  g_return_if_fail (CLUTTER_IS_BORDER_IMAGE (image));

  g_object_freeze_notify (G_OBJECT (image));

  clutter_border_image_set_slice_internal (image, TOP,
                                           top,
                                           border_props[PROP_SLICE_TOP]);
  clutter_border_image_set_slice_internal (image, RIGHT,
                                           right,
                                           border_props[PROP_SLICE_RIGHT]);
  clutter_border_image_set_slice_internal (image, BOTTOM,
                                           bottom,
                                           border_props[PROP_SLICE_BOTTOM]);
  clutter_border_image_set_slice_internal (image, LEFT,
                                           left,
                                           border_props[PROP_SLICE_LEFT]);

  clutter_content_invalidate (CLUTTER_CONTENT (image));

  g_object_thaw_notify (G_OBJECT (image));
}

/**
 * clutter_border_image_get_slices:
 * @image: a #ClutterBorderImage
 * @top: (out): return location for the offset from the top, or %NULL
 * @right: (out): return location for the offset from the right, or %NULL
 * @bottom: (out): return location for the offset from the bottom, or %NULL
 * @left: (out): return location for the offset from the left, or %NULL
 *
 * Retrieves the offsets set using clutter_border_image_set_slices().
 *
 * Since: 1.8
 */
void
clutter_border_image_get_slices (ClutterBorderImage *image,
                                 gdouble            *top,
                                 gdouble            *right,
                                 gdouble            *bottom,
                                 gdouble            *left)
{
  g_return_if_fail (CLUTTER_IS_BORDER_IMAGE (image));

  if (top)
    *top = image->priv->slices[TOP];

  if (right)
    *right = image->priv->slices[RIGHT];

  if (bottom)
    *bottom = image->priv->slices[BOTTOM];

  if (left)
    *left = image->priv->slices[LEFT];
}
