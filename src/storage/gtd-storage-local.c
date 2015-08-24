/* gtd-storage-local.c
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

#include "gtd-storage-local.h"
#include "gtd-storage.h"
#include "gtd-task-list.h"

#include <glib/gi18n.h>

struct _GtdStorageLocal
{
  GtdStorage          parent;

  GIcon              *icon;
};

G_DEFINE_TYPE (GtdStorageLocal, gtd_storage_local, GTD_TYPE_STORAGE)

enum {
  PROP_0,
  LAST_PROP
};

static GIcon*
gtd_storage_local_get_icon (GtdStorage *storage)
{
  g_return_val_if_fail (GTD_IS_STORAGE_LOCAL (storage), NULL);

  return GTD_STORAGE_LOCAL (storage)->icon;
}

static GtdTaskList*
gtd_storage_local_create_list (GtdStorage  *storage,
                               const gchar *name)
{
  ESourceExtension *extension;
  GtdTaskList *task_list;
  ESource *source;
  GError *error;

  g_return_val_if_fail (GTD_IS_STORAGE_LOCAL (storage), NULL);

  error = NULL;

  /* Create the source */
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

  /* EDS properties */
  e_source_set_display_name (source, name);

  /* Make it a local source */
  extension = e_source_get_extension (source, E_SOURCE_EXTENSION_TASK_LIST);

  e_source_set_parent (source, "local-stub");
  e_source_backend_set_backend_name (E_SOURCE_BACKEND (extension), "local");

  /* Create task list */
  task_list = gtd_task_list_new (source, _("On This Computer"));

  return task_list;
}

static gint
gtd_storage_local_compare (GtdStorage *a,
                           GtdStorage *b)
{
  /* The local storage is always the first one */
  return GTD_IS_STORAGE_LOCAL (a) - GTD_IS_STORAGE_LOCAL (b);
}

static void
gtd_storage_local_finalize (GObject *object)
{
  GtdStorageLocal *self = GTD_STORAGE_LOCAL (object);

  g_clear_object (&self->icon);

  G_OBJECT_CLASS (gtd_storage_local_parent_class)->finalize (object);
}

static void
gtd_storage_local_class_init (GtdStorageLocalClass *klass)
{
  GtdStorageClass *storage_class = GTD_STORAGE_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtd_storage_local_finalize;

  storage_class->get_icon = gtd_storage_local_get_icon;
  storage_class->compare = gtd_storage_local_compare;
  storage_class->create_list = gtd_storage_local_create_list;
}

static void
gtd_storage_local_init (GtdStorageLocal *self)
{
  self->icon = G_ICON (g_themed_icon_new_with_default_fallbacks ("computer-symbolic"));
}

GtdStorage*
gtd_storage_local_new (void)
{
  return g_object_new (GTD_TYPE_STORAGE_LOCAL,
                       "id", "local",
                       "name", _("Local"),
                       "provider", _("On This Computer"),
                       NULL);
}
