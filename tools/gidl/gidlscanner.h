#ifndef __G_IDL_SCANNER_H__
#define __G_IDL_SCANNER_H__

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _GidlScanner     GidlScanner;

typedef struct _GidlNode        GidlNode;
typedef struct _GidlModule      GidlModule;
typedef struct _GidlInterface   GidlInterface;
typedef struct _GidlProperty    GidlProperty;
typedef struct _GidlSignal      GidlSignal;
typedef struct _GidlMethod      GidlMethod;
typedef struct _GidlAnnotation  GidlAnnotation;
typedef struct _GidlParameter   GidlParameter;
typedef struct _GidlConst       GidlConst;
typedef struct _GidlEnum        GidlEnum;
typedef struct _GidlErrorDomain GidlErrorDomain;
typedef struct _GidlStruct      GidlStruct;
typedef struct _GidlUnion       GidlUnion;
typedef struct _GidlMember      GidlMember;
typedef struct _GidlValue       GidlValue;

typedef enum {
  G_IDL_INVALID_NODE,

  G_IDL_MODULE,
  G_IDL_INTERFACE,
  G_IDL_PROPERTY,
  G_IDL_SIGNAL,
  G_IDL_METHOD,
  G_IDL_ANNOTATION,
  G_IDL_PARAMETER,
  G_IDL_CONST,
  G_IDL_ENUM,
  G_IDL_ERROR_DOMAIN,
  G_IDL_STRUCT,
  G_IDL_UNION,
  G_IDL_MEMBER,
  G_IDL_VALUE
} GidlNodeType;

typedef enum {
  G_IDL_PROPERTY_READONLY,
  G_IDL_PROPERTY_WRITEONLY,
  G_IDL_PROPERTY_READWRITE
} GidlPropertyVisibility;

typedef enum {
  G_IDL_PROPERTY_ASSIGN,
  G_IDL_PROPERTY_COPY,
  G_IDL_PROPERTY_REFERENCE
} GidlPropertyMemory;

typedef enum {
  G_IDL_DIRECTION_IN,
  G_IDL_DIRECTION_OUT,
  G_IDL_DIRECTION_INOUT
} GidlParameterDirection;

GidlScanner *   g_idl_scanner_alloc             (void);
GidlScanner *   g_idl_scanner_init_with_file    (GidlScanner  *scanner,
                                                 const char   *filename);
GidlScanner *   g_idl_scanner_init_with_data    (GidlScanner  *scanner,
                                                 const char   *data,
                                                 gssize        length);
GidlScanner *   g_idl_scanner_init_with_stream  (GidlScanner  *scanner,
                                                 GInputStream *stream);
GidlScanner *   g_idl_scanner_free              (GidlScanner  *scanner);

gboolean        g_idl_scanner_parse             (GidlScanner  *scanner,
                                                 GCancellable *cancellable,
                                                 GError      **error);
void            g_idl_scanner_parse_async       (GidlScanner  *scanner,
                                                 GCancellable *cancellable,
                                                 GAsyncReadyCallback callback,
                                                 gpointer data);
gboolean        g_idl_scanner_parse_finish      (GidlScanner *scanner,
                                                 GAsyncResult *result,
                                                 GError **error);

GidlNodeType    g_idl_node_type                 (const GidlNode *node);
GidlNode *      g_idl_node_ref                  (GidlNode       *node);
void            g_idl_node_unref                (GidlNode       *node);

G_GNUC_INTERNAL
void            g_idl_scanner_add_node          (GidlScanner    *scanner,
                                                 GidlNode       *node);

G_END_DECLS

#endif /* __G_IDL_SCANNER_H__ */
