/* gtd-storage-goa.h
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

#ifndef GTD_STORAGE_GOA_H
#define GTD_STORAGE_GOA_H

#include "gtd-types.h"
#include "gtd-storage.h"

#include <glib.h>
#include <goa/goa.h>

G_BEGIN_DECLS

#define GTD_TYPE_STORAGE_GOA (gtd_storage_goa_get_type())

G_DECLARE_FINAL_TYPE (GtdStorageGoa, gtd_storage_goa, GTD, STORAGE_GOA, GtdStorage)

GtdStorage*          gtd_storage_goa_new                         (GoaObject          *object);

GoaAccount*          gtd_storage_goa_get_account                 (GtdStorageGoa      *goa_storage);

GoaObject*           gtd_storage_goa_get_object                  (GtdStorageGoa      *goa_storage);

void                 gtd_storage_goa_set_object                  (GtdStorageGoa      *goa_storage,
                                                                  GoaObject          *object);

const gchar*         gtd_storage_goa_get_parent                  (GtdStorageGoa      *goa_storage);

void                 gtd_storage_goa_set_parent                  (GtdStorageGoa      *goa_storage,
                                                                  const gchar        *parent);

const gchar*         gtd_storage_goa_get_uri                     (GtdStorageGoa      *goa_storage);

void                 gtd_storage_goa_set_uri                     (GtdStorageGoa      *goa_storage,
                                                                  const gchar        *uri);

G_END_DECLS

#endif /* GTD_STORAGE_GOA_H */
