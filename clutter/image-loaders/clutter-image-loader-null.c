#include "config.h"

#include "clutter-image-loader-null.h"

#include <cogl/cogl.h>
#include <gio/gio.h>

struct _ClutterImageLoaderNull
{
  ClutterImageLoader parent_instance;
};

#define clutter_image_loader_null_get_type    _clutter_image_loader_null_get_type
G_DEFINE_TYPE_WITH_CODE (ClutterImageLoaderNull,
                         clutter_image_loader_null,
                         CLUTTER_TYPE_IMAGE_LOADER,
                         g_io_extension_point_implement (CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME,
                                                         g_define_type_id,
                                                         "null",
                                                         0));

static void
clutter_image_loader_null_finalize (GObject *gobject)
{
  G_OBJECT_CLASS (clutter_image_loader_null_parent_class)->finalize (gobject);
}

static gboolean
clutter_image_loader_null_is_supported (void)
{
  return FALSE;
}

static void
clutter_image_loader_null_get_image_size (ClutterImageLoader *loader,
                                          gint               *width,
                                          gint               *height)
{
  if (width)
    *width = 0;

  if (height)
    *height = 0;
}

static CoglHandle
clutter_image_loader_null_get_texture_handle (ClutterImageLoader *loader)
{
  return COGL_INVALID_HANDLE;
}

static void
clutter_image_loader_null_class_init (ClutterImageLoaderNullClass *klass)
{
  ClutterImageLoaderClass *loader_class = CLUTTER_IMAGE_LOADER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = clutter_image_loader_null_finalize;

  loader_class->is_supported = clutter_image_loader_null_is_supported;
  loader_class->get_image_size = clutter_image_loader_null_get_image_size;
  loader_class->get_texture_handle = clutter_image_loader_null_get_texture_handle;
}

static void
clutter_image_loader_null_init (ClutterImageLoaderNull *loader)
{
}
