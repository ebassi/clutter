#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-keyframe-transition.h"

typedef struct _KeyFrame
{
  double key;
  GValue value;
} KeyFrame;

struct _ClutterKeyframeTransitionPrivate
{
  GArray *frames;
};
