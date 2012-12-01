#ifndef __CLUTTER_LAYOUT_MANAGER_DEPRECATED_H__
#define __CLUTTER_LAYOUT_MANAGER_DEPRECATED_H__

#include <clutter/clutter-layout-manager.h>

G_BEGIN_DECLS

CLUTTER_DEPRECATED_IN_1_14
void            clutter_layout_manager_allocate                 (ClutterLayoutManager   *manager,
                                                                 ClutterContainer       *container,
                                                                 const ClutterActorBox  *allocation,
                                                                 ClutterAllocationFlags  flags);
CLUTTER_DEPRECATED_IN_1_12
ClutterAlpha *  clutter_layout_manager_begin_animation          (ClutterLayoutManager   *manager,
                                                                 guint                   duration,
                                                                 gulong                  mode);
CLUTTER_DEPRECATED_IN_1_12
void            clutter_layout_manager_end_animation            (ClutterLayoutManager   *manager);
CLUTTER_DEPRECATED_IN_1_12
gdouble         clutter_layout_manager_get_animation_progress   (ClutterLayoutManager   *manager);


G_END_DECLS

#endif /* __CLUTTER_LAYOUT_MANAGER_DEPRECATED_H__ */
