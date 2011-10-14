#include "config.h"

#include "clutter-animation-context.h"

#include "clutter-animatable.h"
#include "clutter-interval.h"
#include "clutter-private.h"

#define CLUTTER_ANIMATION_CONTEXT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_ANIMATION_CONTEXT, ClutterAnimationContextClass))
#define CLUTTER_IS_ANIMATION_CONTEXT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_ANIMATION_CONTEXT))
#define CLUTTER_ANIMATION_CONTEXT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_ANIMATION_CONTEXT, ClutterAnimationContextClass))

struct _ClutterAnimationContext
{
  GObject parent_instance;

  ClutterAnimatable *animatable;

  guint duration;
  gulong mode;

  ClutterAnimationHint hints;

  GHashTable *properties;
};

struct _ClutterAnimationContextClass
{
  GObjectClass parent_class;
};

enum
{
  PROP_0,

  ANIMATABLE,
  DURATION,
  MODE,

  LAST_PROPERTY
};

static GParamSpec *obj_props[LAST_PROPERTY] = { NULL, };

G_DEFINE_TYPE (ClutterAnimationContext, clutter_animation_context, G_TYPE_OBJECT)

static void
clutter_animation_context_set_property (GObject      *gobject,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  ClutterAnimationContext *self = CLUTTER_ANIMATION_CONTEXT (gobject);

  switch (prop_id)
    {
    case MODE:
      self->mode = g_value_get_ulong (value);
      break;

    case DURATION:
      self->duration = g_value_get_uint (value);
      break;

    case ANIMATABLE:
      self->animatable = g_value_get_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_animation_context_get_property (GObject    *gobject,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  ClutterAnimationContext *self = CLUTTER_ANIMATION_CONTEXT (gobject);

  switch (prop_id)
    {
    case MODE:
      g_value_set_ulong (value, self->mode);
      break;

    case DURATION:
      g_value_set_uint (value, self->duration);
      break;

    case ANIMATABLE:
      g_value_set_object (value, self->animatable);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_animation_context_finalize (GObject *gobject)
{
  g_hash_table_destroy (CLUTTER_ANIMATION_CONTEXT (gobject)->properties);

  G_OBJECT_CLASS (clutter_animation_context_parent_class)->finalize (gobject);
}

static void
clutter_animation_context_class_init (ClutterAnimationContextClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  obj_props[ANIMATABLE] =
    g_param_spec_object ("animatable",
                         P_("Animatable"),
                         P_("The animatable instance"),
                         CLUTTER_TYPE_ANIMATABLE,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_STRINGS |
                         G_PARAM_CONSTRUCT_ONLY);

  obj_props[MODE] =
    g_param_spec_ulong ("mode",
                        P_("Mode"),
                        P_("The mode of the animation"),
                        0, G_MAXULONG,
                        CLUTTER_LINEAR,
                        G_PARAM_READWRITE |
                        G_PARAM_STATIC_STRINGS |
                        G_PARAM_CONSTRUCT_ONLY);

  obj_props[DURATION] =
    g_param_spec_uint ("duration",
                       P_("Duration"),
                       P_("Duration of the animation, in milliseconds"),
                       0, G_MAXUINT,
                       200,
                       G_PARAM_READWRITE |
                       G_PARAM_STATIC_STRINGS |
                       G_PARAM_CONSTRUCT_ONLY);

  gobject_class->set_property = clutter_animation_context_set_property;
  gobject_class->get_property = clutter_animation_context_get_property;
  gobject_class->finalize = clutter_animation_context_finalize;

  g_object_class_install_properties (gobject_class, LAST_PROPERTY, obj_props);
}

static void
clutter_animation_context_init (ClutterAnimationContext *self)
{
  self->mode = CLUTTER_LINEAR;
  self->duration = 200;

  self->properties = g_hash_table_new (NULL, NULL);
}

ClutterAnimationContext *
clutter_animation_context_new (ClutterAnimatable *animatable,
                               gulong             mode,
                               guint              duration)
{
  g_return_val_if_fail (CLUTTER_IS_ANIMATABLE (animatable), NULL);

  return g_object_new (CLUTTER_TYPE_ANIMATION_CONTEXT,
                       "animatable", animatable,
                       "mode", mode,
                       "duration", duration,
                       NULL);
}

void
clutter_animation_context_add_property (ClutterAnimationContext *context,
                                        const gchar             *name,
                                        ClutterInterval         *interval)
{
  g_return_if_fail (CLUTTER_IS_ANIMATION_CONTEXT (context));
  g_return_if_fail (name != NULL);
  g_return_if_fail (CLUTTER_IS_INTERVAL (interval));

  g_hash_table_insert (context->properties,
                       (gpointer) g_intern_string (name),
                       interval);
}

void
clutter_animation_context_remove_property (ClutterAnimationContext *context,
                                           const gchar             *name)
{
  g_return_if_fail (CLUTTER_IS_ANIMATION_CONTEXT (context));
  g_return_if_fail (name != NULL);

  g_hash_table_remove (context->properties, g_intern_string (name));
}

gboolean
clutter_animation_context_has_property (ClutterAnimationContext *context,
                                        const gchar             *name)
{
  g_return_val_if_fail (CLUTTER_IS_ANIMATION_CONTEXT (context), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);

  return g_hash_table_lookup (context->properties, g_intern_string (name)) != NULL;
}

ClutterAnimatable *
clutter_animation_context_get_animatable (ClutterAnimationContext *context)
{
  g_return_val_if_fail (CLUTTER_IS_ANIMATION_CONTEXT (context), NULL);

  return context->animatable;
}

void
clutter_animation_context_add_hints (ClutterAnimationContext *context,
                                     ClutterAnimationHint     hints)
{
  g_return_if_fail (CLUTTER_IS_ANIMATION_CONTEXT (context));

  context->hints |= hints;
}

ClutterAnimationHint
clutter_animation_context_get_hints (ClutterAnimationContext *context)
{
  g_return_val_if_fail (CLUTTER_IS_ANIMATION_CONTEXT (context), 0);

  return context->hints;
}
