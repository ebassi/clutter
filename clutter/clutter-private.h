/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
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
 *
 *
 */

#ifndef __CLUTTER_PRIVATE_H__
#define __CLUTTER_PRIVATE_H__

#include <glib.h>

#include <glib/gi18n-lib.h>

#ifdef CLUTTER_USING_SYSTEM_COGL
#include <cogl/cogl-pango.h>
#else
#include "pango/cogl-pango.h"
#endif

#include "clutter-backend.h"
#include "clutter-effect.h"
#include "clutter-event.h"
#include "clutter-feature.h"
#include "clutter-id-pool.h"
#include "clutter-layout-manager.h"
#include "clutter-macros-private.h"
#include "clutter-master-clock.h"
#include "clutter-settings.h"
#include "clutter-stage.h"

G_BEGIN_DECLS

typedef struct _ClutterMainContext      ClutterMainContext;

/* Cairo stores the data in native byte order as ARGB but Cogl's pixel
   formats specify the actual byte order. Therefore we need to use a
   different format depending on the architecture */
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define CLUTTER_CAIRO_PIXEL_FORMAT      (COGL_PIXEL_FORMAT_BGRA_8888_PRE)
#else
#define CLUTTER_CAIRO_PIXEL_FORMAT      (COGL_PIXEL_FORMAT_ARGB_8888_PRE)
#endif

typedef enum {
  CLUTTER_ACTOR_UNUSED_FLAG = 0,

  CLUTTER_IN_DESTRUCTION = 1 << 0,
  CLUTTER_IS_TOPLEVEL    = 1 << 1,
  CLUTTER_IN_REPARENT    = 1 << 2,

  /* Used to avoid recursion */
  CLUTTER_IN_PAINT       = 1 << 3,

  /* Used to avoid recursion */
  CLUTTER_IN_RELAYOUT    = 1 << 4,

  /* Used by the stage if resizing is an asynchronous operation (like on
   * X11) to delay queueing relayouts until we got a notification from the
   * event handling
   */
  CLUTTER_IN_RESIZE      = 1 << 5,

  /* a flag for internal children of Containers */
  CLUTTER_INTERNAL_CHILD = 1 << 6
} ClutterPrivateFlags;

/*
 * ClutterMainContext:
 *
 * The shared state of Clutter
 */
struct _ClutterMainContext
{
  /* the main windowing system backend */
  ClutterBackend *backend;

  /* the main event queue */
  GQueue *events_queue;

  /* timer used to print the FPS count */
  GTimer *timer;

  ClutterPickMode  pick_mode;

  /* mapping between reused integer ids and actors */
  ClutterIDPool *id_pool;

  /* default FPS; this is only used if we cannot sync to vblank */
  guint frame_rate;

  /* actors with a grab on all devices */
  ClutterActor *pointer_grab_actor;
  ClutterActor *keyboard_grab_actor;

  /* stack of actors with shaders during paint */
  GSList *shaders;

  /* fb bit masks for col<->id mapping in picking */
  gint fb_r_mask;
  gint fb_g_mask;
  gint fb_b_mask;
  gint fb_r_mask_used;
  gint fb_g_mask_used;
  gint fb_b_mask_used;

  PangoContext *pango_context;  /* Global Pango context */
  CoglPangoFontMap *font_map;   /* Global font map */

  ClutterEvent *current_event;
  guint32 last_event_time;

  /* list of repaint functions installed through
   * clutter_threads_add_repaint_func()
   */
  GList *repaint_funcs;

  /* main settings singleton */
  ClutterSettings *settings;

  /* boolean flags */
  guint is_initialized          : 1;
  guint motion_events_per_actor : 1;
  guint defer_display_setup     : 1;
  guint options_parsed          : 1;
};

/* shared between clutter-main.c and clutter-frame-source.c */
typedef struct
{
  GSourceFunc func;
  gpointer data;
  GDestroyNotify notify;
} ClutterThreadsDispatch;

gboolean _clutter_threads_dispatch      (gpointer data);
void     _clutter_threads_dispatch_free (gpointer data);

ClutterMainContext *    _clutter_context_get_default            (void);
gboolean                _clutter_context_is_initialized         (void);
PangoContext *          _clutter_context_create_pango_context   (void);
PangoContext *          _clutter_context_get_pango_context      (void);
ClutterPickMode         _clutter_context_get_pick_mode          (void);
void                    _clutter_context_push_shader_stack      (ClutterActor *actor);
ClutterActor *          _clutter_context_pop_shader_stack       (ClutterActor *actor);
ClutterActor *          _clutter_context_peek_shader_stack      (void);
guint32                 _clutter_context_acquire_id             (gpointer      key);
void                    _clutter_context_release_id             (guint32       id_);

G_CONST_RETURN gchar *_clutter_gettext (const gchar *str);

gboolean      _clutter_feature_init (GError **error);

/* Picking code */
guint         _clutter_pixel_to_id (guchar pixel[4]);
void          _clutter_id_to_color (guint id,
                                    ClutterColor *col);
ClutterActor *_clutter_get_actor_by_id (guint32 actor_id);

/* use this function as the accumulator if you have a signal with
 * a G_TYPE_BOOLEAN return value; this will stop the emission as
 * soon as one handler returns TRUE
 */
gboolean _clutter_boolean_handled_accumulator (GSignalInvocationHint *ihint,
                                               GValue                *return_accu,
                                               const GValue          *handler_return,
                                               gpointer               dummy);
gboolean _clutter_create_surface_accumulator (GSignalInvocationHint *ihint,
                                              GValue                *return_accu,
                                              const GValue          *handler_return,
                                              gpointer               dummy);

void _clutter_run_repaint_functions (void);

void _clutter_constraint_update_allocation (ClutterConstraint *constraint,
                                            ClutterActor      *actor,
                                            ClutterActorBox   *allocation);

GType _clutter_layout_manager_get_child_meta_type (ClutterLayoutManager *manager);

void  _clutter_util_fully_transform_vertices (const CoglMatrix    *modelview,
                                              const CoglMatrix    *projection,
                                              const float         *viewport,
                                              const ClutterVertex *vertices_in,
                                              ClutterVertex       *vertices_out,
                                              int                  n_vertices);

typedef struct _ClutterPlane
{
  CoglVector3 v0;
  CoglVector3 n;
} ClutterPlane;

typedef enum _ClutterCullResult
{
  CLUTTER_CULL_RESULT_UNKNOWN,
  CLUTTER_CULL_RESULT_IN,
  CLUTTER_CULL_RESULT_OUT,
  CLUTTER_CULL_RESULT_PARTIAL
} ClutterCullResult;

void _clutter_io_modules_ensure_loaded (void);

G_END_DECLS

#endif /* __CLUTTER_PRIVATE_H__ */
