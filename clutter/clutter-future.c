#include "config.h"

#include "clutter-future-private.h"

#define CLUTTER_DEFINE_FUTURE(_Name,_Optional,_Mandatory)	\
  { \
    _Name, \
    _Optional, \
    _Mandatory, \
  }

static const ClutterFuture __clutter_futures[] = {
  CLUTTER_DEFINE_FUTURE ("Last/Sentinel", CLUTTER_VERSION_1_0, CLUTTER_VERSION_1_0),
};

const char *
clutter_future_get_name (ClutterFutureFeature feature)
{
  g_assert (feature < CLUTTER_FUTURE_N_FEATURES);

  return __clutter_futures[feature].name;
}

guint
clutter_future_get_optional_version (ClutterFutureFeature feature)
{
  g_assert (feature < CLUTTER_FUTURE_N_FEATURES);

  return __clutter_futures[feature].optional_version;
}

guint
clutter_future_get_mandatory_version (ClutterFutureFeature feature)
{
  g_assert (feature < CLUTTER_FUTURE_N_FEATURES);

  return __clutter_futures[feature].mandatory_version;
}

gboolean
clutter_future_is_enabled (ClutterFutureFeature feature)
{
  g_assert (feature < CLUTTER_FUTURE_N_FEATURES);

  if (__clutter_futures[feature].optional_version > CLUTTER_VERSION_HEX)
    return FALSE;

  if (__clutter_futures[feature].mandatory_version <= CLUTTER_VERSION_HEX)
    return TRUE;

  return FALSE;
}
