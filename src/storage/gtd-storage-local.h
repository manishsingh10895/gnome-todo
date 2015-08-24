/* gtd-storage-local.h
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

#ifndef GTD_STORAGE_LOCAL_H
#define GTD_STORAGE_LOCAL_H

#include "gtd-storage.h"
#include "gtd-types.h"

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GTD_TYPE_STORAGE_LOCAL (gtd_storage_local_get_type())

G_DECLARE_FINAL_TYPE (GtdStorageLocal, gtd_storage_local, GTD, STORAGE_LOCAL, GtdStorage)

GtdStorage*          gtd_storage_local_new                       (void);

G_END_DECLS

#endif /* GTD_STORAGE_LOCAL_H */
