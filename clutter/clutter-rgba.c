/**
 * SECTION:clutter-rgba
 * @Title: ClutterRGBA
 * @Short_Description: A solid color
 *
 * #ClutterRGBA is an object implementing the #ClutterContent interface. It
 * can be used to paint a solid color covering the same area as the allocation
 * of the #ClutterActor<!-- -->s that use it as their content.
 *
 * #ClutterRGBA is available since Clutter 1.6.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-rgba.h"

#include "clutter-actor.h"
#include "clutter-color.h"
#include "clutter-content.h"
#include "clutter-debug.h"
#include "clutter-private.h"

typedef struct _ClutterRGBAClass        ClutterRGBAClass;

struct _ClutterRGBA
{
  GObject parent_instance;

  gdouble red;
  gdouble green;
  gdouble blue;
  gdouble alpha;
};

struct _ClutterRGBAClass
{
  GObjectClass parent_class;
};

enum
{
  PROP_0,

  PROP_RED,
  PROP_GREEN,
  PROP_BLUE,
  PROP_ALPHA,

  PROP_COLOR,

  PROP_STRING,

  PROP_LAST
};

static void clutter_content_iface_init (ClutterContentIface *iface);

static GParamSpec *obj_props[PROP_LAST] = { NULL, };

static const ClutterColor default_color = { 0, 0, 0, 0 };

G_DEFINE_TYPE_WITH_CODE (ClutterRGBA, clutter_rgba, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTENT,
                                                clutter_content_iface_init));

static gboolean
clutter_rgba_setup_material (ClutterContent *content,
                             ClutterActor   *actor)
{
  ClutterRGBA *self = CLUTTER_RGBA (content);
  gfloat paint_opacity;

  paint_opacity = ((float) clutter_actor_get_paint_opacity (actor) / 255.0f)
                * self->alpha;

  cogl_set_source_color4f (self->red,
                           self->green,
                           self->blue,
                           CLAMP (paint_opacity, 0.0, 1.0));

  return TRUE;
}

static void
clutter_content_iface_init (ClutterContentIface *iface)
{
  iface->setup_material = clutter_rgba_setup_material;
}

static void
clutter_rgba_set_property (GObject      *gobject,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  ClutterRGBA *self = CLUTTER_RGBA (gobject);

  switch (prop_id)
    {
    case PROP_RED:
      clutter_rgba_set_red (self, g_value_get_double (value));
      break;

    case PROP_GREEN:
      clutter_rgba_set_green (self, g_value_get_double (value));
      break;

    case PROP_BLUE:
      clutter_rgba_set_blue (self, g_value_get_double (value));
      break;

    case PROP_ALPHA:
      clutter_rgba_set_alpha (self, g_value_get_double (value));
      break;

    case PROP_COLOR:
      clutter_rgba_set_color (self, clutter_value_get_color (value));
      break;

    case PROP_STRING:
      clutter_rgba_set_string (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
clutter_rgba_get_property (GObject    *gobject,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  ClutterRGBA *self = CLUTTER_RGBA (gobject);

  switch (prop_id)
    {
    case PROP_RED:
      g_value_set_double (value, self->red);
      break;

    case PROP_GREEN:
      g_value_set_double (value, self->green);
      break;

    case PROP_BLUE:
      g_value_set_double (value, self->blue);
      break;

    case PROP_ALPHA:
      g_value_set_double (value, self->alpha);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
clutter_rgba_class_init (ClutterRGBAClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /**
   * ClutterRGBA:red:
   *
   * The value of the red channel.
   *
   * Since: 1.6
   */
  obj_props[PROP_RED] =
    g_param_spec_double ("red",
                         P_("Red"),
                         P_("The red value"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterRGBA:green:
   *
   * The value of the green channel.
   *
   * Since: 1.6
   */
  obj_props[PROP_GREEN] =
    g_param_spec_double ("green",
                         P_("Green"),
                         P_("The green value"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterRGBA:blue:
   *
   * The value of the blue channel.
   *
   * Since: 1.6
   */
  obj_props[PROP_BLUE] =
    g_param_spec_double ("blue",
                         P_("Blue"),
                         P_("The blue value"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterRGBA:alpha:
   *
   * The value of the alpha channel.
   *
   * Since: 1.6
   */
  obj_props[PROP_ALPHA] =
    g_param_spec_double ("alpha",
                         P_("Alpha"),
                         P_("The alpha value"),
                         0.0, 1.0,
                         0.0,
                         CLUTTER_PARAM_READWRITE);

  /**
   * ClutterRGBA:color:
   *
   * Sets all the channels of a #ClutterRGBA using a #ClutterColor.
   *
   * Since: 1.6
   */
  obj_props[PROP_COLOR] =
    clutter_param_spec_color ("color",
                              P_("Color"),
                              P_("The full color, as a ClutterColor"),
                              &default_color,
                              CLUTTER_PARAM_WRITABLE);

  /**
   * ClutterRGBA:string:
   *
   * Sets all the channels of a #ClutterRGBA using a string that
   * can be parsed by clutter_color_from_string().
   *
   * Since: 1.6
   */
  obj_props[PROP_STRING] =
    g_param_spec_string ("string",
                         P_("String"),
                         P_("The full color, as a string"),
                         NULL,
                         CLUTTER_PARAM_WRITABLE);

  gobject_class->set_property = clutter_rgba_set_property;
  gobject_class->get_property = clutter_rgba_get_property;
  g_object_class_install_properties (gobject_class, PROP_LAST, obj_props);
}

static void
clutter_rgba_init (ClutterRGBA *self)
{
}

/**
 * clutter_rgba_new:
 * @red: the value of the red channel, between 0 and 1
 * @green: the value of the green channel, between 0 and 1
 * @blue: the value of the blue channel, between 0 and 1
 * @alpha: the value of the alpha channel, between 0 and 1
 *
 * Creates a new #ClutterRGBA object for the given color. This
 * object can be used to paint a solid color inside a #ClutterActor,
 * by calling clutter_actor_set_content().
 *
 * Return value: (transfer full): the newly created #ClutterRGBA instance.
 *
 * Since: 1.6
 */
ClutterContent *
clutter_rgba_new (gdouble red,
                  gdouble green,
                  gdouble blue,
                  gdouble alpha)
{
  return g_object_new (CLUTTER_TYPE_RGBA,
                       "red", red,
                       "green", green,
                       "blue", blue,
                       "alpha", alpha,
                       NULL);
}

/**
 * clutter_rgba_new_from_color:
 * @color: a #ClutterColor
 *
 * Creates a new #ClutterRGBA instance, using @color as the source
 * for the color definition.
 *
 * Return value: (transfer full): the newly created #ClutterRGBA instance.
 *
 * Since: 1.6
 */
ClutterContent *
clutter_rgba_new_from_color (const ClutterColor *color)
{
  return g_object_new (CLUTTER_TYPE_RGBA,
                       "color", color,
                       NULL);
}

/**
 * clutter_rgba_new_from_string:
 * @string: a color definition, as parseable by clutter_color_from_string()
 *
 * Creates a new #ClutterRGBA instance, using the color definition contained
 * inside the passed @string.
 *
 * Return value: (transfer full): the newly created #ClutterRGBA instance
 *
 * Since: 1.6
 */
ClutterContent *
clutter_rgba_new_from_string (const gchar *string)
{
  return g_object_new (CLUTTER_TYPE_RGBA,
                       "string", string,
                       NULL);
}

void
clutter_rgba_set_red (ClutterRGBA *self,
                      gdouble      red)
{
  g_return_if_fail (CLUTTER_IS_RGBA (self));
  g_return_if_fail (red >= 0.0 && red <= 1.0);

  if (self->red == red)
    return;

  self->red = red;

  clutter_content_invalidate (CLUTTER_CONTENT (self));
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_RED]);
}

gdouble
clutter_rgba_get_red (ClutterRGBA *self)
{
  g_return_val_if_fail (CLUTTER_IS_RGBA (self), 0.0);

  return self->red;
}

void
clutter_rgba_set_green (ClutterRGBA *self,
                        gdouble      green)
{
  g_return_if_fail (CLUTTER_IS_RGBA (self));
  g_return_if_fail (green >= 0.0 && green <= 1.0);

  if (self->green == green)
    return;

  self->green = green;

  clutter_content_invalidate (CLUTTER_CONTENT (self));
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_GREEN]);
}

gdouble
clutter_rgba_get_green (ClutterRGBA *self)
{
  g_return_val_if_fail (CLUTTER_IS_RGBA (self), 0.0);

  return self->green;
}

void
clutter_rgba_set_blue (ClutterRGBA *self,
                       gdouble      blue)
{
  g_return_if_fail (CLUTTER_IS_RGBA (self));
  g_return_if_fail (blue >= 0.0 && blue <= 1.0);

  if (self->blue == blue)
    return;

  self->blue = blue;

  clutter_content_invalidate (CLUTTER_CONTENT (self));
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_BLUE]);
}

gdouble
clutter_rgba_get_blue (ClutterRGBA *self)
{
  g_return_val_if_fail (CLUTTER_IS_RGBA (self), 0.0);

  return self->blue;
}

void
clutter_rgba_set_alpha (ClutterRGBA *self,
                        gdouble      alpha)
{
  g_return_if_fail (CLUTTER_IS_RGBA (self));
  g_return_if_fail (alpha >= 0.0 && alpha <= 1.0);

  if (self->alpha == alpha)
    return;

  self->alpha = alpha;

  clutter_content_invalidate (CLUTTER_CONTENT (self));
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ALPHA]);
}

gdouble
clutter_rgba_get_alpha (ClutterRGBA *self)
{
  g_return_val_if_fail (CLUTTER_IS_RGBA (self), 0.0);

  return self->alpha;
}

/**
 * clutter_rgba_set_color:
 * @self: a #ClutterRGBA
 * @color: a #ClutterColor
 *
 * Sets the color values of a #ClutterRGBA using a #ClutterColor.
 *
 * Since: 1.6
 */
void
clutter_rgba_set_color (ClutterRGBA        *self,
                        const ClutterColor *color)
{
  GObject *obj;

  g_return_if_fail (CLUTTER_IS_RGBA (self));

  self->red   = CLAMP (color->red   / 255.0, 0.0, 1.0);
  self->green = CLAMP (color->green / 255.0, 0.0, 1.0);
  self->blue  = CLAMP (color->blue  / 255.0, 0.0, 1.0);
  self->alpha = CLAMP (color->alpha / 255.0, 0.0, 1.0);

  clutter_content_invalidate (CLUTTER_CONTENT (self));

  obj = G_OBJECT (self);
  g_object_notify_by_pspec (obj, obj_props[PROP_RED]);
  g_object_notify_by_pspec (obj, obj_props[PROP_GREEN]);
  g_object_notify_by_pspec (obj, obj_props[PROP_BLUE]);
  g_object_notify_by_pspec (obj, obj_props[PROP_ALPHA]);
}

/**
 * clutter_rgba_set_string:
 * @self: a #ClutterRGBA
 * @string: a color definition, as parseable by clutter_color_from_string()
 *
 * Sets the color values of a #ClutterRGBA using a @string definition.
 *
 * Since: 1.6
 */
void
clutter_rgba_set_string (ClutterRGBA *self,
                         const gchar *string)
{
  ClutterColor color;

  if (!clutter_color_from_string (&color, string))
    return;

  clutter_rgba_set_color (self, &color);
}
