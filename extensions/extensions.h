#ifndef __MINIATURE_EXTENSIONS_H__
#define __MINIATURE_EXTENSIONS_H__

#include <glib-object.h>
#include <telepathy-glib/channel.h>

#include "extensions/_gen/enums.h"
#include "extensions/_gen/cli-misc.h"
#include "extensions/_gen/svc-misc.h"

G_BEGIN_DECLS

#include "extensions/_gen/gtypes.h"
#include "extensions/_gen/interfaces.h"

void miniature_cli_init (void);

G_END_DECLS

#endif
