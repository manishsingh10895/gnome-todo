/* gtd-storage.h
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

#ifndef GTD_STORAGE_H
#define GTD_STORAGE_H

#include "gtd-types.h"

#include <gtk/gtk.h>
#include <libecal/libecal.h>

G_BEGIN_DECLS

#define GTD_TYPE_STORAGE (gtd_storage_get_type())

G_DECLARE_DERIVABLE_TYPE (GtdStorage, gtd_storage, GTD, STORAGE, GObject)

struct _GtdStorageClass
{
  GObject            parent;

  /* Abstract methods */
  GIcon*             (*get_icon)                                 (GtdStorage         *storage);

  gint               (*compare)                                  (GtdStorage         *a,
                                                                  GtdStorage         *b);

  GtdTaskList*       (*create_list)                              (GtdStorage         *storage,
                                                                  const gchar        *name);

  /* Padding for future expansions */
  gpointer           padding[12];
};

gboolean           gtd_storage_get_enabled                       (GtdStorage         *storage);

void               gtd_storage_set_enabled                       (GtdStorage         *storage,
                                                                  gboolean            enabled);

GIcon*             gtd_storage_get_icon                          (GtdStorage         *storage);

const gchar*       gtd_storage_get_id                            (GtdStorage         *storage);

void               gtd_storage_set_id                            (GtdStorage         *storage,
                                                                  const gchar        *id);

gboolean           gtd_storage_get_is_default                    (GtdStorage         *storage);

void               gtd_storage_set_is_default                    (GtdStorage         *storage,
                                                                  gboolean            is_default);

const gchar*       gtd_storage_get_name                          (GtdStorage         *storage);

void               gtd_storage_set_name                          (GtdStorage         *storage,
                                                                  const gchar        *name);

const gchar*       gtd_storage_get_provider                      (GtdStorage         *storage);

void               gtd_storage_set_provider                      (GtdStorage         *storage,
                                                                  const gchar        *name);

gint               gtd_storage_compare                           (GtdStorage         *a,
                                                                  GtdStorage         *b);

GtdTaskList*       gtd_storage_create_task_list                  (GtdStorage         *storage,
                                                                  const gchar        *name);

G_END_DECLS

#endif /* GTD_STORAGE_H */
