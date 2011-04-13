/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2010  Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */
#if !defined(__CLUTTER_H_INSIDE__) && !defined(CLUTTER_COMPILATION)
#error "Only <clutter/clutter.h> can be included directly."
#endif

#ifndef __CLUTTER_DRAWING_AREA_H__
#define __CLUTTER_DRAWING_AREA_H__

#include <cairo.h>
#include <clutter/clutter-actor.h>
#include <clutter/clutter-content.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_DRAWING_AREA               (clutter_drawing_area_get_type ())
#define CLUTTER_DRAWING_AREA(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_DRAWING_AREA, ClutterDrawingArea))
#define CLUTTER_IS_DRAWING_AREA(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_DRAWING_AREA))
#define CLUTTER_DRAWING_AREA_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_DRAWING_AREA, ClutterDrawingAreaClass))
#define CLUTTER_IS_DRAWING_AREA_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_DRAWING_AREA))
#define CLUTTER_DRAWING_AREA_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_DRAWING_AREA, ClutterDrawingAreaClass))

typedef struct _ClutterDrawingArea              ClutterDrawingArea;
typedef struct _ClutterDrawingAreaPrivate       ClutterDrawingAreaPrivate;
typedef struct _ClutterDrawingAreaClass         ClutterDrawingAreaClass;

/**
 * ClutterDrawingArea:
 *
 * The <structname>ClutterDrawingArea</structname> structure contains only
 * private data and should be accessed using the provided API.
 *
 * Since: 1.8
 */
struct _ClutterDrawingArea
{
  /*< private >*/
  GObject parent_instance;

  ClutterDrawingAreaPrivate *priv;
};

/**
 * ClutterDrawingAreaClass:
 * @create_surface: class handler for the #ClutterDrawingArea::create_surface
 *   signal
 * @draw: class handler for the #ClutterDrawingArea::draw signal
 *
 * The <structname>ClutterDrawingAreaClass</structname> structure contains
 * only private data.
 *
 * Since: 1.8
 */
struct _ClutterDrawingAreaClass
{
  /*< private >*/
  GObjectClass parent_class;

  /*< public >*/
  cairo_surface_t *(* create_surface) (ClutterDrawingArea *area,
                                       ClutterActor       *actor,
                                       guint               width,
                                       guint               height);

  gboolean         (* draw)           (ClutterDrawingArea *area,
                                       ClutterActor       *actor,
                                       cairo_t            *cr);

  /*< private >*/
  void (*_clutter_drawing_area_0) (void);
  void (*_clutter_drawing_area_1) (void);
  void (*_clutter_drawing_area_2) (void);
  void (*_clutter_drawing_area_3) (void);
  void (*_clutter_drawing_area_4) (void);
  void (*_clutter_drawing_area_5) (void);
  void (*_clutter_drawing_area_6) (void);
  void (*_clutter_drawing_area_7) (void);
};

GType clutter_drawing_area_get_type (void) G_GNUC_CONST;

ClutterContent *clutter_drawing_area_new (void);

G_END_DECLS

#endif /* __CLUTTER_DRAWING_AREA_H__ */
