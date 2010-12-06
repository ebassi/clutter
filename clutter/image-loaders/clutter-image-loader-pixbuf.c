#include "config.h"

#include "clutter-image-loader-pixbuf.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>
#include <cogl/cogl.h>

#include "clutter-debug.h"
#include "clutter-image.h"
#include "clutter-private.h"

struct _ClutterImageLoaderPixbuf
{
  ClutterImageLoader parent_instance;

  GdkPixbufLoader *loader;

  gint image_width;
  gint image_height;
  gint rowstride;

  CoglPixelFormat pixel_format;

  CoglHandle texture;
};

#define clutter_image_loader_pixbuf_get_type    _clutter_image_loader_pixbuf_get_type
G_DEFINE_TYPE_WITH_CODE (ClutterImageLoaderPixbuf,
                         clutter_image_loader_pixbuf,
                         CLUTTER_TYPE_IMAGE_LOADER,
                         g_io_extension_point_implement (CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME,
                                                         g_define_type_id,
                                                         "gdk-pixbuf",
                                                         20));

static void
clutter_image_loader_pixbuf_dispose (GObject *gobject)
{
  ClutterImageLoaderPixbuf *loader;

  loader = CLUTTER_IMAGE_LOADER_PIXBUF (gobject);

  if (loader->loader != NULL)
    {
      gdk_pixbuf_loader_close (loader->loader, NULL);
      g_object_unref (loader->loader);
      loader->loader = NULL;
    }

  if (loader->texture != COGL_INVALID_HANDLE)
    {
      cogl_handle_unref (loader->texture);
      loader->texture = COGL_INVALID_HANDLE;
    }

  G_OBJECT_CLASS (clutter_image_loader_pixbuf_parent_class)->dispose (gobject);
}

static gboolean
clutter_image_loader_pixbuf_is_supported (void)
{
  return TRUE;
}

static gboolean
clutter_image_loader_pixbuf_load_stream (ClutterImageLoader *loader,
                                         GInputStream       *stream,
                                         GCancellable       *cancellable,
                                         GError            **error)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);
  GdkPixbuf *pixbuf;

  pixbuf = gdk_pixbuf_new_from_stream (stream, cancellable, error);
  if (pixbuf == NULL)
    return FALSE;

  self->image_width = gdk_pixbuf_get_width (pixbuf);
  self->image_height = gdk_pixbuf_get_height (pixbuf);
  self->pixel_format = gdk_pixbuf_get_has_alpha (pixbuf)
                     ? COGL_PIXEL_FORMAT_RGBA_8888
                     : COGL_PIXEL_FORMAT_RGB_888;
  self->rowstride = gdk_pixbuf_get_rowstride (pixbuf);

  CLUTTER_NOTE (MISC, "Image: %d x %d (rowstride: %d, has-alpha: %s)",
                self->image_width,
                self->image_height,
                self->rowstride,
                gdk_pixbuf_get_has_alpha (pixbuf) ? "yes" : "no");

  self->texture = cogl_texture_new_from_data (self->image_width,
                                              self->image_height,
                                              COGL_TEXTURE_NONE,
                                              self->pixel_format,
                                              COGL_PIXEL_FORMAT_ANY,
                                              self->rowstride,
                                              gdk_pixbuf_get_pixels (pixbuf));

  g_object_unref (pixbuf);

  if (self->texture == COGL_INVALID_HANDLE &&
      (error != NULL && *error == NULL))
    g_set_error_literal (error, CLUTTER_IMAGE_ERROR,
                         CLUTTER_IMAGE_ERROR_INVALID_DATA,
                         _("Unable to load the image data"));

  return (self->texture != COGL_INVALID_HANDLE);
}

static void
clutter_image_loader_pixbuf_get_image_size (ClutterImageLoader *loader,
                                            gint               *width,
                                            gint               *height)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);

  if (width)
    *width = self->image_width;

  if (height)
    *height = self->image_height;
}

static CoglHandle
clutter_image_loader_pixbuf_get_texture_handle (ClutterImageLoader *loader)
{
  ClutterImageLoaderPixbuf *self = CLUTTER_IMAGE_LOADER_PIXBUF (loader);

  return self->texture;
}

static void
clutter_image_loader_pixbuf_class_init (ClutterImageLoaderPixbufClass *klass)
{
  ClutterImageLoaderClass *loader_class = CLUTTER_IMAGE_LOADER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = clutter_image_loader_pixbuf_dispose;

  loader_class->is_supported = clutter_image_loader_pixbuf_is_supported;
  loader_class->load_stream = clutter_image_loader_pixbuf_load_stream;
  loader_class->get_image_size = clutter_image_loader_pixbuf_get_image_size;
  loader_class->get_texture_handle = clutter_image_loader_pixbuf_get_texture_handle;
}

static void
clutter_image_loader_pixbuf_init (ClutterImageLoaderPixbuf *loader)
{
}
