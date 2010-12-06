#include "config.h"

#include "clutter-image-loader.h"

#include <string.h>
#include <gio/gio.h>

#include "clutter-private.h"

G_DEFINE_ABSTRACT_TYPE (ClutterImageLoader, clutter_image_loader, G_TYPE_OBJECT);

static void
clutter_image_loader_class_init (ClutterImageLoaderClass *klass)
{
}

static void
clutter_image_loader_init (ClutterImageLoader *loader)
{
}

static gpointer
get_default_image_loader (gpointer data)
{
  ClutterImageLoaderClass *chosen_class;
  ClutterImageLoaderClass **ret = data;
  GIOExtensionPoint *ep;
  GList *extensions, *l;

  _clutter_io_modules_ensure_loaded ();

  ep = g_io_extension_point_lookup (CLUTTER_IMAGE_LOADER_EXTENSION_POINT_NAME);

  extensions = g_io_extension_point_get_extensions (ep);

  chosen_class = NULL;
  for (l = extensions; l != NULL; l = l->next)
    {
      GIOExtension *extension = l->data;
      ClutterImageLoaderClass *klass;

      klass = CLUTTER_IMAGE_LOADER_CLASS (g_io_extension_ref_class (extension));

      if (klass->is_supported ())
        {
          chosen_class = klass;
          break;
        }
      else
        g_type_class_unref (klass);
    }

  if (chosen_class)
    {
      *ret = chosen_class;
      return (gpointer) G_TYPE_FROM_CLASS (chosen_class);
    }
  else
    return (gpointer) G_TYPE_INVALID;
}

ClutterImageLoader *
_clutter_image_loader_new (void)
{
  static GOnce once_init = G_ONCE_INIT;
  GTypeClass *type_class = NULL;
  ClutterImageLoader *loader;
  GType type = G_TYPE_INVALID;

  g_once (&once_init, get_default_image_loader, &type_class);
  type = (GType) once_init.retval;
  if (type == G_TYPE_INVALID)
    {
      g_critical ("Unable to load the default image loader. Check the "
                  "compilation options of Clutter.");
      return NULL;
    }

  loader = g_object_new (type, NULL);

  if (type_class != NULL)
    g_type_class_unref (type_class);

  return loader;
}

void
_clutter_image_loader_get_image_size (ClutterImageLoader *loader,
                                      gint               *width,
                                      gint               *height)
{
  g_return_if_fail (CLUTTER_IS_IMAGE_LOADER (loader));

  CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->get_image_size (loader,
                                                           width,
                                                           height);
}

CoglHandle
_clutter_image_loader_get_texture_handle (ClutterImageLoader *loader)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE_LOADER (loader), NULL);

  return CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->get_texture_handle (loader);
}

gboolean
_clutter_image_loader_load_stream (ClutterImageLoader  *loader,
                                   GInputStream        *stream,
                                   GCancellable        *cancellable,
                                   GError             **error)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE_LOADER (loader), FALSE);
  g_return_val_if_fail (G_IS_INPUT_STREAM (stream), FALSE);
  g_return_val_if_fail (cancellable == NULL ||
                        G_IS_CANCELLABLE (cancellable), FALSE);


  return CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->load_stream (loader, stream,
                                                               cancellable,
                                                               error);
}

void
_clutter_image_loader_load_stream_async (ClutterImageLoader  *loader,
                                         GInputStream        *stream,
                                         GCancellable        *cancellable,
                                         GAsyncReadyCallback  callback,
                                         gpointer             user_data)
{
  g_return_if_fail (CLUTTER_IS_IMAGE_LOADER (loader));
  g_return_if_fail (G_IS_INPUT_STREAM (stream));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail (callback != NULL);

  CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->load_stream_async (loader, stream,
                                                              cancellable,
                                                              callback,
                                                              user_data);
}

gboolean
_clutter_image_loader_load_stream_finish (ClutterImageLoader  *loader,
                                          GAsyncResult        *result,
                                          GError             **error)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE_LOADER (loader), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  return CLUTTER_IMAGE_LOADER_GET_CLASS (loader)->load_stream_finish (loader,
                                                                      result,
                                                                      error);
}
