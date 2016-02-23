/* gtd-plugin-dialog-row.h
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
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

#ifndef GTD_PLUGIN_DIALOG_ROW_H
#define GTD_PLUGIN_DIALOG_ROW_H

#include "gtd-types.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libpeas/peas.h>

G_BEGIN_DECLS

#define GTD_TYPE_PLUGIN_DIALOG_ROW (gtd_plugin_dialog_row_get_type())

G_DECLARE_FINAL_TYPE (GtdPluginDialogRow, gtd_plugin_dialog_row, GTD, PLUGIN_DIALOG_ROW, GtkListBoxRow)

GtkWidget*           gtd_plugin_dialog_row_new                   (PeasPluginInfo     *info,
                                                                  GtdActivatable     *activatable);

PeasPluginInfo*      gtd_plugin_dialog_row_get_info              (GtdPluginDialogRow *row);

GtdActivatable*      gtd_plugin_dialog_row_get_plugin            (GtdPluginDialogRow *row);

void                 gtd_plugin_dialog_row_set_plugin            (GtdPluginDialogRow *row,
                                                                  GtdActivatable     *activatable);

G_END_DECLS

#endif /* GTD_PLUGIN_DIALOG_ROW_H */
