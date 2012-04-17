#include "config.h"

#include "clutter-future-private.h"

#define CLUTTER_DEFINE_FUTURE(_Name,_Optional,_Mandatory)	\
  { \
    _Name, \
    _Optional, \
    _Mandatory, \
  }

static const ClutterFuture __clutter_futures[CLUTTER_FUTURE_N_FEATURES] = {
  CLUTTER_DEFINE_FUTURE ("default-easing-state",
                         G_ENCODE_VERSION (1, 11),
                         G_ENCODE_VERSION (2, 0)),
};

static gboolean __clutter_enabled_futures[CLUTTER_FUTURE_N_FEATURES] = { FALSE, };

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
  guint version;

  g_assert (feature < CLUTTER_FUTURE_N_FEATURES);

  version = G_ENCODE_VERSION (CLUTTER_MAJOR_VERSION, CLUTTER_MINOR_VERSION);

  if (__clutter_futures[feature].optional_version > version)
    return FALSE;

  if (__clutter_futures[feature].mandatory_version <= version)
    return TRUE;

  return __clutter_enabled_futures[feature];
}

void
clutter_future_enable (ClutterFutureFeature feature)
{
  g_assert (feature < CLUTTER_FUTURE_N_FEATURES);

  __clutter_enabled_futures[feature] = TRUE;
}
