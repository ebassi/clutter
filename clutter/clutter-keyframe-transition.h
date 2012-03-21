#ifndef __CLUTTER_KEYFRAME_TRANSITION_H__
#define __CLUTTER_KEYFRAME_TRANSITION_H__

#include <clutter/clutter-types.h>
#include <clutter/clutter-property-transition.h>

G_BEGIN_DECLS

typedef struct _ClutterKeyframeTransitionPrivate        ClutterKeyframeTransitionPrivate;
typedef struct _ClutterKeyframeTransitionClass          ClutterKeyframeTransitionClass;

/**
 * ClutterKeyframeTransition:
 *
 * FIXME
 *
 * Since: 1.12
 */
struct _ClutterKeyframeTransition
{
  /*< private >*/
  ClutterPropertyTransition parent_instance;

  ClutterKeyframeTransitionPrivate *priv;
};

/**
 * ClutterKeyframeTransitionClass:
 *
 * FIXME
 *
 * Since: 1.12
 */
struct _ClutterKeyframeTransitionClass
{
  /*< private >*/
  ClutterPropertyTransitionClass parent_class;

  gpointer _padding[8];
};

GType clutter_keyframe_transition_get_type (void) G_GNUC_CONST;

ClutterTransition *     clutter_keyframe_transition_new                 (const char *property_name);

void                    clutter_keyframe_transition_set_key_frames      (ClutterKeyframeTransition *transition,
                                                                         guint                      n_key_frames,
                                                                         const double              *key_frames);
void                    clutter_keyframe_transition_set_values          (ClutterKeyframeTransition *transition,
                                                                         guint                      n_values,
                                                                         const GValue              *values);
void                    clutter_keyframe_transition_set                 (ClutterKeyframeTransition *transition,
                                                                         GType                      gtype,
                                                                         guint                      n_values,
                                                                         ...);

G_END_DECLS

#endif /* __CLUTTER_KEYFRAME_TRANSITION_H__ */
