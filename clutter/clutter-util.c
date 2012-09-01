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

/**
 * SECTION:clutter-util
 * @short_description: Utility functions
 *
 * Various miscellaneous utilility functions.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

#include <glib/gi18n-lib.h>

#include "clutter-main.h"
#include "clutter-interval.h"
#include "clutter-private.h"

#include "deprecated/clutter-util.h"

/**
 * clutter_util_next_p2:
 * @a: Value to get the next power
 *
 * Calculates the nearest power of two, greater than or equal to @a.
 *
 * Return value: The nearest power of two, greater or equal to @a.
 *
 * Deprecated: 1.2
 */
gint
clutter_util_next_p2 (gint a)
{
  int rval = 1;

  while (rval < a)
    rval <<= 1;

  return rval;
}

/*< private >
 * _clutter_gettext:
 * @str: a string to localize
 *
 * Retrieves the localized version of @str, using the Clutter domain
 *
 * Return value: the translated string
 */
const gchar *
_clutter_gettext (const gchar *str)
{
  return g_dgettext (GETTEXT_PACKAGE, str);
}

/* Help macros to scale from OpenGL <-1,1> coordinates system to
 * window coordinates ranging [0,window-size]
 */
#define MTX_GL_SCALE_X(x,w,v1,v2) ((((((x) / (w)) + 1.0f) / 2.0f) * (v1)) + (v2))
#define MTX_GL_SCALE_Y(y,w,v1,v2) ((v1) - (((((y) / (w)) + 1.0f) / 2.0f) * (v1)) + (v2))
#define MTX_GL_SCALE_Z(z,w,v1,v2) (MTX_GL_SCALE_X ((z), (w), (v1), (v2)))

void
_clutter_util_fully_transform_vertices (const CoglMatrix *modelview,
                                        const CoglMatrix *projection,
                                        const float *viewport,
                                        const ClutterVertex *vertices_in,
                                        ClutterVertex *vertices_out,
                                        int n_vertices)
{
  CoglMatrix modelview_projection;
  ClutterVertex4 *vertices_tmp;
  int i;

  vertices_tmp = g_alloca (sizeof (ClutterVertex4) * n_vertices);

  if (n_vertices >= 4)
    {
      /* XXX: we should find a way to cache this per actor */
      cogl_matrix_multiply (&modelview_projection,
                            projection,
                            modelview);
      cogl_matrix_project_points (&modelview_projection,
                                  3,
                                  sizeof (ClutterVertex),
                                  vertices_in,
                                  sizeof (ClutterVertex4),
                                  vertices_tmp,
                                  n_vertices);
    }
  else
    {
      cogl_matrix_transform_points (modelview,
                                    3,
                                    sizeof (ClutterVertex),
                                    vertices_in,
                                    sizeof (ClutterVertex4),
                                    vertices_tmp,
                                    n_vertices);

      cogl_matrix_project_points (projection,
                                  3,
                                  sizeof (ClutterVertex4),
                                  vertices_tmp,
                                  sizeof (ClutterVertex4),
                                  vertices_tmp,
                                  n_vertices);
    }

  for (i = 0; i < n_vertices; i++)
    {
      ClutterVertex4 vertex_tmp = vertices_tmp[i];
      ClutterVertex *vertex_out = &vertices_out[i];
      /* Finally translate from OpenGL coords to window coords */
      vertex_out->x = MTX_GL_SCALE_X (vertex_tmp.x, vertex_tmp.w,
                                      viewport[2], viewport[0]);
      vertex_out->y = MTX_GL_SCALE_Y (vertex_tmp.y, vertex_tmp.w,
                                      viewport[3], viewport[1]);
    }
}

/*< private >
 * _clutter_util_rectangle_union:
 * @src1: first rectangle to union
 * @src2: second rectangle to union
 * @dest: (out): return location for the unioned rectangle
 *
 * Calculates the union of two rectangles.
 *
 * The union of rectangles @src1 and @src2 is the smallest rectangle which
 * includes both @src1 and @src2 within it.
 *
 * It is allowed for @dest to be the same as either @src1 or @src2.
 *
 * This function should really be in Cairo.
 */
void
_clutter_util_rectangle_union (const cairo_rectangle_int_t *src1,
                               const cairo_rectangle_int_t *src2,
                               cairo_rectangle_int_t       *dest)
{
  int dest_x, dest_y;

  dest_x = MIN (src1->x, src2->x);
  dest_y = MIN (src1->y, src2->y);

  dest->width = MAX (src1->x + src1->width, src2->x + src2->width) - dest_x;
  dest->height = MAX (src1->y + src1->height, src2->y + src2->height) - dest_y;
  dest->x = dest_x;
  dest->y = dest_y;
}

static void
_clutter_util_matrix_transpose_vector4_transform (const ClutterMatrix  *matrix,
                                                  const ClutterVertex4 *point,
                                                  ClutterVertex4       *res)
{
  res->x = matrix->xx * point->x
         + matrix->xy * point->y
         + matrix->xz * point->z
         + matrix->xw * point->w;

  res->y = matrix->yx * point->x
         + matrix->yy * point->y
         + matrix->yz * point->z
         + matrix->yw * point->w;

  res->z = matrix->zx * point->x
         + matrix->zy * point->y
         + matrix->zz * point->z
         + matrix->zw * point->w;

  res->w = matrix->wz * point->x
         + matrix->wy * point->w
         + matrix->wz * point->z
         + matrix->ww * point->w;
}

static void
_clutter_util_vertex_combine (const ClutterVertex *a,
                              const ClutterVertex *b,
                              double               ascl,
                              double               bscl,
                              ClutterVertex       *res)
{
  res->x = (ascl * a->x) + (bscl * b->x);
  res->y = (ascl * a->y) + (bscl * b->y);
  res->z = (ascl * a->z) + (bscl * b->z);
}

void
_clutter_util_vertex4_interpolate (const ClutterVertex4 *a,
                                   const ClutterVertex4 *b,
                                   double                progress,
                                   ClutterVertex4       *res)
{
  res->x = a->x + (b->x - a->x) * progress;
  res->y = a->y + (b->y - a->y) * progress;
  res->z = a->z + (b->z - a->z) * progress;
  res->w = a->w + (b->w - a->w) * progress;
}

/*< private >
 * clutter_util_matrix_decompose:
 * @src: the matrix to decompose
 * @scale_p: (out caller-allocates): return location for a vertex containing
 *   the scaling factors
 * @shear_p: (out) (array length=3): return location for an array of 3
 *   elements containing the skew factors (XY, XZ, and YZ respectively)
 * @rotate_p: (out caller-allocates): return location for a vertex containing
 *   the Euler angles
 * @translate_p: (out caller-allocates): return location for a vertex
 *   containing the translation vector
 * @perspective_p: (out caller-allocates: return location for a 4D vertex
 *   containing the perspective
 *
 * Decomposes a #ClutterMatrix into the transformations that compose it.
 *
 * This code is based on the matrix decomposition algorithm as published in
 * the CSS Transforms specification by the W3C CSS working group, available
 * at http://www.w3.org/TR/css3-transforms/.
 *
 * The algorithm, in turn, is based on the "unmatrix" method published in
 * "Graphics Gems II, edited by Jim Arvo", which is available at:
 * http://tog.acm.org/resources/GraphicsGems/gemsii/unmatrix.c
 *
 * Return value: %TRUE if the decomposition was successful, and %FALSE
 *   if the matrix is singular
 */
gboolean
_clutter_util_matrix_decompose (const ClutterMatrix *src,
                                ClutterVertex       *scale_p,
                                float                shear_p[3],
                                ClutterVertex       *rotate_p,
                                ClutterVertex       *translate_p,
                                ClutterVertex4      *perspective_p)
{
  CoglMatrix matrix = *src;
  CoglMatrix perspective;
  ClutterVertex4 vertex_tmp;
  ClutterVertex row[3], pdum;
  int i, j;

#define XY_SHEAR        0
#define XZ_SHEAR        1
#define YZ_SHEAR        2
#define MAT(m,r,c)      ((float *)(m))[(c) * 4 + (r)]

  /* normalize the matrix */
  if (matrix.ww == 0.f)
    return FALSE;

  for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 4; j++)
        {
          MAT (&matrix, i, j) /= MAT (&matrix, 3, 3);
        }
    }

  /* perspective is used to solve for perspective, but it also provides
   * an easy way to test for singularity of the upper 3x3 component
   */
  perspective = matrix;

  /* transpose */
  MAT (&perspective, 0, 3) = 0.f;
  MAT (&perspective, 1, 3) = 0.f;
  MAT (&perspective, 2, 3) = 0.f;
  MAT (&perspective, 3, 3) = 1.f;

  if (clutter_matrix_determinant (&perspective) == 0.f)
    return FALSE;

  if (MAT (&matrix, 0, 3) != 0.f ||
      MAT (&matrix, 1, 3) != 0.f ||
      MAT (&matrix, 2, 3) != 0.f)
    {
      CoglMatrix perspective_inv;
      ClutterVertex4 p;

      vertex_tmp.x = MAT (&matrix, 0, 3);
      vertex_tmp.y = MAT (&matrix, 1, 3);
      vertex_tmp.z = MAT (&matrix, 2, 3);
      vertex_tmp.w = MAT (&matrix, 3, 3);

      /* solve the equation by inverting perspective... */
      cogl_matrix_get_inverse (&perspective, &perspective_inv);

      /* ... and multiplying vertex_tmp by the inverse */
      _clutter_util_matrix_transpose_vector4_transform (&perspective_inv,
                                                        &vertex_tmp,
                                                        &p);

      *perspective_p = p;

      /* clear the perspective part */
      MAT (&matrix, 0, 3) = 0.0f;
      MAT (&matrix, 1, 3) = 0.0f;
      MAT (&matrix, 2, 3) = 0.0f;
      MAT (&matrix, 3, 3) = 1.0f;
    }
  else
    {
      /* no perspective */
      perspective_p->x = 0.0f;
      perspective_p->y = 0.0f;
      perspective_p->z = 0.0f;
      perspective_p->w = 1.0f;
    }

  /* translation */
  translate_p->x = MAT (&matrix, 3, 0);
  MAT (&matrix, 3, 0) = 0.f;
  translate_p->y = MAT (&matrix, 3, 1);
  MAT (&matrix, 3, 1) = 0.f;
  translate_p->z = MAT (&matrix, 3, 2);
  MAT (&matrix, 3, 2) = 0.f;

  /* scale and shear; we split the upper 3x3 matrix into rows */
  for (i = 0; i < 3; i++)
    {
      row[i].x = MAT (&matrix, i, 0);
      row[i].y = MAT (&matrix, i, 1);
      row[i].z = MAT (&matrix, i, 2);
    }

  /* compute scale.x and normalize the first row */
  scale_p->x = clutter_vertex_length (&row[0]);
  clutter_vertex_normalize (&row[0]);

  /* compute XY shear and make the second row orthogonal to the first */
  shear_p[XY_SHEAR] = clutter_vertex_dot (&row[0], &row[1]);
  _clutter_util_vertex_combine (&row[1], &row[0],
                                1.0, -shear_p[XY_SHEAR],
                                &row[1]);

  /* compute the Y scale and normalize the second row */
  scale_p->y = clutter_vertex_length (&row[1]);
  clutter_vertex_normalize (&row[1]);
  shear_p[XY_SHEAR] /= scale_p->y;

  /* compute XZ and YZ shears, orthogonalize the third row */
  shear_p[XZ_SHEAR] = clutter_vertex_dot (&row[0], &row[2]);
  _clutter_util_vertex_combine (&row[2], &row[0],
                                1.0, -shear_p[XZ_SHEAR],
                                &row[2]);

  shear_p[YZ_SHEAR] = clutter_vertex_dot (&row[1], &row[2]);
  _clutter_util_vertex_combine (&row[2], &row[1],
                                1.0, -shear_p[YZ_SHEAR],
                                &row[2]);

  /* get the Z scale and normalize the third row*/
  scale_p->z = clutter_vertex_dot (&row[0], &row[2]);
  clutter_vertex_normalize (&row[2]);
  shear_p[XZ_SHEAR] /= scale_p->z;
  shear_p[YZ_SHEAR] /= scale_p->z;

  /* at this point, the matrix (inside row[]) is orthonormal.
   * check for a coordinate system flip; if the determinant
   * is -1, then negate the matrix and scaling factors
   */
  clutter_vertex_cross (&row[1], &row[2], &pdum);
  if (clutter_vertex_dot (&row[0], &pdum) < 0.f)
    {
      scale_p->x *= -1.f;

      for (i = 0; i < 3; i++)
        {
          row[i].x *= -1.f;
          row[i].y *= -1.f;
          row[i].z *= -1.f;
        }
    }

  /* now get the rotations out */
  rotate_p->y = asinf (-row[0].z);
  if (cosf (rotate_p->y) != 0.f)
    {
      rotate_p->x = atan2f (row[1].z, row[2].z);
      rotate_p->z = atan2f (row[0].y, row[0].x);
    }
  else
    {
      rotate_p->x = atan2f (-row[2].x, row[1].y);
      rotate_p->z = 0.f;
    }

#undef XY_SHEAR
#undef XZ_SHEAR
#undef YZ_SHEAR
#undef MAT

  return TRUE;
}

typedef struct
{
  GType value_type;
  ClutterProgressFunc func;
} ProgressData;

G_LOCK_DEFINE_STATIC (progress_funcs);
static GHashTable *progress_funcs = NULL;

gboolean
_clutter_has_progress_function (GType gtype)
{
  const char *type_name = g_type_name (gtype);

  if (progress_funcs == NULL)
    return FALSE;

  return g_hash_table_lookup (progress_funcs, type_name) != NULL;
}

gboolean
_clutter_run_progress_function (GType gtype,
                                const GValue *initial,
                                const GValue *final,
                                gdouble progress,
                                GValue *retval)
{
  ProgressData *pdata;
  gboolean res;

  G_LOCK (progress_funcs);

  if (G_UNLIKELY (progress_funcs == NULL))
    {
      res = FALSE;
      goto out;
    }

  pdata = g_hash_table_lookup (progress_funcs, g_type_name (gtype));
  if (G_UNLIKELY (pdata == NULL))
    {
      res = FALSE;
      goto out;
    }

  res = pdata->func (initial, final, progress, retval);

out:
  G_UNLOCK (progress_funcs);

  return res;
}

static void
progress_data_destroy (gpointer data_)
{
  g_slice_free (ProgressData, data_);
}

/**
 * clutter_interval_register_progress_func: (skip)
 * @value_type: a #GType
 * @func: a #ClutterProgressFunc, or %NULL to unset a previously
 *   set progress function
 *
 * Sets the progress function for a given @value_type, like:
 *
 * |[
 *   clutter_interval_register_progress_func (MY_TYPE_FOO,
 *                                            my_foo_progress);
 * ]|
 *
 * Whenever a #ClutterInterval instance using the default
 * #ClutterInterval::compute_value implementation is set as an
 * interval between two #GValue of type @value_type, it will call
 * @func to establish the value depending on the given progress,
 * for instance:
 *
 * |[
 *   static gboolean
 *   my_int_progress (const GValue *a,
 *                    const GValue *b,
 *                    gdouble       progress,
 *                    GValue       *retval)
 *   {
 *     gint ia = g_value_get_int (a);
 *     gint ib = g_value_get_int (b);
 *     gint res = factor * (ib - ia) + ia;
 *
 *     g_value_set_int (retval, res);
 *
 *     return TRUE;
 *   }
 *
 *   clutter_interval_register_progress_func (G_TYPE_INT, my_int_progress);
 * ]|
 *
 * To unset a previously set progress function of a #GType, pass %NULL
 * for @func.
 *
 * Since: 1.0
 */
void
clutter_interval_register_progress_func (GType               value_type,
                                         ClutterProgressFunc func)
{
  ProgressData *progress_func;
  const char *type_name;

  g_return_if_fail (value_type != G_TYPE_INVALID);

  type_name = g_type_name (value_type);

  G_LOCK (progress_funcs);

  if (G_UNLIKELY (progress_funcs == NULL))
    progress_funcs = g_hash_table_new_full (NULL, NULL,
                                            NULL,
                                            progress_data_destroy);

  progress_func =
    g_hash_table_lookup (progress_funcs, type_name);

  if (G_UNLIKELY (progress_func))
    {
      if (func == NULL)
        {
          g_hash_table_remove (progress_funcs, type_name);
          g_slice_free (ProgressData, progress_func);
        }
      else
        progress_func->func = func;
    }
  else
    {
      progress_func = g_slice_new (ProgressData);
      progress_func->value_type = value_type;
      progress_func->func = func;

      g_hash_table_replace (progress_funcs,
                            (gpointer) type_name,
                            progress_func);
    }

  G_UNLOCK (progress_funcs);
}
