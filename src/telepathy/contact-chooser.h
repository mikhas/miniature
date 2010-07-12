/*
 * contact-chooser.h
 *
 * Copyright (C) 2009-2010 Collabora Ltd. <http://www.collabora.co.uk/>
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

#ifndef __MINIATURE_CONTACT_CHOOSER_H__
#define __MINIATURE_CONTACT_CHOOSER_H__

#include <glib-object.h>

#include <libosso-abook/osso-abook.h>

G_BEGIN_DECLS

#define MINIATURE_TYPE_CONTACT_CHOOSER \
  (miniature_contact_chooser_get_type ())
#define MINIATURE_CONTACT_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), MINIATURE_TYPE_CONTACT_CHOOSER, \
   MiniatureContactChooser))
#define MINIATURE_CONTACT_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), MINIATURE_TYPE_CONTACT_CHOOSER, \
   MiniatureContactChooserClass))
#define MINIATURE_IS_CONTACT_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), MINIATURE_TYPE_CONTACT_CHOOSER))
#define MINIATURE_IS_CONTACT_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), MINIATURE_TYPE_CONTACT_CHOOSER))
#define MINIATURE_CONTACT_CHOOSER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MINIATURE_TYPE_CONTACT_CHOOSER, \
   MiniatureContactChooserClass))

typedef struct _MiniatureContactChooser MiniatureContactChooser;
typedef struct _MiniatureContactChooserClass MiniatureContactChooserClass;

struct _MiniatureContactChooserClass {
  OssoABookContactViewClass parent_class;
};

struct _MiniatureContactChooser {
  OssoABookContactView parent;
};

GType miniature_contact_chooser_get_type (void);

GtkWidget * miniature_contact_chooser_new (void);

gboolean miniature_contact_chooser_can_send_to_roster_contact (
    MiniatureContactChooser *self, OssoABookContact *roster_contact);
G_END_DECLS

#endif /* #ifndef __MINIATURE_CONTACT_CHOOSER_H__*/
