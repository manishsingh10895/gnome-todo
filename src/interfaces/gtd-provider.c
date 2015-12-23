/* gtd-provider.c
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

#include "gtd-provider.h"
#include "gtd-task-list.h"

G_DEFINE_INTERFACE (GtdProvider, gtd_provider, GTD_TYPE_OBJECT)

enum
{
  LIST_ADDED,
  LIST_CHANGED,
  LIST_REMOVED,
  NUM_SIGNALS
};

static guint signals[NUM_SIGNALS] = { 0, };

static void
gtd_provider_default_init (GtdProviderInterface *iface)
{
  /**
   * GtdProvider::enabled:
   *
   * Whether the #GtdProvider is enabled.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_boolean ("enabled",
                                                             "Identifier of the provider",
                                                             "The identifier of the provider",
                                                             FALSE,
                                                             G_PARAM_READABLE));

  /**
   * GtdProvider::icon:
   *
   * The icon of the #GtdProvider, e.g. the account icon
   * of a GNOME Online Accounts' account.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("icon",
                                                            "Icon of the provider",
                                                            "The icon of the provider",
                                                            G_TYPE_ICON,
                                                            G_PARAM_READABLE));

  /**
   * GtdProvider::id:
   *
   * The unique identifier of the #GtdProvider.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("id",
                                                            "Identifier of the provider",
                                                            "The identifier of the provider",
                                                            NULL,
                                                            G_PARAM_READABLE));

  /**
   * GtdProvider::name:
   *
   * The user-visible name of the #GtdProvider.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("name",
                                                            "Name of the provider",
                                                            "The user-visible name of the provider",
                                                            NULL,
                                                            G_PARAM_READABLE));

  /**
   * GtdProvider::description:
   *
   * The description of the #GtdProvider, e.g. the account user
   * of a GNOME Online Accounts' account.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("description",
                                                            "Description of the provider",
                                                            "The description of the provider",
                                                            NULL,
                                                            G_PARAM_READABLE));

  /**
   * GtdProvider::list-added:
   *
   * The ::list-added signal is emmited after a #GtdTaskList
   * is connected.
   */
  signals[LIST_ADDED] = g_signal_new ("list-added",
                                      GTD_TYPE_PROVIDER,
                                      G_SIGNAL_RUN_LAST,
                                      0,
                                      NULL,
                                      NULL,
                                      NULL,
                                      G_TYPE_NONE,
                                      1,
                                      GTD_TYPE_TASK_LIST);

/**
   * GtdProvider::list-changed:
   *
   * The ::list-changed signal is emmited after a #GtdTaskList
   * has any of it's properties changed.
   */
  signals[LIST_CHANGED] = g_signal_new ("list-changed",
                                        GTD_TYPE_PROVIDER,
                                        G_SIGNAL_RUN_LAST,
                                        0,
                                        NULL,
                                        NULL,
                                        NULL,
                                        G_TYPE_NONE,
                                        1,
                                        GTD_TYPE_TASK_LIST);

  /**
   * GtdManager::list-removed:
   *
   * The ::list-removed signal is emmited after a #GtdTaskList
   * is disconnected.
   */
  signals[LIST_REMOVED] = g_signal_new ("list-removed",
                                        GTD_TYPE_PROVIDER,
                                        G_SIGNAL_RUN_LAST,
                                        0,
                                        NULL,
                                        NULL,
                                        NULL,
                                        G_TYPE_NONE,
                                        1,
                                        GTD_TYPE_TASK_LIST);
}

const gchar*
gtd_provider_get_id (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_id, NULL);

  return GTD_PROVIDER_GET_IFACE (provider)->get_id (provider);
}

const gchar*
gtd_provider_get_name (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_name, NULL);

  return GTD_PROVIDER_GET_IFACE (provider)->get_name (provider);
}

const gchar*
gtd_provider_get_description (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_description, NULL);

  return GTD_PROVIDER_GET_IFACE (provider)->get_description (provider);
}

gboolean
gtd_provider_get_enabled (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), FALSE);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_enabled, FALSE);

  return GTD_PROVIDER_GET_IFACE (provider)->get_enabled (provider);
}

GIcon*
gtd_provider_get_icon (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_icon, NULL);

  return GTD_PROVIDER_GET_IFACE (provider)->get_icon (provider);
}

const GtkWidget*
gtd_provider_get_edit_panel (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_edit_panel, NULL);

  return GTD_PROVIDER_GET_IFACE (provider)->get_edit_panel (provider);
}

void
gtd_provider_create_task (GtdProvider *provider,
                          GtdTask     *task)
{
  g_return_if_fail (GTD_IS_PROVIDER (provider));
  g_return_if_fail (GTD_PROVIDER_GET_IFACE (provider)->create_task);

  GTD_PROVIDER_GET_IFACE (provider)->create_task (provider, task);
}

void
gtd_provider_update_task (GtdProvider *provider,
                          GtdTask     *task)
{
  g_return_if_fail (GTD_IS_PROVIDER (provider));
  g_return_if_fail (GTD_PROVIDER_GET_IFACE (provider)->update_task);

  GTD_PROVIDER_GET_IFACE (provider)->update_task (provider, task);
}

void
gtd_provider_remove_task (GtdProvider *provider,
                          GtdTask     *task)
{
  g_return_if_fail (GTD_IS_PROVIDER (provider));
  g_return_if_fail (GTD_PROVIDER_GET_IFACE (provider)->remove_task);

  GTD_PROVIDER_GET_IFACE (provider)->remove_task (provider, task);
}

void
gtd_provider_create_task_list (GtdProvider *provider,
                               GtdTaskList *list)
{
  g_return_if_fail (GTD_IS_PROVIDER (provider));
  g_return_if_fail (GTD_PROVIDER_GET_IFACE (provider)->create_task_list);

  GTD_PROVIDER_GET_IFACE (provider)->create_task_list (provider, list);
}

void
gtd_provider_update_task_list (GtdProvider *provider,
                               GtdTaskList *list)
{
  g_return_if_fail (GTD_IS_PROVIDER (provider));
  g_return_if_fail (GTD_PROVIDER_GET_IFACE (provider)->update_task_list);

  GTD_PROVIDER_GET_IFACE (provider)->update_task_list (provider, list);
}

void
gtd_provider_remove_task_list (GtdProvider *provider,
                               GtdTaskList *list)
{
  g_return_if_fail (GTD_IS_PROVIDER (provider));
  g_return_if_fail (GTD_PROVIDER_GET_IFACE (provider)->remove_task_list);

  GTD_PROVIDER_GET_IFACE (provider)->remove_task_list (provider, list);
}

GList*
gtd_provider_get_task_lists (GtdProvider *provider)
{
  g_return_val_if_fail (GTD_IS_PROVIDER (provider), NULL);
  g_return_val_if_fail (GTD_PROVIDER_GET_IFACE (provider)->get_task_lists, NULL);

  return GTD_PROVIDER_GET_IFACE (provider)->get_task_lists (provider);
}
