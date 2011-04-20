#ifndef __CLUTTER_MACROS_PRIVATE_H__
#define __CLUTTER_MACROS_PRIVATE_H__

#define CLUTTER_REGISTER_VALUE_TRANSFORM_TO(TYPE_TO,func)             { \
  g_value_register_transform_func (g_define_type_id, TYPE_TO, func);    \
}

#define CLUTTER_REGISTER_VALUE_TRANSFORM_FROM(TYPE_FROM,func)         { \
  g_value_register_transform_func (TYPE_FROM, g_define_type_id, func);  \
}

#define CLUTTER_REGISTER_INTERVAL_PROGRESS(func)                      { \
  clutter_interval_register_progress_func (g_define_type_id, func);     \
}

#define CLUTTER_PRIVATE_FLAGS(a)	 (((ClutterActor *) (a))->private_flags)
#define CLUTTER_SET_PRIVATE_FLAGS(a,f)	 (CLUTTER_PRIVATE_FLAGS (a) |= (f))
#define CLUTTER_UNSET_PRIVATE_FLAGS(a,f) (CLUTTER_PRIVATE_FLAGS (a) &= ~(f))

#define CLUTTER_ACTOR_IS_TOPLEVEL(a)            ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_IS_TOPLEVEL) != FALSE)
#define CLUTTER_ACTOR_IS_INTERNAL_CHILD(a)      ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_INTERNAL_CHILD) != FALSE)
#define CLUTTER_ACTOR_IN_DESTRUCTION(a)         ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_IN_DESTRUCTION) != FALSE)
#define CLUTTER_ACTOR_IN_REPARENT(a)            ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_IN_REPARENT) != FALSE)
#define CLUTTER_ACTOR_IN_PAINT(a)               ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_IN_PAINT) != FALSE)
#define CLUTTER_ACTOR_IN_RELAYOUT(a)            ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_IN_RELAYOUT) != FALSE)
#define CLUTTER_STAGE_IN_RESIZE(a)              ((CLUTTER_PRIVATE_FLAGS (a) & CLUTTER_IN_RESIZE) != FALSE)

#define CLUTTER_PARAM_READABLE  (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS)
#define CLUTTER_PARAM_WRITABLE  (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)
#define CLUTTER_PARAM_READWRITE (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)

/* automagic interning of a static string */
#define I_(str)  (g_intern_static_string ((str)))

/* mark all properties under the "Property" context */
#ifdef ENABLE_NLS
#define P_(String) (_clutter_gettext ((String)))
#else
#define P_(String) (String)
#endif

#define _G_DEFINE_PROPERTY_SYNTH_GET_DECL(TypeName, type_name, field_type, field_name) \
field_type type_name##_get_##field_name (TypeName *self)

#define _G_DEFINE_PROPERTY_SYNTH_GET_BEGIN(TypeName, type_name, field_type, field_name, default_value) \
{ \
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (self, type_name##_get_type ()), default_value); \
  { /* custom code follows */
#define _G_DEFINE_PROPERTY_SYNTH_GET_END(field_name)    \
  } /* following custom code */                         \
  return self->priv->field_name;                        \
}

#define _G_DEFINE_PROPERTY_SYNTH_SET_DECL(TypeName, type_name, field_type, field_name) \
\
void type_name##_set_##field_name (TypeName *self, field_type value)

#define _G_DEFINE_PROPERTY_SYNTH_SET_BEGIN(TypeName, type_name, field_type, field_name) \
{ \
  TypeName##Private *priv; \
\
  g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (self, type_name##_get_type ())); \
\
  priv = self->priv; \
\
  if (priv->field_name != value) \
    { \
      priv->field_name = value; \
      { /* custom code follows */
#define _G_DEFINE_PROPERTY_SYNTH_SET_END()      \
      }/* following custom code */              \
    }                                           \
}

/*< private >
 * G_DEFINE_PROPERTY:
 * @TypeName: the name of the type, in Camel case
 * @type_name: the name of the type, in lowercase, with words separated by '_'
 * @field_type: the type of the property, which must match the type of the
 *   field in the @TypeName<!-- -->Private structure
 * @field_name: the name of the property, which must match the name of the
 *   field in the @TypeName<!-- -->Private structure
 *
 * Declares the accessor functions for a @field_name property in the
 * class @TypeName. This macro should only be used in header files.
 */
#define G_DEFINE_PROPERTY(TypeName, type_name, field_type, field_name)           \
_G_DEFINE_PROPERTY_SYNTH_SET_DECL (TypeName, type_name, field_type, field_name); \
_G_DEFINE_PROPERTY_SYNTH_GET_DECL (TypeName, type_name, field_type, field_name);

/*< private >
 * G_DEFINE_PROPERTY_SYNTH_SET_WITH_CODE:
 * @TypeName: the name of the type, in Camel case
 * @type_name: the name of the type, in lowercase, with words separated by '_'
 * @field_type: the type of the property, which must match the type of the
 *   field in the @TypeName<!-- -->Private structure
 * @field_name: the name of the property, which must match the name of the
 *   field in the @TypeName<!-- -->Private structure
 *
 * Defines the setter function for a @field_name property in the
 * class @TypeName, with the possibility of calling custom code.
 *
 * This macro should only be used in C source files.
 *
 * This macro should be used when intending to notify on #GObject properties,
 * for instance:
 *
 * |[
 * G_DEFINE_PROPERTY_SYNTH_SET_WITH_CODE (ClutterActor, clutter_actor,
 *                                        int, margin_top,
 *                                        g_object_notify (G_OBJECT (self), "margin-top"))
 * ]|
 */
#define G_DEFINE_PROPERTY_SYNTH_SET_WITH_CODE(TypeName, type_name, field_type, field_name, _C_) \
_G_DEFINE_PROPERTY_SYNTH_SET_DECL (TypeName, type_name, field_type, field_name) \
_G_DEFINE_PROPERTY_SYNTH_SET_BEGIN (TypeName, type_name, field_type, field_name) \
{ _C_; } \
_G_DEFINE_PROPERTY_SYNTH_SET_END ()

/*< private >
 * G_DEFINE_PROPERTY_SYNTH_GET_WITH_CODE:
 * @TypeName: the name of the type, in Camel case
 * @type_name: the name of the type, in lowercase, with words separated by '_'
 * @field_type: the type of the property, which must match the type of the
 *   field in the @TypeName<!-- -->Private structure
 * @field_name: the name of the property, which must match the name of the
 *   field in the @TypeName<!-- -->Private structure
 * @ret: default return value (mostly for error conditions) for the getter
 *   function
 *
 * Defines the getter function for a @field_name property in the
 * class @TypeName, with the possibility of calling custom code.
 *
 * This macro should only be used in C source files.
 */
#define G_DEFINE_PROPERTY_SYNTH_GET_WITH_CODE(TypeName, type_name, field_type, field_name, ret, _C_) \
_G_DEFINE_PROPERTY_SYNTH_GET_DECL (TypeName, type_name, field_type, field_name) \
_G_DEFINE_PROPERTY_SYNTH_GET_BEGIN (TypeName, type_name, field_type, field_name, ret) \
{ _C_; } \
_G_DEFINE_PROPERTY_SYNTH_GET_END (field_name);

/*< private >
 * G_DEFINE_PROPERTY_SYNTH_SET:
 * @TypeName: the name of the type, in Camel case
 * @type_name: the name of the type, in lowercase, with words separated by '_'
 * @field_type: the type of the property, which must match the type of the
 *   field in the @TypeName<!-- -->Private structure
 * @field_name: the name of the property, which must match the name of the
 *   field in the @TypeName<!-- -->Private structure
 *
 * Defines the setter function for a @field_name property in the
 * class @TypeName. This macro should only be used in C source files.
 *
 * See also %G_DEFINE_PROPERTY_SYNTH_SET_WITH_CODE.
 */
#define G_DEFINE_PROPERTY_SYNTH_SET(TypeName, type_name, field_type, field_name)        G_DEFINE_PROPERTY_SYNTH_SET_WITH_CODE (TypeName, type_name, field_type, field_name, ;)

/*< private >
 * G_DEFINE_PROPERTY_SYNTH_GET:
 * @TypeName: the name of the type, in Camel case
 * @type_name: the name of the type, in lowercase, with words separated by '_'
 * @field_type: the type of the property, which must match the type of the
 *   field in the @TypeName<!-- -->Private structure
 * @field_name: the name of the property, which must match the name of the
 *   field in the @TypeName<!-- -->Private structure
 * @ret: default return value (mostly for error conditions) for the getter
 *   function
 *
 * Defines the getter function for a @field_name property in the
 * class @TypeName. This macro should only be used in C source files.
 *
 * See also %G_DEFINE_PROPERTY_SYNTH_GET_WITH_CODE.
 */
#define G_DEFINE_PROPERTY_SYNTH_GET(TypeName, type_name, field_type, field_name, ret)   G_DEFINE_PROPERTY_SYNTH_GET_WITH_CODE (TypeName, type_name, field_type, field_name, ret, ;)

/*< private >
 * G_DEFINE_PROPERTY_SYNTH:
 * @TypeName: the name of the type, in Camel case
 * @type_name: the name of the type, in lowercase, with words separated by '_'
 * @field_type: the type of the property, which must match the type of the
 *   field in the @TypeName<!-- -->Private structure
 * @field_name: the name of the property, which must match the name of the
 *   field in the @TypeName<!-- -->Private structure
 * @ret: default return value (mostly for error conditions) for the getter
 *   function
 *
 * Defines the accessor functions for a @field_name property in the
 * class @TypeName. This macro should only be used in C source files, for
 * instance:
 *
 * |[
 *   G_DEFINE_PROPERTY_SYNTH (ClutterActor, clutter_actor, int, margin_top, 0)
 * ]|
 *
 * will synthesize the following code:
 *
 * |[
 * void
 * clutter_actor_set_margin_top (ClutterActor *self,
 *                               int           value)
 * {
 *   ClutterActorPrivate *priv;
 *
 *   g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (self, clutter_actor_get_type ()));
 *
 *   priv = self->priv;
 *
 *   if (priv->margin_top != value)
 *     {
 *       priv->value = value
 *     }
 * }
 *
 * int
 * clutter_actor_get_margin_top (ClutterActor *self)
 * {
 *   g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (self, clutter_actor_get_type ()), 0);
 *
 *   return self->priv->margin_top;
 * }
 * ]|
 */
#define G_DEFINE_PROPERTY_SYNTH(TypeName, type_name, field_type, field_name, ret)       \
G_DEFINE_PROPERTY_SYNTH_SET (TypeName, type_name, field_type, field_name)       \
G_DEFINE_PROPERTY_SYNTH_GET (TypeName, type_name, field_type, field_name, ret)

#endif /* __CLUTTER_MACROS_PRIVATE_H__ */
