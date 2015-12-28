/* gtd-list-selector.h
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

#ifndef GTD_LIST_SELECTOR_H
#define GTD_LIST_SELECTOR_H

#include "gtd-enums.h"
#include "gtd-types.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTD_TYPE_LIST_SELECTOR (gtd_list_selector_get_type ())

G_DECLARE_INTERFACE (GtdListSelector, gtd_list_selector, GTD, LIST_SELECTOR, GtkWidget)

struct _GtdListSelectorInterface
{
  GTypeInterface parent;

  GtdWindowMode      (*get_mode)                                 (GtdListSelector    *selector);

  void               (*set_mode)                                 (GtdListSelector    *selector,
                                                                  GtdWindowMode       mode);

  const gchar*       (*get_search_query)                         (GtdListSelector    *selector);

  void               (*set_search_query)                         (GtdListSelector    *selector,
                                                                  const gchar        *search_query);

  GList*             (*get_selected_lists)                       (GtdListSelector    *selector);
};

GtdWindowMode        gtd_list_selector_get_mode                  (GtdListSelector    *selector);

void                 gtd_list_selector_set_mode                  (GtdListSelector    *selector,
                                                                  GtdWindowMode       mode);

const gchar*         gtd_list_selector_get_search_query          (GtdListSelector    *selector);

void                 gtd_list_selector_set_search_query          (GtdListSelector    *selector,
                                                                  const gchar        *search_query);

GList*               gtd_list_selector_get_selected_lists        (GtdListSelector    *selector);

G_END_DECLS

#endif /* GTD_LIST_SELECTOR_H */
