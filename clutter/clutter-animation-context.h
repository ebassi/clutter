#ifndef __CLUTTER_ANIMATION_CONTEXT_H__
#define __CLUTTER_ANIMATION_CONTEXT_H__

#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_ANIMATION_CONTEXT          (clutter_animation_context_get_type ())
#define CLUTTER_ANIMATION_CONTEXT(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_ANIMATION_CONTEXT, ClutterAnimationContext))
#define CLUTTER_IS_ANIMATION_CONTEXT(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_ANIMATION_CONTEXT))

typedef struct _ClutterAnimationContextClass    ClutterAnimationContextClass;

GType clutter_animation_context_get_type (void) G_GNUC_CONST;

ClutterAnimationContext *       clutter_animation_context_new           (ClutterAnimatable       *animatable,
                                                                         gulong                   mode,
                                                                         guint                    duration);

void                    clutter_animation_context_add_property          (ClutterAnimationContext *context,
                                                                         const gchar             *name,
                                                                         ClutterInterval         *interval);
void                    clutter_animation_context_remove_property       (ClutterAnimationContext *context,
                                                                         const gchar             *name);
gboolean                clutter_animation_context_has_property          (ClutterAnimationContext *context,
                                                                         const gchar             *name);

ClutterAnimatable *     clutter_animation_context_get_animatable        (ClutterAnimationContext *context);

void                    clutter_animation_context_add_hints             (ClutterAnimationContext *context,
                                                                         ClutterAnimationHint     hint);
ClutterAnimationHint    clutter_animation_context_get_hints             (ClutterAnimationContext *context);

G_END_DECLS

#endif /* __CLUTTER_ANIMATION_CONTEXT_H__ */
