#ifndef __CLUTTER_IMAGE_LOADER_NULL_H__
#define __CLUTTER_IMAGE_LOADER_NULL_H__

#include <clutter/clutter-image-loader.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_IMAGE_LOADER_NULL          (_clutter_image_loader_null_get_type ())
#define CLUTTER_IMAGE_LOADER_NULL(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_IMAGE_LOADER_NULL, ClutterImageLoaderNull))
#define CLUTTER_IS_IMAGE_LOADER_NULL(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_IMAGE_LOADER_NULL))

typedef struct _ClutterImageLoaderNull          ClutterImageLoaderNull;
typedef struct _ClutterImageLoaderClass         ClutterImageLoaderNullClass;

GType _clutter_image_loader_null_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __CLUTTER_IMAGE_LOADER_NULL_H__ */
