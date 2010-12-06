#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cogl/cogl.h>

#include "clutter-image.h"

#include "clutter-debug.h"
#include "clutter-image-loader.h"
#include "clutter-marshal.h"
#include "clutter-private.h"

struct _ClutterImagePrivate
{
  ClutterImageLoader *loader;

  gint image_width;
  gint image_height;

  CoglMaterial *material;
};

enum
{
  PROP_0,

  PROP_WIDTH,
  PROP_HEIGHT,

  LAST_PROP
};

static GParamSpec *image_props[LAST_PROP] = { NULL, };

enum
{
  SIZE_CHANGED,

  LAST_SIGNAL
};

static guint image_signals[LAST_SIGNAL] = { 0, };

static CoglMaterial *image_template_material = NULL;

G_DEFINE_TYPE (ClutterImage, clutter_image, G_TYPE_OBJECT);

static CoglMaterial *
copy_template_material (void)
{
  if (G_UNLIKELY (image_template_material == NULL))
    {
      CoglHandle dummy_tex;

      dummy_tex = cogl_texture_new_with_size (1, 1, COGL_TEXTURE_NO_SLICING,
                                              COGL_PIXEL_FORMAT_RGBA_8888_PRE);

      image_template_material = cogl_material_new ();
      cogl_material_set_layer (image_template_material, 0, dummy_tex);
      cogl_handle_unref (dummy_tex);
    }

  return cogl_material_copy (image_template_material);
}

static void
clutter_image_dispose (GObject *gobject)
{
  ClutterImagePrivate *priv = CLUTTER_IMAGE (gobject)->priv;

  if (priv->material != NULL)
    {
      cogl_object_unref (priv->material);
      priv->material = NULL;
    }

  G_OBJECT_CLASS (clutter_image_parent_class)->dispose (gobject);
}

static void
clutter_image_real_size_changed (ClutterImage *image,
                                 gint          width,
                                 gint          height)
{
}

static void
clutter_image_get_property (GObject    *gobject,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  ClutterImagePrivate *priv = CLUTTER_IMAGE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_WIDTH:
      g_value_set_int (value, priv->image_width);
      break;

    case PROP_HEIGHT:
      g_value_set_int (value, priv->image_height);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
clutter_image_class_init (ClutterImageClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ClutterImagePrivate));

  image_props[PROP_WIDTH] =
    g_param_spec_int ("width",
                      P_("Width"),
                      P_("The width of the image, in pixels"),
                      0, G_MAXINT,
                      0,
                      CLUTTER_PARAM_READABLE);

  image_props[PROP_HEIGHT] =
    g_param_spec_int ("height",
                      P_("Height"),
                      P_("The height of the image, in pixels"),
                      0, G_MAXINT,
                      0,
                      CLUTTER_PARAM_READABLE);

  gobject_class->dispose = clutter_image_dispose;
  gobject_class->get_property = clutter_image_get_property;
  g_object_class_install_properties (gobject_class, LAST_PROP, image_props);

  gobject_class->dispose = clutter_image_dispose;

  image_signals[SIZE_CHANGED] =
    g_signal_new (I_("size-changed"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterImageClass, size_changed),
                  NULL, NULL,
                  _clutter_marshal_VOID__INT_INT,
                  G_TYPE_NONE, 2,
                  G_TYPE_INT,
                  G_TYPE_INT);

  klass->size_changed = clutter_image_real_size_changed;
}

static void
clutter_image_init (ClutterImage *image)
{
  image->priv = G_TYPE_INSTANCE_GET_PRIVATE (image, CLUTTER_TYPE_IMAGE,
                                             ClutterImagePrivate);

  image->priv->material = copy_template_material ();
}

ClutterImage *
clutter_image_new (void)
{
  return g_object_new (CLUTTER_TYPE_IMAGE, NULL);
}

void
clutter_image_get_size (ClutterImage *image,
                        gint         *width,
                        gint         *height)
{
  g_return_if_fail (CLUTTER_IS_IMAGE (image));

  if (width)
    *width = image->priv->image_width;

  if (height)
    *height = image->priv->image_height;
}

gboolean
clutter_image_load_from_data (ClutterImage     *image,
                              const guchar     *data,
                              CoglPixelFormat   format,
                              gint              width,
                              gint              height,
                              gint              rowstride,
                              gint              bpp,
                              GError          **error)
{
  ClutterImagePrivate *priv;
  CoglHandle texture;

  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  priv = image->priv;

  texture = cogl_texture_new_from_data (width, height,
                                        COGL_TEXTURE_NONE,
                                        format,
                                        COGL_PIXEL_FORMAT_ANY,
                                        rowstride,
                                        data);
  if (texture == COGL_INVALID_HANDLE)
    {
      g_set_error (error, CLUTTER_IMAGE_ERROR,
                   CLUTTER_IMAGE_ERROR_INVALID_DATA,
                   _("Unable to load image data"));
      return FALSE;
    }

  cogl_material_set_layer (priv->material, 0, texture);
  cogl_handle_unref (texture);

  return TRUE;
}

gboolean
clutter_image_load (ClutterImage  *image,
                    GFile         *gfile,
                    GCancellable  *cancellable,
                    gint          *width,
                    gint          *height,
                    GError       **error)
{
  ClutterImagePrivate *priv;
  GFileInputStream *stream;
  CoglHandle texture;
  gboolean res;

  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (G_IS_FILE (gfile), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable),
                        FALSE);

  priv = image->priv;

  if (priv->loader == NULL)
    {
      priv->loader = _clutter_image_loader_new ();
      if (priv->loader == NULL)
        return FALSE;

      CLUTTER_NOTE (MISC, "Image loader type: %s",
                    G_OBJECT_TYPE_NAME (priv->loader));
    }

  stream = g_file_read (gfile, cancellable, error);
  if (stream == NULL)
    return FALSE;

  res = _clutter_image_loader_load_stream (priv->loader,
                                           G_INPUT_STREAM (stream),
                                           cancellable,
                                           error);
  if (!res)
    {
      g_object_unref (stream);
      return FALSE;
    }

  _clutter_image_loader_get_image_size (priv->loader,
                                        &priv->image_width,
                                        &priv->image_height);

  texture = _clutter_image_loader_get_texture_handle (priv->loader);
  if (texture == COGL_INVALID_HANDLE)
    {
      g_object_unref (stream);
      return FALSE;
    }

  cogl_material_set_layer (priv->material, 0, texture);

  g_signal_emit (image, image_signals[SIZE_CHANGED], 0,
                 priv->image_width,
                 priv->image_height);

  if (width)
    *width = priv->image_width;

  if (height)
    *height = priv->image_height;

  g_object_unref (priv->loader);
  priv->loader = NULL;

  g_object_unref (stream);

  return TRUE;
}

void
clutter_image_load_async (ClutterImage        *image,
                          GFile               *gfile,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  g_return_if_fail (CLUTTER_IS_IMAGE (image));
  g_return_if_fail (G_IS_FILE (gfile));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail (callback != NULL);
}

gboolean
clutter_image_load_finish (ClutterImage  *image,
                           GAsyncResult  *res,
                           gint          *width,
                           gint          *height,
                           GError       **error)
{
  g_return_val_if_fail (CLUTTER_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);

  return TRUE;
}

GQuark
clutter_image_error_quark (void)
{
  return g_quark_from_static_string ("clutter-image-error-quark");
}
