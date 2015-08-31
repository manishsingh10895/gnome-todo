/* gtd-storage.c
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

#include "gtd-task-list.h"
#include "gtd-storage.h"

#include <glib/gi18n.h>

typedef struct
{
  gchar                 *id;
  gchar                 *name;
  gchar                 *provider;

  gint                   enabled : 1;
  gint                   is_default : 1;
} GtdStoragePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GtdStorage, gtd_storage, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_ENABLED,
  PROP_ICON,
  PROP_ID,
  PROP_IS_DEFAULT,
  PROP_NAME,
  PROP_PROVIDER,
  LAST_PROP
};

static void
gtd_storage_finalize (GObject *object)
{
  GtdStorage *self = (GtdStorage *)object;
  GtdStoragePrivate *priv = gtd_storage_get_instance_private (self);

  g_clear_pointer (&priv->id, g_free);
  g_clear_pointer (&priv->name, g_free);
  g_clear_pointer (&priv->provider, g_free);

  G_OBJECT_CLASS (gtd_storage_parent_class)->finalize (object);
}

static void
gtd_storage_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GtdStorage *self = GTD_STORAGE (object);
  GtdStoragePrivate *priv = gtd_storage_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_ENABLED:
      g_value_set_boolean (value, priv->enabled);
      break;

    case PROP_ICON:
      g_value_set_object (value, gtd_storage_get_icon (self));
      break;

    case PROP_ID:
      g_value_set_string (value, gtd_storage_get_id (self));
      break;

    case PROP_IS_DEFAULT:
      g_value_set_boolean (value, priv->is_default);
      break;

    case PROP_NAME:
      g_value_set_string (value, gtd_storage_get_name (self));
      break;

    case PROP_PROVIDER:
      g_value_set_string (value, gtd_storage_get_provider (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_storage_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GtdStorage *self = GTD_STORAGE (object);

  switch (prop_id)
    {
    case PROP_ENABLED:
      gtd_storage_set_enabled (self, g_value_get_boolean (value));
      break;

    case PROP_ID:
      gtd_storage_set_id (self, g_value_get_string (value));
      break;

    case PROP_IS_DEFAULT:
      gtd_storage_set_is_default (self, g_value_get_boolean (value));
      break;

    case PROP_NAME:
      gtd_storage_set_name (self, g_value_get_string (value));
      break;

    case PROP_PROVIDER:
      gtd_storage_set_provider (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_storage_class_init (GtdStorageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtd_storage_finalize;
  object_class->get_property = gtd_storage_get_property;
  object_class->set_property = gtd_storage_set_property;

  /**
   * GtdStorage::enabled:
   *
   * Whether the storage is available to be used.
   */
  g_object_class_install_property (
        object_class,
        PROP_ENABLED,
        g_param_spec_boolean ("enabled",
                              "Whether the storage is enabled",
                              "Whether the storage is available to be used.",
                              FALSE,
                              G_PARAM_READWRITE));

  /**
   * GtdStorage::icon:
   *
   * The icon representing this storage.
   */
  g_object_class_install_property (
        object_class,
        PROP_ICON,
        g_param_spec_object ("icon",
                             "Icon of the storage",
                             "The icon representing the storage location.",
                             G_TYPE_ICON,
                             G_PARAM_READABLE));

  /**
   * GtdStorage::id:
   *
   * The unique identifier of this storage.
   */
  g_object_class_install_property (
        object_class,
        PROP_ID,
        g_param_spec_string ("id",
                             "Identifier of the storage",
                             "The unique identifier of the storage location.",
                             NULL,
                             G_PARAM_READWRITE));

  /**
   * GtdStorage::is-default:
   *
   * Whether the storage is the default to be used.
   */
  g_object_class_install_property (
        object_class,
        PROP_IS_DEFAULT,
        g_param_spec_boolean ("is-default",
                              "Whether the storage is the default",
                              "Whether the storage is the default storage location to be used.",
                              FALSE,
                              G_PARAM_READWRITE));

  /**
   * GtdStorage::name:
   *
   * The user-visible name of this storage.
   */
  g_object_class_install_property (
        object_class,
        PROP_NAME,
        g_param_spec_string ("name",
                             "Name of the storage",
                             "The user-visible name of the storage location.",
                             NULL,
                             G_PARAM_READWRITE));

  /**
   * GtdStorage::provider:
   *
   * The user-visible provider name of this storage.
   */
  g_object_class_install_property (
        object_class,
        PROP_PROVIDER,
        g_param_spec_string ("provider",
                             "Name of the data provider of the storage",
                             "The user-visible name of the data provider of the storage location.",
                             NULL,
                             G_PARAM_READWRITE));
}

static void
gtd_storage_init (GtdStorage *self)
{
}

/**
 * gtd_storage_get_enabled:
 * @storage: a #GtdStorage
 *
 * Whether @storage is available for creating new #GtdTaskList.
 *
 * Returns: %TRUE if the storage is available for use, %FALSE otherwise.
 */
gboolean
gtd_storage_get_enabled (GtdStorage *storage)
{
  GtdStoragePrivate *priv;

  g_return_val_if_fail (GTD_IS_STORAGE (storage), FALSE);

  priv = gtd_storage_get_instance_private (storage);

  return priv->enabled;
}

/**
 * gtd_storage_set_enabled:
 * @storage: a #GtdStorage
 * @enabled: %TRUE to make it available for use, %FALSE otherwise.
 *
 * Sets the #GtdStorage::enabled property to @enabled.
 *
 * Returns:
 */
void
gtd_storage_set_enabled (GtdStorage *storage,
                         gboolean    enabled)
{
  GtdStoragePrivate *priv;

  g_return_if_fail (GTD_IS_STORAGE (storage));

  priv = gtd_storage_get_instance_private (storage);

  if (priv->enabled != enabled)
    {
      priv->enabled = enabled;
      g_object_notify (G_OBJECT (storage), "enabled");
    }
}

/**
 * gtd_storage_get_icon:
 * @storage: a #GtdStorage
 *
 * The #GIcon that represents this storage location. A provider
 * type must be given, or this function returns %NULL.
 *
 * Returns: (transfer none): the #GIcon that represents this
 * storage location, or %NULL.
 */
GIcon*
gtd_storage_get_icon (GtdStorage *storage)
{
  g_return_val_if_fail (GTD_STORAGE_CLASS (G_OBJECT_GET_CLASS (storage))->get_icon, NULL);

  return GTD_STORAGE_CLASS (G_OBJECT_GET_CLASS (storage))->get_icon (storage);
}

/**
 * gtd_storage_get_id:
 * @storage: a #GtdStorage
 *
 * Retrieves the unique identifier of @storage.
 *
 * Returns: (transfer none): the unique identifier of @storage.
 */
const gchar*
gtd_storage_get_id (GtdStorage *storage)
{
  GtdStoragePrivate *priv;

  g_return_val_if_fail (GTD_IS_STORAGE (storage), FALSE);

  priv = gtd_storage_get_instance_private (storage);

  return priv->id;
}

/**
 * gtd_storage_set_id:
 * @storage: a #GtdStorage
 * @id: the id of @storage
 *
 * Sets the unique identifier of @storage.
 *
 * Returns:
 */
void
gtd_storage_set_id (GtdStorage  *storage,
                    const gchar *id)
{
  GtdStoragePrivate *priv;

  g_return_if_fail (GTD_IS_STORAGE (storage));

  priv = gtd_storage_get_instance_private (storage);

  if (g_strcmp0 (priv->id, id) != 0)
    {
      g_clear_pointer (&priv->id, g_free);

      priv->id = g_strdup (id);

      g_object_notify (G_OBJECT (storage), "id");
    }
}

/**
 * gtd_storage_get_is_default:
 * @storage: a #GtdStorage
 *
 * Whether @storage is the default #GtdStorage or not.
 *
 * Returns: %TRUE is @storage is the default storage, %FALSE otherwise.
 */
gboolean
gtd_storage_get_is_default (GtdStorage *storage)
{
  GtdStoragePrivate *priv;

  g_return_val_if_fail (GTD_IS_STORAGE (storage), FALSE);

  priv = gtd_storage_get_instance_private (storage);

  return priv->is_default;
}

/**
 * gtd_storage_set_is_default:
 * @storage: a #GtdStorage
 * @is_default: %TRUE to make @storage the default one, %FALSE otherwise
 *
 * Sets the #GtdStorage::is-default property.
 *
 * Returns:
 */
void
gtd_storage_set_is_default (GtdStorage *storage,
                            gboolean    is_default)
{
  GtdStoragePrivate *priv;

  g_return_if_fail (GTD_IS_STORAGE (storage));

  priv = gtd_storage_get_instance_private (storage);

  if (priv->is_default != is_default)
    {
      priv->is_default = is_default;

      g_object_notify (G_OBJECT (storage), "is-default");
    }
}

/**
 * gtd_storage_get_name:
 * @storage: a #GtdStorage
 *
 * Retrieves the user-visible name of @storage
 *
 * Returns: (transfer none): the user-visible name of @storage
 */
const gchar*
gtd_storage_get_name (GtdStorage *storage)
{
  GtdStoragePrivate *priv;

  g_return_val_if_fail (GTD_IS_STORAGE (storage), NULL);

  priv = gtd_storage_get_instance_private (storage);

  return priv->name;
}

/**
 * gtd_storage_set_name:
 * @storage: a #GtdStorage
 *
 * Sets the user visible name of @storage.
 *
 * Returns:
 */
void
gtd_storage_set_name (GtdStorage  *storage,
                      const gchar *name)
{
  GtdStoragePrivate *priv;

  g_return_if_fail (GTD_IS_STORAGE (storage));

  priv = gtd_storage_get_instance_private (storage);

  if (g_strcmp0 (priv->name, name) != 0)
    {
      g_clear_pointer (&priv->name, g_free);

      priv->name = g_strdup (name);

      g_object_notify (G_OBJECT (storage), "name");
    }
}

/**
 * gtd_storage_get_provider:
 * @storage: a #GtdStorage
 *
 * Retrieves the user-visible name of @storage's provider
 *
 * Returns: (transfer none): the user-visible name of @storage's provider.
 */
const gchar*
gtd_storage_get_provider (GtdStorage *storage)
{
  GtdStoragePrivate *priv;

  g_return_val_if_fail (GTD_IS_STORAGE (storage), NULL);

  priv = gtd_storage_get_instance_private (storage);

  return priv->provider;
}

/**
 * gtd_storage_set_provider:
 * @storage: a #GtdStorage
 * @provider: the storage provider for @storage.
 *
 * Sets the provider of @storage. This directly affects the
 * @GtdStorage::icon property.
 *
 * Returns:
 */
void
gtd_storage_set_provider (GtdStorage  *storage,
                          const gchar *provider)
{
  GtdStoragePrivate *priv;

  g_return_if_fail (GTD_IS_STORAGE (storage));

  priv = gtd_storage_get_instance_private (storage);

  if (g_strcmp0 (priv->provider, provider) != 0)
    {
      g_clear_pointer (&priv->provider, g_free);

      priv->provider = g_strdup (provider);

      g_object_notify (G_OBJECT (storage), "provider");
    }
}

/**
 * gtd_storage_compare:
 * @a: a #GtdStorage
 * @b: a #GtdStorage
 *
 * Compare @a and @b
 *
 * Returns: -1 if @a comes before @b, 0 if they're equal, 1 otherwise.
 */
gint
gtd_storage_compare (GtdStorage *a,
                     GtdStorage *b)
{
  gint retval;

  g_return_val_if_fail (GTD_IS_STORAGE (a), 0);
  g_return_val_if_fail (GTD_IS_STORAGE (b), 0);

  /* Let subclasses decide */
  retval = GTD_STORAGE_CLASS (G_OBJECT_GET_CLASS (a))->compare (a, b);

  if (retval != 0)
    return retval;

  retval = GTD_STORAGE_CLASS (G_OBJECT_GET_CLASS (b))->compare (a, b);

  return retval;
}

GtdTaskList*
gtd_storage_create_task_list (GtdStorage  *storage,
                              const gchar *name)
{
  g_return_val_if_fail (GTD_STORAGE_CLASS (G_OBJECT_GET_CLASS (storage))->create_list, NULL);

  return GTD_STORAGE_CLASS (G_OBJECT_GET_CLASS (storage))->create_list (storage, name);
}
