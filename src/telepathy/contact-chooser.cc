/*
 * contact-chooser.c
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <glib.h>

#include <telepathy-glib/dbus.h>
#include <telepathy-glib/interfaces.h>
#include <telepathy-glib/util.h>
#include <telepathy-glib/proxy-subclass.h>
#include <telepathy-glib/handle.h>
#include <telepathy-glib/connection.h>

#include "contact-chooser.h"

#define GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MINIATURE_TYPE_CONTACT_CHOOSER, \
      MiniatureContactChooserPrivate))

G_DEFINE_TYPE (MiniatureContactChooser, miniature_contact_chooser,
    OSSO_ABOOK_TYPE_CONTACT_VIEW)

/* This widget doesn't handle:
 *  1. accounts going offline
 *  2. accounts coming online
 *  3. contacts going offline
 *  4. contacts going online
 *  5. new contacts appearing
 * (6. custom requestable channel classes)
 *
 * As the model doesn't persist, however, closing and re-opening the dialog
 * will fix any problems. These should of course be fixed in the long term.
 */

enum /* signals */
{
  DONE,
  WORKING,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0, };

typedef struct _MiniatureContactChooserPrivate MiniatureContactChooserPrivate;
struct _MiniatureContactChooserPrivate
{
  /* (owned) gchar* -> (owned) GHashTable: TpHandle -> gboolean */
  GHashTable *handle_capable;

  /* (reffed) McAccount -> (owned) GArray: TpHandle */
  GHashTable *further_calls;
  guint further_calls_timeout_id;

  gboolean made_all_calls;
  guint calls_in_progress;

  TpDBusDaemon *dbus;
};

typedef struct {
    GObject *instance;
    GObject *user_data;
    gulong handler_id;
} WeakHandlerCtx;

static WeakHandlerCtx *
whc_new (GObject *instance,
    GObject *user_data)
{
  WeakHandlerCtx *ctx = g_slice_new0 (WeakHandlerCtx);

  ctx->instance = instance;
  ctx->user_data = user_data;

  return ctx;
}

static void
whc_free (WeakHandlerCtx *ctx)
{
  g_slice_free (WeakHandlerCtx, ctx);
}

static void user_data_destroyed_cb (gpointer, GObject *);

static void
instance_destroyed_cb (gpointer ctx_,
                       GObject *where_the_instance_was G_GNUC_UNUSED)
{
  WeakHandlerCtx *ctx = (WeakHandlerCtx *) ctx_;

  g_debug ("instance for %p destroyed; cleaning up", ctx);

  /* No need to disconnect the signal here, the instance has gone away. */
  g_object_weak_unref (ctx->user_data, user_data_destroyed_cb, ctx);
  whc_free (ctx);
}

static void
user_data_destroyed_cb (gpointer ctx_,
    GObject *where_the_user_data_was G_GNUC_UNUSED)
{
  WeakHandlerCtx *ctx = (WeakHandlerCtx *) ctx_;

  g_debug ("user_data for %p destroyed; disconnecting", ctx);

  g_signal_handler_disconnect (ctx->instance, ctx->handler_id);
  g_object_weak_unref (ctx->instance, instance_destroyed_cb, ctx);
  whc_free (ctx);
}

static void
miniature_signal_connect_weak (gpointer instance,
    const gchar *detailed_signal,
    GCallback c_handler,
    GObject *user_data)
{
  GObject *instance_obj = G_OBJECT (instance);
  WeakHandlerCtx *ctx = whc_new (instance_obj, user_data);

  g_debug ("connecting to %p:%s with context %p", instance, detailed_signal, ctx);

  ctx->handler_id = g_signal_connect (instance, detailed_signal, c_handler,
      user_data);

  g_object_weak_ref (instance_obj, instance_destroyed_cb, ctx);
  g_object_weak_ref (user_data, user_data_destroyed_cb, ctx);
}

static void
miniature_contact_chooser_finalize (GObject *self)
{
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);

  if (priv->handle_capable != NULL)
    {
      g_hash_table_destroy (priv->handle_capable);
      priv->handle_capable = NULL;
    }

  if (priv->further_calls_timeout_id > 0)
    {
      g_source_remove (priv->further_calls_timeout_id);
      priv->further_calls_timeout_id = 0;
    }

  if (priv->further_calls != NULL)
    {
      g_hash_table_destroy (priv->further_calls);
      priv->further_calls = NULL;
    }

  if (G_OBJECT_CLASS (miniature_contact_chooser_parent_class)->finalize)
    G_OBJECT_CLASS (miniature_contact_chooser_parent_class)->finalize (self);
}

static void
miniature_contact_chooser_dispose (GObject *self)
{
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);

  if (priv->dbus != NULL)
    {
      g_object_unref (priv->dbus);
      priv->dbus = NULL;
    }

  if (G_OBJECT_CLASS (miniature_contact_chooser_parent_class)->dispose)
    G_OBJECT_CLASS (miniature_contact_chooser_parent_class)->dispose (self);
}

static void
check_all_calls_made (MiniatureContactChooser *self)
{
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);

  if (!priv->made_all_calls || priv->calls_in_progress != 0)
    return;

  g_signal_emit (self, signals[DONE], 0);
}

static void
free_garray (gpointer data)
{
  g_array_free ((GArray *) data, TRUE);
}

/* https://bugzilla.gnome.org/show_bug.cgi?id=65987 */
static gchar *
strreplace (const gchar *string,
    const gchar *search,
    const gchar *replacement)
{
  gchar *str, **arr;

  if (replacement == NULL)
    replacement = "";

  arr = g_strsplit (string, search, -1);
  if (arr != NULL && arr[0] != NULL)
    str = g_strjoinv (replacement, arr);
  else
    str = g_strdup (string);

  g_strfreev (arr);

  return str;
}

static void
get_contact_caps_cb (TpConnection *proxy,
    GHashTable *contact_caps,
    const GError *error,
    gpointer user_data,
    GObject *weak_object)
{
  MiniatureContactChooser *chooser = MINIATURE_CONTACT_CHOOSER (weak_object);
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (chooser);
  GHashTable *contact_ft_capabilites;
  GHashTableIter iter;
  gpointer key, value;
  gboolean first_time = FALSE;
  const gchar *account_object_path = (const gchar *) user_data;

  priv->calls_in_progress--;

  if (error != NULL)
    {
      g_debug ("error: %s", error->message);
      return;
    }

  if (contact_caps == NULL)
    {
      g_debug ("contact capabilities hash table is NULL");
      return;
    }

  contact_ft_capabilites = (GHashTable *) g_hash_table_lookup (priv->handle_capable,
      account_object_path);

  if (contact_ft_capabilites == NULL)
    {
      first_time = TRUE;
      contact_ft_capabilites = g_hash_table_new (g_direct_hash, g_direct_equal);
    }

  g_hash_table_iter_init (&iter, contact_caps);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      guint handle = GPOINTER_TO_UINT (key);
      GPtrArray *classes = (GPtrArray *) value;
      gboolean out = FALSE;
      guint i;

      for (i = 0; i < classes->len; i++)
        {
          GHashTable *class_;
          GHashTableIter class_iter;
          gpointer class_key, class_value;
          gboolean class_channeltype = FALSE;
          gboolean class_service = FALSE;

          GValueArray *array;

          array = (GValueArray *) g_ptr_array_index (classes, i);
          class_ = (GHashTable *) g_value_get_boxed (g_value_array_get_nth (array, 0));

          g_hash_table_iter_init (&class_iter, class_);
          while (g_hash_table_iter_next (&class_iter, &class_key, &class_value))
            {
              if (!tp_strdiff ((const gchar *) class_key,
                      TP_IFACE_CHANNEL ".ChannelType")
                  && !tp_strdiff (g_value_get_string ((GValue *) class_value),
                      TP_IFACE_CHANNEL_TYPE_STREAM_TUBE))
                {
                  class_channeltype = TRUE;
                }
              else if (!tp_strdiff ((const gchar *) class_key,
                      TP_IFACE_CHANNEL_TYPE_STREAM_TUBE ".Service")
                  && !tp_strdiff (g_value_get_string ((GValue *) class_value),
                      "Miniature"))
                {
                  class_service = TRUE;
                }
            }
          if (class_channeltype && class_service)
            {
              out = TRUE;
              goto superbreak;
            }
        }

superbreak:
      g_debug ("Contact %u on connection %p has%s tube capabilites",
          handle, proxy, out ? "" : " no");

      g_hash_table_insert (contact_ft_capabilites, GUINT_TO_POINTER (handle),
          GINT_TO_POINTER (out));
    }

  if (first_time)
    {
      g_hash_table_insert (priv->handle_capable,
          g_strdup (account_object_path),
          contact_ft_capabilites);
    }

  gtk_tree_model_filter_refilter (
      GTK_TREE_MODEL_FILTER (osso_abook_tree_view_get_filter_model (
              OSSO_ABOOK_TREE_VIEW (chooser))));

  check_all_calls_made (chooser);
}

static void
call_table_foreach (gpointer key,
    gpointer value,
    gpointer user_data)
{
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (user_data);
  TpConnection *conn;
  gchar *bus_name;
  const gchar *connection_path;

  McAccount *account = (McAccount *)key;
  const GArray *handles = (const GArray *) value;

  connection_path = mc_account_get_connection_path (account);

  bus_name = strreplace (connection_path + 1, "/", ".");

  conn = tp_connection_new (priv->dbus, bus_name,
      mc_account_get_connection_path (account), NULL);

  tp_proxy_add_interface_by_id ((TpProxy *)conn,
      TP_IFACE_QUARK_CONNECTION_INTERFACE_CONTACT_CAPABILITIES);

  g_signal_emit (user_data, signals[WORKING], 0);

  priv->calls_in_progress++;
  tp_cli_connection_interface_contact_capabilities_call_get_contact_capabilities (
      conn, -1, handles, get_contact_caps_cb,
      g_strdup (tp_proxy_get_object_path (account)), (GDestroyNotify) g_free,
      G_OBJECT (user_data));

  g_free (bus_name);
  g_object_unref (conn);
}

static void
add_handles_to_table_to_lookup (OssoABookContact *contact,
    GHashTable *table)
{
  GList *roster_contacts, *l;

  if (osso_abook_contact_is_roster_contact (contact))
    roster_contacts = g_list_append (NULL, contact);
  else
    roster_contacts = osso_abook_contact_get_roster_contacts (contact);

  for (l = roster_contacts; l != NULL; l = l->next)
    {
      OssoABookContact *c = (OssoABookContact *) l->data;
      McAccount *account;
      TpHandle handle;
      GArray *contacts;

      account = osso_abook_contact_get_account (c);

      /* offline */
      if (mc_account_get_connection_path (account) == NULL)
        continue;

      handle = osso_abook_presence_get_handle (OSSO_ABOOK_PRESENCE (c));

      contacts = (GArray *) g_hash_table_lookup (table, account);

      if (contacts == NULL)
        {
          contacts = g_array_new (FALSE, FALSE, sizeof (TpHandle));
          g_array_append_val (contacts, handle);
          g_hash_table_insert (table, g_object_ref (account),
              contacts);
        }
      else
        {
          g_array_append_val (contacts, handle);
        }
    }

  g_list_free (roster_contacts);
}

static gboolean
further_calls_timeout (gpointer data)
{
  MiniatureContactChooser *self = MINIATURE_CONTACT_CHOOSER (data);
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);

  g_debug ("Calling");

  g_hash_table_foreach (priv->further_calls, call_table_foreach, self);
  g_hash_table_remove_all (priv->further_calls);

  priv->further_calls_timeout_id = 0;
  return FALSE;
}

static void
roster_contact_presence_type_notify (GObject *gobject,
    GParamSpec *pspec G_GNUC_UNUSED,
    MiniatureContactChooser *chooser)
{
  OssoABookContact *r_contact = OSSO_ABOOK_CONTACT (gobject);
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (chooser);

  add_handles_to_table_to_lookup (r_contact, priv->further_calls);

  if (priv->further_calls_timeout_id > 0)
    g_source_remove (priv->further_calls_timeout_id);

  priv->further_calls_timeout_id = g_timeout_add_seconds (2,
      further_calls_timeout, chooser);

  g_signal_handlers_disconnect_by_func (gobject,
      (GClosure*) roster_contact_presence_type_notify, chooser);
}

static void
row_inserted_cb (GtkTreeModel *model,
    GtkTreePath *path G_GNUC_UNUSED,
    GtkTreeIter *iter,
    MiniatureContactChooser *chooser)
{
  OssoABookContact *contact;
  GList *roster_contacts, *l;

  gtk_tree_model_get (model, iter,
      OSSO_ABOOK_LIST_STORE_COLUMN_CONTACT, &contact,
      -1);

  roster_contacts = osso_abook_contact_get_roster_contacts (contact);

  for (l = roster_contacts; l != NULL; l = l->next)
    {
      /* offline */
      if (mc_account_get_connection_path (osso_abook_contact_get_account (
                  OSSO_ABOOK_CONTACT (l->data))) == NULL)
        continue;

      if (osso_abook_presence_get_handle (OSSO_ABOOK_PRESENCE (l->data)) > 0)
        {
          roster_contact_presence_type_notify (G_OBJECT (l->data),
              NULL, chooser);
        }
      else
        {
          miniature_signal_connect_weak (l->data, "notify::presence-type",
              G_CALLBACK (roster_contact_presence_type_notify),
              G_OBJECT (chooser));
        }
    }

  g_list_free (roster_contacts);
  g_object_unref (contact);
}

static void
roster_ready_cb (OssoABookWaitable *waitable G_GNUC_UNUSED,
    const GError *error,
    gpointer user_data)
{
  MiniatureContactChooser *self = MINIATURE_CONTACT_CHOOSER (user_data);
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);
  OssoABookContactModel *model;
  GtkTreeIter iter;
  GHashTable *call_table;

  if (error != NULL)
    {
      g_debug ("error: %s", error->message);
      return;
    }

  model = OSSO_ABOOK_CONTACT_MODEL (
      osso_abook_tree_view_get_base_model (OSSO_ABOOK_TREE_VIEW (self)));

  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), &iter))
    return;

  call_table = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
      free_garray);

  while (TRUE)
    {
      OssoABookContact *contact;

      gtk_tree_model_get (GTK_TREE_MODEL (model), &iter,
          OSSO_ABOOK_LIST_STORE_COLUMN_CONTACT, &contact,
          -1);
      add_handles_to_table_to_lookup (contact, call_table);
      g_object_unref (contact);

      if (!gtk_tree_model_iter_next (GTK_TREE_MODEL (model), &iter))
        break;
    }

  g_hash_table_foreach (call_table, call_table_foreach, self);
  g_hash_table_destroy (call_table);

  priv->made_all_calls = TRUE;
  /* We don't call check_all_calls_made here because the only way a
   * foo_call_bar can fail in the function itself if the proxy doesn't
   * have the interface. Every other time it fails in an idle, i.e. after
   * here. */

  g_signal_connect (model, "row-inserted", G_CALLBACK (row_inserted_cb), self);

}

static gboolean
visible_func (GtkTreeModel *model,
    GtkTreeIter *iter,
    gpointer user_data)
{
  MiniatureContactChooserPrivate *priv;
  //gboolean out = FALSE;  // All contacts visible for now
  gboolean out = TRUE;
  GList *roster_contacts, *l;
  OssoABookContact *contact;

  /* not great */
  if (!MINIATURE_IS_CONTACT_CHOOSER (user_data))
    return FALSE;

  priv = GET_PRIVATE (user_data);

  gtk_tree_model_get (model, iter,
      OSSO_ABOOK_LIST_STORE_COLUMN_CONTACT, &contact,
      -1);

  if (contact == NULL)
    return FALSE;

  roster_contacts = osso_abook_contact_get_roster_contacts (contact);

  for (l = roster_contacts; l != NULL; l = l->next)
    {
      McAccount *account;
      OssoABookContact *roster_contact = (OssoABookContact *) l->data;
      GHashTable *table;
      TpHandle handle;
      gpointer lookup;

      handle = osso_abook_presence_get_handle (
          OSSO_ABOOK_PRESENCE (roster_contact));

      account = osso_abook_contact_get_account (roster_contact);

      table = (GHashTable *) g_hash_table_lookup (priv->handle_capable,
          tp_proxy_get_object_path (account));

      if (table == NULL)
        continue;

      lookup = g_hash_table_lookup (table, GUINT_TO_POINTER (handle));

      if (lookup != NULL && GPOINTER_TO_INT (lookup))
        {
          out = TRUE;
          break;
        }
    }

  g_list_free (roster_contacts);

  g_object_unref (contact);

  return out;
}

static void
miniature_contact_chooser_constructed (GObject *self)
{
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);
  OssoABookContactModel *model;
  OssoABookRoster *roster;

  if (G_OBJECT_CLASS (miniature_contact_chooser_parent_class)->constructed)
    G_OBJECT_CLASS (miniature_contact_chooser_parent_class)->constructed (self);

  priv->further_calls_timeout_id = 0;
  priv->calls_in_progress = 0;
  priv->made_all_calls = FALSE;

  priv->dbus = tp_dbus_daemon_dup (NULL);

  priv->handle_capable = g_hash_table_new_full (g_str_hash, g_str_equal,
      (GDestroyNotify) g_free, (GDestroyNotify) g_hash_table_destroy);

  priv->further_calls = g_hash_table_new_full (g_direct_hash, g_direct_equal,
      (GDestroyNotify) g_object_unref, free_garray);

  model = OSSO_ABOOK_CONTACT_MODEL (
      osso_abook_tree_view_get_base_model (OSSO_ABOOK_TREE_VIEW (self)));
  roster = osso_abook_list_store_get_roster (OSSO_ABOOK_LIST_STORE (model));
  osso_abook_waitable_call_when_ready (OSSO_ABOOK_WAITABLE (roster),
      roster_ready_cb, self, NULL);
}

static void
miniature_contact_chooser_class_init (MiniatureContactChooserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = miniature_contact_chooser_dispose;
  object_class->finalize = miniature_contact_chooser_finalize;
  object_class->constructed = miniature_contact_chooser_constructed;

  signals[DONE] = g_signal_new ("done",
      G_OBJECT_CLASS_TYPE (klass),
      G_SIGNAL_RUN_LAST,
      0,
      NULL, NULL,
      g_cclosure_marshal_VOID__VOID,
      G_TYPE_NONE, 0);

  signals[WORKING] = g_signal_new ("working",
      G_OBJECT_CLASS_TYPE (klass),
      G_SIGNAL_RUN_LAST,
      0,
      NULL, NULL,
      g_cclosure_marshal_VOID__VOID,
      G_TYPE_NONE, 0);

  g_type_class_add_private (klass, sizeof (MiniatureContactChooserPrivate));
}

static void
miniature_contact_chooser_init (MiniatureContactChooser *self G_GNUC_UNUSED)
{
}

GtkWidget *
miniature_contact_chooser_new (void)
{
  GtkWidget *out;
  OssoABookContactModel *model;
  OssoABookFilterModel *filter;

  model = osso_abook_contact_model_get_default ();
  filter = osso_abook_filter_model_new (OSSO_ABOOK_LIST_STORE (model));

  out = (GtkWidget *) g_object_new (MINIATURE_TYPE_CONTACT_CHOOSER,
      "model", filter,
      "base-model", model,
      "filter-model", filter,
      "show-contact-avatar", TRUE,
      "hildon-ui-mode", HILDON_UI_MODE_NORMAL,
      NULL);

  osso_abook_filter_model_set_visible_func (filter, visible_func, out, NULL);
  gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (filter));

  g_object_unref (filter);

  return out;
}

gboolean
miniature_contact_chooser_can_send_to_roster_contact (MiniatureContactChooser *self,
    OssoABookContact *roster_contact)
{
  MiniatureContactChooserPrivate *priv = GET_PRIVATE (self);
  McAccount *account;
  GHashTable *table;
  TpHandle handle;
  gpointer lookup;

  if (!osso_abook_contact_is_roster_contact (roster_contact))
    return FALSE;

  handle = osso_abook_presence_get_handle (
      OSSO_ABOOK_PRESENCE (roster_contact));

  account = osso_abook_contact_get_account (roster_contact);

  table = (GHashTable *) g_hash_table_lookup (priv->handle_capable,
      tp_proxy_get_object_path (account));

  if (table == NULL)
    return FALSE;

  lookup = g_hash_table_lookup (table, GUINT_TO_POINTER (handle));

  if (lookup != NULL && GPOINTER_TO_INT (lookup))
    return TRUE;

  return FALSE;
}
