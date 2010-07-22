#include "extensions.h"

#include <telepathy-glib/proxy-subclass.h>

static void _miniature_ext_register_dbus_glib_marshallers (void);

/* include auto-generated stubs for client-specific code */
#include "_gen/signals-marshal.h"
#include "_gen/cli-misc-body.h"
#include "_gen/register-dbus-glib-marshallers-body.h"

static gpointer
miniature_cli_once (gpointer data)
{
  _miniature_ext_register_dbus_glib_marshallers ();

  tp_proxy_init_known_interfaces ();

  tp_proxy_or_subclass_hook_on_interface_add (TP_TYPE_PROXY,
      miniature_cli_misc_add_signals);

  return NULL;
}

void
miniature_cli_init (void)
{
  static GOnce once = G_ONCE_INIT;

  g_once (&once, miniature_cli_once, NULL);
}