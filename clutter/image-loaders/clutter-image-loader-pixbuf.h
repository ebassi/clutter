#ifndef __CLUTTER_IMAGE_LOADER_PIXBUF_H__
#define __CLUTTER_IMAGE_LOADER_PIXBUF_H__

#include <clutter/clutter-image-loader.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_IMAGE_LOADER_PIXBUF        (_clutter_image_loader_pixbuf_get_type ())
#define CLUTTER_IMAGE_LOADER_PIXBUF(obj)        (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_IMAGE_LOADER_PIXBUF, ClutterImageLoaderPixbuf))
#define CLUTTER_IS_IMAGE_LOADER_PIXBUF(obj)     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_IMAGE_LOADER_PIXBUF))

typedef struct _ClutterImageLoaderPixbuf        ClutterImageLoaderPixbuf;
typedef struct _ClutterImageLoaderClass         ClutterImageLoaderPixbufClass;

GType _clutter_image_loader_pixbuf_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __CLUTTER_IMAGE_LOADER_PIXBUF_H__ */
