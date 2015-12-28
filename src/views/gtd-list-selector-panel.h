/* gtd-list-selector-panel.h
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

#ifndef GTD_LIST_SELECTOR_PANEL_H
#define GTD_LIST_SELECTOR_PANEL_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTD_TYPE_LIST_SELECTOR_PANEL (gtd_list_selector_panel_get_type())

G_DECLARE_FINAL_TYPE (GtdListSelectorPanel, gtd_list_selector_panel, GTD, LIST_SELECTOR_PANEL, GtkStack)

GtkWidget*           gtd_list_selector_panel_new                 (void);

G_END_DECLS

#endif /* GTD_LIST_SELECTOR_PANEL_H */
