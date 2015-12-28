/* gtd-list-selector-grid.h
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

#ifndef GTD_LIST_SELECTOR_GRID_H
#define GTD_LIST_SELECTOR_GRID_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTD_TYPE_LIST_SELECTOR_GRID (gtd_list_selector_grid_get_type())

G_DECLARE_FINAL_TYPE (GtdListSelectorGrid, gtd_list_selector_grid, GTD, LIST_SELECTOR_GRID, GtkFlowBox)

GtkWidget*           gtd_list_selector_grid_new                  (void);

G_END_DECLS

#endif /* GTD_LIST_SELECTOR_GRID_H */
