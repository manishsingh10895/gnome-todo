/* gtd-plugin-manager.h
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

#ifndef GTD_PLUGIN_MANAGER_H
#define GTD_PLUGIN_MANAGER_H

#include <glib-object.h>

#include "gtd-object.h"
#include "gtd-types.h"

G_BEGIN_DECLS

#define GTD_TYPE_PLUGIN_MANAGER (gtd_plugin_manager_get_type())

G_DECLARE_FINAL_TYPE (GtdPluginManager, gtd_plugin_manager, GTD, PLUGIN_MANAGER, GtdObject)

GtdPluginManager*    gtd_plugin_manager_new                      (void);

void                 gtd_plugin_manager_load_plugins             (GtdPluginManager   *self);

GList*               gtd_plugin_manager_get_loaded_plugins       (GtdPluginManager   *self);

G_END_DECLS

#endif /* GTD_PLUGIN_MANAGER_H */
