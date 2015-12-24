/* gtd-manager.c
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

#include "interfaces/gtd-provider.h"
#include "gtd-manager.h"
#include "gtd-plugin-manager.h"
#include "gtd-task.h"
#include "gtd-task-list.h"

#include <glib/gi18n.h>
#include <libecal/libecal.h>
#include <libedataserverui/libedataserverui.h>

typedef struct
{
  /*
   * Today & Scheduled lists
   */
  GtdTaskList           *today_tasks_list;
  GtdTaskList           *scheduled_tasks_list;

  GSettings             *settings;
  GtdPluginManager      *plugin_manager;

  GList                 *tasklists;
  GList                 *providers;
  GtdProvider           *default_provider;
} GtdManagerPrivate;

struct _GtdManager
{
   GtdObject           parent;

  /*< private >*/
  GtdManagerPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtdManager, gtd_manager, GTD_TYPE_OBJECT)

/* Singleton instance */
GtdManager *gtd_manager_instance = NULL;

enum
{
  DEFAULT_PROVIDER_CHANGED,
  LIST_ADDED,
  LIST_CHANGED,
  LIST_REMOVED,
  SHOW_ERROR_MESSAGE,
  PROVIDER_ADDED,
  PROVIDER_REMOVED,
  NUM_SIGNALS
};

enum
{
  PROP_0,
  PROP_DEFAULT_PROVIDER,
  LAST_PROP
};

static guint signals[NUM_SIGNALS] = { 0, };
/*
static gboolean
is_today (GDateTime *dt)
{
  GDateTime *today;

  today = g_date_time_new_now_local ();

  if (g_date_time_get_year (dt) == g_date_time_get_year (today) &&
      g_date_time_get_month (dt) == g_date_time_get_month (today) &&
      g_date_time_get_day_of_month (dt) == g_date_time_get_day_of_month (today))
    {
      return TRUE;
    }

  g_date_time_unref (today);

  return FALSE;
}

 */

static void
emit_show_error_message (GtdManager  *manager,
                         const gchar *primary_text,
                         const gchar *secondary_text)
{
  g_signal_emit (manager,
                 signals[SHOW_ERROR_MESSAGE],
                 0,
                 primary_text,
                 secondary_text);
}

static void
gtd_manager_finalize (GObject *object)
{
  GtdManager *self = (GtdManager *)object;

  g_clear_object (&self->priv->scheduled_tasks_list);
  g_clear_object (&self->priv->today_tasks_list);

  G_OBJECT_CLASS (gtd_manager_parent_class)->finalize (object);
}

static void
gtd_manager_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_manager_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_manager_constructed (GObject *object)
{
  GtdManagerPrivate *priv = GTD_MANAGER (object)->priv;
  gchar *default_location;

  G_OBJECT_CLASS (gtd_manager_parent_class)->constructed (object);

  default_location = g_settings_get_string (priv->settings, "storage-location");

  g_free (default_location);
}

static void
gtd_manager_class_init (GtdManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtd_manager_finalize;
  object_class->get_property = gtd_manager_get_property;
  object_class->set_property = gtd_manager_set_property;
  object_class->constructed = gtd_manager_constructed;

  /**
   * GtdManager::goa-client:
   *
   * The #GoaClient asyncronously loaded.
   */
  g_object_class_install_property (
        object_class,
        PROP_DEFAULT_PROVIDER,
        g_param_spec_object ("default-provider",
                            "The default provider of the application",
                            "The default provider of the application",
                            GTD_TYPE_PROVIDER,
                            G_PARAM_READWRITE));

  /**
   * GtdManager::default-provider-changed:
   *
   * The ::default-provider-changed signal is emmited when a new #GtdStorage
   * is set as default.
   */
  signals[DEFAULT_PROVIDER_CHANGED] =
                  g_signal_new ("default-provider-changed",
                                GTD_TYPE_MANAGER,
                                G_SIGNAL_RUN_LAST,
                                0,
                                NULL,
                                NULL,
                                NULL,
                                G_TYPE_NONE,
                                2,
                                GTD_TYPE_PROVIDER,
                                GTD_TYPE_PROVIDER);

  /**
   * GtdManager::list-added:
   *
   * The ::list-added signal is emmited after a #GtdTaskList
   * is connected.
   */
  signals[LIST_ADDED] = g_signal_new ("list-added",
                                      GTD_TYPE_MANAGER,
                                      G_SIGNAL_RUN_LAST,
                                      0,
                                      NULL,
                                      NULL,
                                      NULL,
                                      G_TYPE_NONE,
                                      1,
                                      GTD_TYPE_TASK_LIST);

/**
   * GtdManager::list-changed:
   *
   * The ::list-changed signal is emmited after a #GtdTaskList
   * has any of it's properties changed.
   */
  signals[LIST_CHANGED] = g_signal_new ("list-changed",
                                        GTD_TYPE_MANAGER,
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
                                        GTD_TYPE_MANAGER,
                                        G_SIGNAL_RUN_LAST,
                                        0,
                                        NULL,
                                        NULL,
                                        NULL,
                                        G_TYPE_NONE,
                                        1,
                                        GTD_TYPE_TASK_LIST);

  /**
   * GtdManager::show-error-message:
   *
   * Notifies about errors, and sends the error message for widgets
   * to display.
   *
   */
  signals[SHOW_ERROR_MESSAGE] = g_signal_new ("show-error-message",
                                              GTD_TYPE_MANAGER,
                                              G_SIGNAL_RUN_LAST,
                                              0,
                                              NULL,
                                              NULL,
                                              NULL,
                                              G_TYPE_NONE,
                                              2,
                                              G_TYPE_STRING,
                                              G_TYPE_STRING);

  /**
   * GtdManager::provider-added:
   *
   * The ::provider-added signal is emmited after a #GtdProvider
   * is added.
   */
  signals[PROVIDER_ADDED] = g_signal_new ("provider-added",
                                          GTD_TYPE_MANAGER,
                                          G_SIGNAL_RUN_LAST,
                                          0,
                                          NULL,
                                          NULL,
                                          NULL,
                                          G_TYPE_NONE,
                                          1,
                                          GTD_TYPE_PROVIDER);

  /**
   * GtdManager::provider-removed:
   *
   * The ::provider-removed signal is emmited after a #GtdProvider
   * is removed from the list.
   */
  signals[PROVIDER_REMOVED] = g_signal_new ("provider-removed",
                                            GTD_TYPE_MANAGER,
                                            G_SIGNAL_RUN_LAST,
                                            0,
                                            NULL,
                                            NULL,
                                            NULL,
                                            G_TYPE_NONE,
                                            1,
                                            GTD_TYPE_PROVIDER);
}

static void
gtd_manager__list_added (GtdProvider *provider,
                         GtdTaskList *list,
                         GtdManager  *self)
{
  GtdManagerPrivate *priv = gtd_manager_get_instance_private (self);

  priv->tasklists = g_list_append (priv->tasklists, list);

  g_signal_emit (self, signals[LIST_ADDED], 0, list);
}

static void
gtd_manager__list_changed (GtdProvider *provider,
                           GtdTaskList *list,
                           GtdManager  *self)
{
  g_signal_emit (self, signals[LIST_CHANGED], 0, list);
}

static void
gtd_manager__list_removed (GtdProvider *provider,
                           GtdTaskList *list,
                           GtdManager  *self)
{
  GtdManagerPrivate *priv = gtd_manager_get_instance_private (self);

  priv->tasklists = g_list_remove (priv->tasklists, list);

  g_signal_emit (self, signals[LIST_REMOVED], 0, list);
}

static void
gtd_manager__provider_added (GtdPluginManager *plugin_manager,
                             GtdProvider      *provider,
                             GtdManager       *self)
{
  GtdManagerPrivate *priv = gtd_manager_get_instance_private (self);
  GList *lists;
  GList *l;

  priv->providers = g_list_append (priv->providers, provider);

  /* Add lists */
  lists = gtd_provider_get_task_lists (provider);

  for (l = lists; l != NULL; l = l->next)
    gtd_manager__list_added (provider, l->data, self);

  g_signal_connect (provider,
                    "list-added",
                    G_CALLBACK (gtd_manager__list_added),
                    self);

  g_signal_connect (provider,
                    "list-changed",
                    G_CALLBACK (gtd_manager__list_changed),
                    self);

  g_signal_connect (provider,
                    "list-removed",
                    G_CALLBACK (gtd_manager__list_removed),
                    self);

  g_signal_emit (self, signals[PROVIDER_ADDED], 0, provider);
}

static void
gtd_manager__provider_removed (GtdPluginManager *plugin_manager,
                               GtdProvider      *provider,
                               GtdManager       *self)
{
  GtdManagerPrivate *priv = gtd_manager_get_instance_private (self);
  GList *lists;
  GList *l;

  priv->providers = g_list_remove (priv->providers, provider);

  /* Remove lists */
  lists = gtd_provider_get_task_lists (provider);

  for (l = lists; l != NULL; l = l->next)
    gtd_manager__list_removed (provider, l->data, self);

  g_signal_emit (self, signals[PROVIDER_REMOVED], 0, provider);
}
static void
gtd_manager_init (GtdManager *self)
{
  self->priv = gtd_manager_get_instance_private (self);
  self->priv->settings = g_settings_new ("org.gnome.todo");

  /* fixed task lists */
  self->priv->scheduled_tasks_list = g_object_new (GTD_TYPE_TASK_LIST, NULL);
  self->priv->today_tasks_list = g_object_new (GTD_TYPE_TASK_LIST, NULL);

  /* plugin manager */
  self->priv->plugin_manager = gtd_plugin_manager_new ();

  g_signal_connect (self->priv->plugin_manager,
                    "provider-registered",
                    G_CALLBACK (gtd_manager__provider_added),
                    self);

  g_signal_connect (self->priv->plugin_manager,
                    "provider-unregistered",
                    G_CALLBACK (gtd_manager__provider_removed),
                    self);
}

/**
 * gtd_manager_get_default:
 *
 * Retrieves the singleton #GtdManager instance. You should always
 * use this function instead of @gtd_manager_new.
 *
 * Returns: (transfer none): the singleton #GtdManager instance.
 */
GtdManager*
gtd_manager_get_default (void)
{
  if (!gtd_manager_instance)
    gtd_manager_instance = gtd_manager_new ();

  return gtd_manager_instance;
}

GtdManager*
gtd_manager_new (void)
{
  return g_object_new (GTD_TYPE_MANAGER, NULL);
}

/**
 * gtd_manager_create_task:
 * @manager: a #GtdManager
 * @task: a #GtdTask
 *
 * Ask for @task's parent list source to create @task.
 *
 * Returns:
 */
void
gtd_manager_create_task (GtdManager *manager,
                         GtdTask    *task)
{
  GtdTaskList *list;
  GtdProvider *provider;

  g_return_if_fail (GTD_IS_MANAGER (manager));
  g_return_if_fail (GTD_IS_TASK (task));

  list = gtd_task_get_list (task);
  provider = gtd_task_list_get_provider (list);

  gtd_provider_create_task (provider, task);
}

/**
 * gtd_manager_remove_task:
 * @manager: a #GtdManager
 * @task: a #GtdTask
 *
 * Ask for @task's parent list source to remove @task.
 *
 * Returns:
 */
void
gtd_manager_remove_task (GtdManager *manager,
                         GtdTask    *task)
{
  GtdTaskList *list;
  GtdProvider *provider;

  g_return_if_fail (GTD_IS_MANAGER (manager));
  g_return_if_fail (GTD_IS_TASK (task));

  list = gtd_task_get_list (task);
  provider = gtd_task_list_get_provider (list);

  gtd_provider_remove_task (provider, task);
}

/**
 * gtd_manager_update_task:
 * @manager: a #GtdManager
 * @task: a #GtdTask
 *
 * Ask for @task's parent list source to update @task.
 *
 * Returns:
 */
void
gtd_manager_update_task (GtdManager *manager,
                         GtdTask    *task)
{
  GtdTaskList *list;
  GtdProvider *provider;

  g_return_if_fail (GTD_IS_MANAGER (manager));
  g_return_if_fail (GTD_IS_TASK (task));

  list = gtd_task_get_list (task);
  provider = gtd_task_list_get_provider (list);

  gtd_provider_update_task (provider, task);
}

/**
 * gtd_manager_create_task_list:
 *
 *
 * Creates a new task list at the given source.
 *
 * Returns:
 */
void
gtd_manager_create_task_list (GtdManager  *manager,
                              GtdTaskList *list)
{
  GtdProvider *provider;

  g_return_if_fail (GTD_IS_MANAGER (manager));
  g_return_if_fail (GTD_IS_TASK_LIST (list));

  provider = gtd_task_list_get_provider (list);

  gtd_provider_create_task_list (provider, list);
}

/**
 * gtd_manager_remove_task_list:
 * @manager: a #GtdManager
 * @list: a #GtdTaskList
 *
 * Deletes @list from the registry.
 *
 * Returns:
 */
void
gtd_manager_remove_task_list (GtdManager  *manager,
                              GtdTaskList *list)
{
  GtdProvider *provider;

  g_return_if_fail (GTD_IS_MANAGER (manager));
  g_return_if_fail (GTD_IS_TASK_LIST (list));

  provider = gtd_task_list_get_provider (list);

  gtd_provider_remove_task_list (provider, list);

  g_signal_emit (manager,
                 signals[LIST_REMOVED],
                 0,
                 list);
}

/**
 * gtd_manager_save_task_list:
 * @manager: a #GtdManager
 * @list: a #GtdTaskList
 *
 * Save or create @list.
 *
 * Returns:
 */
void
gtd_manager_save_task_list (GtdManager  *manager,
                            GtdTaskList *list)
{
  GtdProvider *provider;

  g_return_if_fail (GTD_IS_MANAGER (manager));
  g_return_if_fail (GTD_IS_TASK_LIST (list));

  provider = gtd_task_list_get_provider (list);

  gtd_provider_update_task_list (provider, list);
}

/**
 * gtd_manager_get_task_lists:
 * @manager: a #GtdManager
 *
 * Retrieves the list of #GtdTaskList already loaded.
 *
 * Returns: (transfer full): a newly allocated list of #GtdTaskList, or %NULL if none.
 */
GList*
gtd_manager_get_task_lists (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), NULL);

  return g_list_copy (manager->priv->tasklists);
}

/**
 * gtd_manager_get_storage_locations:
 *
 * Retrieves the list of available #GtdStorage.
 *
 * Returns: (transfer full): (type #GtdStorage): a newly allocated #GList of
 * #GtdStorage. Free with @g_list_free after use.
 */
GList*
gtd_manager_get_providers (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), NULL);

  return g_list_copy (manager->priv->providers);
}

/**
 * gtd_manager_get_default_provider:
 * @manager: a #GtdManager
 *
 * Retrieves the default provider location. Default is "local".
 *
 * Returns: (transfer none): the default provider.
 */
GtdProvider*
gtd_manager_get_default_provider (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), NULL);

  return manager->priv->default_provider;
}

/**
 * gtd_manager_set_default_storage:
 * @manager: a #GtdManager
 * @default_storage: the default storage location.
 *
 * Sets the default storage location id.
 *
 * Returns:
 */
void
gtd_manager_set_default_provider (GtdManager  *manager,
                                  GtdProvider *provider)
{
  g_return_if_fail (GTD_IS_MANAGER (manager));
/*
  if (!gtd_storage_get_is_default (default_storage))
    {
      GtdStorage *previus_default = NULL;
      GList *l;

      g_settings_set_string (manager->priv->settings,
                             "storage-location",
                             gtd_storage_get_id (default_storage));

      for (l = manager->priv->storage_locations; l != NULL; l = l->next)
        {
          if (gtd_storage_get_is_default (l->data))
            previus_default = l->data;

          gtd_storage_set_is_default (l->data, l->data == default_storage);
        }

      g_signal_emit (manager,
                     signals[DEFAULT_PROVIDER_CHANGED],
                     0,
                     default_storage,
                     previus_default);
    }
 */
}

/**
 * gtd_manager_get_settings:
 * @manager: a #GtdManager
 *
 * Retrieves the internal #GSettings from @manager.
 *
 * Returns: (transfer none): the internal #GSettings of @manager
 */
GSettings*
gtd_manager_get_settings (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), NULL);

  return manager->priv->settings;
}

/**
 * gtd_manager_get_is_first_run:
 * @manager: a #GtdManager
 *
 * Retrieves the 'first-run' setting.
 *
 * Returns: %TRUE if GNOME To Do was never run before, %FALSE otherwise.
 */
gboolean
gtd_manager_get_is_first_run (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), FALSE);

  return g_settings_get_boolean (manager->priv->settings, "first-run");
}

/**
 * gtd_manager_set_is_first_run:
 * @manager: a #GtdManager
 * @is_first_run: %TRUE to make it first run, %FALSE otherwise.
 *
 * Sets the 'first-run' setting.
 *
 * Returns:
 */
void
gtd_manager_set_is_first_run (GtdManager *manager,
                              gboolean    is_first_run)
{
  g_return_if_fail (GTD_IS_MANAGER (manager));

  g_settings_set_boolean (manager->priv->settings,
                          "first-run",
                          is_first_run);
}

/**
 * gtd_manager_get_scheduled_list:
 * @manager: a #GtdManager
 *
 * Retrieves the internal #GtdTaskList that holds scheduled tasks.
 *
 * Returns: (transfer none): the internal #GtdTaskList with scheduled
 * tasks
 */
GtdTaskList*
gtd_manager_get_scheduled_list (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), NULL);

  return manager->priv->scheduled_tasks_list;
}

/**
 * gtd_manager_get_today_list:
 * @manager: a #GtdManager
 *
 * Retrieves the internal #GtdTaskList that holds tasks for today.
 *
 * Returns: (transfer none): the internal #GtdTaskList with today's
 * tasks
 */
GtdTaskList*
gtd_manager_get_today_list (GtdManager *manager)
{
  g_return_val_if_fail (GTD_IS_MANAGER (manager), NULL);

  return manager->priv->today_tasks_list;
}

void
gtd_manager_emit_error_message (GtdManager  *manager,
                                const gchar *primary_message,
                                const gchar *secondary_message)
{
  g_return_if_fail (GTD_IS_MANAGER (manager));

  emit_show_error_message (manager,
                           primary_message,
                           secondary_message);
}
