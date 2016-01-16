/* gtd-list-selector-item.h
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

#ifndef GTD_LIST_SELECTOR_ITEM_H
#define GTD_LIST_SELECTOR_ITEM_H

#include "gtd-enums.h"
#include "gtd-types.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTD_TYPE_LIST_SELECTOR_ITEM (gtd_list_selector_item_get_type ())

G_DECLARE_INTERFACE (GtdListSelectorItem, gtd_list_selector_item, GTD, LIST_SELECTOR_ITEM, GtkWidget)

struct _GtdListSelectorItemInterface
{
  GTypeInterface parent;

  GtdTaskList*       (*get_list)                                 (GtdListSelectorItem *item);

  gboolean           (*get_selected)                             (GtdListSelectorItem *item);

  void               (*set_selected)                             (GtdListSelectorItem *item,
                                                                  gboolean             selected);
};

GtdTaskList*         gtd_list_selector_item_get_list             (GtdListSelectorItem *item);

gboolean             gtd_list_selector_item_get_selected         (GtdListSelectorItem *item);

void                 gtd_list_selector_item_set_selected         (GtdListSelectorItem *item,
                                                                  gboolean             selected);

G_END_DECLS

#endif /* GTD_LIST_SELECTOR_ITEM_H */
