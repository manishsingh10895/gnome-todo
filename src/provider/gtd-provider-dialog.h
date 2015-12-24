/* gtd-provider-dialog.h
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

#ifndef GTD_PROVIDER_DIALOG_H
#define GTD_PROVIDER_DIALOG_H

#include "gtd-types.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTD_TYPE_PROVIDER_DIALOG (gtd_provider_dialog_get_type())

G_DECLARE_FINAL_TYPE (GtdProviderDialog, gtd_provider_dialog, GTD, PROVIDER_DIALOG, GtkDialog)

GtkWidget*         gtd_provider_dialog_new                       (void);

GtdManager*        gtd_provider_dialog_get_manager               (GtdProviderDialog   *dialog);

void               gtd_provider_dialog_set_manager               (GtdProviderDialog   *dialog,
                                                                  GtdManager         *manager);

G_END_DECLS

#endif /* GTD_PROVIDER_DIALOG_H */
