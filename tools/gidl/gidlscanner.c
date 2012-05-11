#include "config.h"

#include "gidlscanner.h"

#include <stdlib.h>
#include <string.h>

struct _GidlScanner
{
  char *filename;
  GInputStream *stream;

  GidlNode *root;
};

struct _GidlNode
{
  GidlNodeType type;

  int ref_count;
};

struct _GidlModule
{
  GidlNode _parent;

  char *name;

  GPtrArray *annotations;
};

struct _GidlInterface
{
  GidlNode _parent;

  char *scoped_name;
  char **decomposed_name;

  GHashTable *inherits;
  GHashTable *annotations;
  GHashTable *properties;
  GHashTable *signals;
  GHashTable *methods;
};

struct _GidlProperty
{
  GidlNode _parent;

  char *name;

  GidlNode *type;

  GidlPropertyVisibility visibility;
  GidlPropertyMemory memory;

  GHashTable *annotations;
};

struct _GidlMethod
{
  GidlNode _parent;

  char *name;

  GidlNode *retval;
  GHashTable *parameters;
  GHashTable *annotations;
  GHashTable *exceptions;
};
