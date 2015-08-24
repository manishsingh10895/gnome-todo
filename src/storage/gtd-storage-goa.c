/* gtd-storage-goa.c
 *
 * Copyright (C) 2015 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtd-storage-goa.h"
#include "gtd-storage.h"
#include "gtd-task-list.h"

#include <glib/gi18n.h>

struct _GtdStorageGoa
{
  GtdStorage          parent;

  GoaAccount         *account;
  GIcon              *icon;
  gchar              *parent_source;
  gchar              *uri;
};

G_DEFINE_TYPE (GtdStorageGoa, gtd_storage_goa, GTD_TYPE_STORAGE)

enum {
  PROP_0,
  PROP_ACCOUNT,
  PROP_PARENT,
  PROP_URI,
  LAST_PROP
};

static GIcon*
gtd_storage_goa_get_icon (GtdStorage *storage)
{
  g_return_val_if_fail (GTD_IS_STORAGE_GOA (storage), NULL);

  return GTD_STORAGE_GOA (storage)->icon;
}

static GtdTaskList*
gtd_storage_goa_create_list (GtdStorage  *storage,
                             const gchar *name)
{
  GtdStorageGoa *self;
  GtdTaskList *task_list;
  ESource *source;
  GError *error;

  g_return_val_if_fail (GTD_IS_STORAGE_GOA (storage), NULL);

  self = GTD_STORAGE_GOA (storage);
  error = NULL;
  source = e_source_new (NULL,
                         NULL,
                         &error);

  if (error)
    {
      g_warning ("%s: %s: %s",
                 G_STRFUNC,
                 _("Error creating new task list"),
                 error->message);

      g_clear_error (&error);
      return NULL;
    }

  /* Some properties */
  e_source_set_display_name (source, name);
  e_source_set_parent (source, self->parent_source);

  /* TODO: create source for GOA account */
  e_source_get_extension (source, E_SOURCE_EXTENSION_TASK_LIST);

  /* Create task list */
  task_list = gtd_task_list_new (source, gtd_storage_get_name (storage));

  return task_list;
}

static gint
gtd_storage_goa_compare (GtdStorage *a,
                         GtdStorage *b)
{
  GoaAccount *account_a;
  GoaAccount *account_b;
  gint retval;

  /*
   * If any of the compared storages is not a GOA storage, we have
   * no means to properly compare them. Simply return 0 and let the
   * parent class' call decide what to do.
   */
  if (!GTD_IS_STORAGE_GOA (a) || !GTD_IS_STORAGE_GOA (b))
    return 0;

  account_a = gtd_storage_goa_get_account (GTD_STORAGE_GOA (a));
  account_b = gtd_storage_goa_get_account (GTD_STORAGE_GOA (b));

  retval = g_strcmp0 (goa_account_get_provider_type (account_b), goa_account_get_provider_type (account_a));

  if (retval != 0)
    return retval;

  retval = g_strcmp0 (gtd_storage_get_id (b), gtd_storage_get_id (a));

  return retval;
}

static void
gtd_storage_goa_finalize (GObject *object)
{
  GtdStorageGoa *self = (GtdStorageGoa *)object;

  g_clear_object (&self->account);
  g_clear_object (&self->icon);
  g_clear_pointer (&self->parent_source, g_free);

  G_OBJECT_CLASS (gtd_storage_goa_parent_class)->finalize (object);
}

static void
gtd_storage_goa_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GtdStorageGoa *self = GTD_STORAGE_GOA (object);

  switch (prop_id)
    {
    case PROP_ACCOUNT:
      g_value_set_object (value, self->account);
      break;

    case PROP_PARENT:
      g_value_set_string (value, gtd_storage_goa_get_parent (self));
      break;

    case PROP_URI:
      g_value_set_string (value, gtd_storage_goa_get_uri (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_storage_goa_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GtdStorageGoa *self = GTD_STORAGE_GOA (object);

  switch (prop_id)
    {
    case PROP_ACCOUNT:
      g_set_object (&self->account, g_value_get_object (value));
      g_object_notify (object, "account");

      if (self->account)
        {
          gchar *icon_name;

          icon_name = g_strdup_printf ("goa-account-%s", goa_account_get_provider_type (self->account));

          gtd_storage_set_name (GTD_STORAGE (object), goa_account_get_identity (self->account));
          gtd_storage_set_provider (GTD_STORAGE (object), goa_account_get_provider_name (self->account));

          g_set_object (&self->icon, g_themed_icon_new (icon_name));
          g_object_notify (object, "icon");

          g_free (icon_name);
        }

      break;

    case PROP_PARENT:
      gtd_storage_goa_set_parent (self, g_value_get_string (value));
      break;

    case PROP_URI:
      gtd_storage_goa_set_uri (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_storage_goa_class_init (GtdStorageGoaClass *klass)
{
  GtdStorageClass *storage_class = GTD_STORAGE_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtd_storage_goa_finalize;
  object_class->get_property = gtd_storage_goa_get_property;
  object_class->set_property = gtd_storage_goa_set_property;

  storage_class->get_icon = gtd_storage_goa_get_icon;
  storage_class->compare = gtd_storage_goa_compare;
  storage_class->create_list = gtd_storage_goa_create_list;

  /**
   * GtdStorageGoa::account:
   *
   * The #GoaAccount of this storage.
   */
  g_object_class_install_property (
        object_class,
        PROP_ACCOUNT,
        g_param_spec_object ("account",
                             _("Account of the storage"),
                             _("The account of the storage location."),
                             GOA_TYPE_ACCOUNT,
                             G_PARAM_READWRITE));

  /**
   * GtdStorageGoa::parent:
   *
   * The parent source of this storage.
   */
  g_object_class_install_property (
        object_class,
        PROP_PARENT,
        g_param_spec_string ("parent",
                             _("Parent of the storage"),
                             _("The parent source identifier of the storage location."),
                             NULL,
                             G_PARAM_READWRITE));

  /**
   * GtdStorageGoa::uri:
   *
   * The uri of this GOA calendar.
   */
  g_object_class_install_property (
        object_class,
        PROP_URI,
        g_param_spec_string ("uri",
                             _("URI of the storage"),
                             _("The URI of the calendar of the storage location."),
                             NULL,
                             G_PARAM_READWRITE));
}

static void
gtd_storage_goa_init (GtdStorageGoa *self)
{
}

GtdStorage*
gtd_storage_goa_new (GoaAccount *account)
{
  GtdStorageGoa *self;
  GtdStorage *storage;

  storage =  g_object_new (GTD_TYPE_STORAGE_GOA,
                           "account", account,
                           NULL);
  self = GTD_STORAGE_GOA (storage);

  /*
   * HACK: for any esoteric reason, gtd_storage_goa_set_property
   * is not called when we set ::account property.
   */
  g_set_object (&self->account, account);
  g_object_notify (G_OBJECT (self), "account");

  if (self->account)
    {
      gchar *icon_name;

      icon_name = g_strdup_printf ("goa-account-%s", goa_account_get_provider_type (self->account));

      gtd_storage_set_name (storage, goa_account_get_identity (account));
      gtd_storage_set_provider (storage, goa_account_get_provider_name (account));

      g_set_object (&self->icon, g_themed_icon_new (icon_name));
      g_object_notify (G_OBJECT (self), "icon");

      g_free (icon_name);
    }

  return storage;
}

GoaAccount*
gtd_storage_goa_get_account (GtdStorageGoa *goa_storage)
{
  g_return_val_if_fail (GTD_IS_STORAGE_GOA (goa_storage), NULL);

  return goa_storage->account;
}

const gchar*
gtd_storage_goa_get_parent (GtdStorageGoa *goa_storage)
{
  g_return_val_if_fail (GTD_IS_STORAGE_GOA (goa_storage), NULL);

  return goa_storage->parent_source;
}

void
gtd_storage_goa_set_parent (GtdStorageGoa *goa_storage,
                            const gchar   *parent)
{
  g_return_if_fail (GTD_IS_STORAGE_GOA (goa_storage));

  if (g_strcmp0 (goa_storage->parent_source, parent) != 0)
    {
      g_clear_pointer (&goa_storage->parent_source, g_free);

      goa_storage->parent_source = g_strdup (parent);

      g_object_notify (G_OBJECT (goa_storage), "parent");
    }
}

const gchar*
gtd_storage_goa_get_uri (GtdStorageGoa *goa_storage)
{
  g_return_val_if_fail (GTD_IS_STORAGE_GOA (goa_storage), NULL);

  return goa_storage->uri;
}

void
gtd_storage_goa_set_uri (GtdStorageGoa *goa_storage,
                         const gchar   *uri)
{
  g_return_if_fail (GTD_IS_STORAGE_GOA (goa_storage));

  if (g_strcmp0 (goa_storage->uri, uri) != 0)
    {
      g_clear_pointer (&goa_storage->uri, g_free);

      goa_storage->uri = g_strdup (uri);

      g_object_notify (G_OBJECT (goa_storage), "uri");
    }
}
