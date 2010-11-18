#ifndef __CLUTTER_CONTENT_PRIVATE_H__
#define __CLUTTER_CONTENT_PRIVATE_H__

#include <clutter/clutter-content.h>

G_BEGIN_DECLS

void            _clutter_content_add_actor      (ClutterContent     *content,
                                                 ClutterActor       *actor);
void            _clutter_content_remove_actor   (ClutterContent     *content,
                                                 ClutterActor       *actor);
GHashTable *    _clutter_content_get_actors     (ClutterContent     *content);

void            _clutter_content_pick_content   (ClutterContent     *content,
                                                 ClutterActor       *actor,
                                                 const ClutterColor *pick_color);

G_END_DECLS

#endif /* __CLUTTER_CONTENT_PRIVATE_H__ */
