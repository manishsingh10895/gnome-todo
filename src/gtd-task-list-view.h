/* gtd-task-list-view.h
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

#ifndef GTD_TASK_LIST_VIEW_H
#define GTD_TASK_LIST_VIEW_H

#include "gtd-types.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTD_TYPE_TASK_LIST_VIEW (gtd_task_list_view_get_type())

G_DECLARE_FINAL_TYPE (GtdTaskListView, gtd_task_list_view, GTD, TASK_LIST_VIEW, GtkOverlay)

/**
 * GtdTaskListViewHeaderFunc:
 * @row: the current #GtkListBoxRow
 * @row_task: the #GtdTask that @row represents
 * @before: the #GtkListBoxRow before @row
 * @before_task: the #GtdTask that @before represents
 * @user_data: (closure): user data
 *
 * The header function called on every task.
 */
typedef void (*GtdTaskListViewHeaderFunc) (GtkListBoxRow        *row,
                                           GtdTask              *row_task,
                                           GtkListBoxRow        *before,
                                           GtdTask              *before_task,
                                           gpointer              user_data);

/**
 * GtdTaskListViewSortFunc:
 * @row1: the current #GtkListBoxRow
 * @row1_task: the #GtdTask that @row represents
 * @row2: the #GtkListBoxRow before @row
 * @row2_task: the #GtdTask that @before represents
 * @user_data: (closure): user data
 *
 * The sorting function called on every task.
 */
typedef gint (*GtdTaskListViewSortFunc)   (GtkListBoxRow        *row1,
                                           GtdTask              *row1_task,
                                           GtkListBoxRow        *row2,
                                           GtdTask              *row2_task,
                                           gpointer              user_data);

GtkWidget*                gtd_task_list_view_new                (void);

GList*                    gtd_task_list_view_get_list           (GtdTaskListView        *view);

void                      gtd_task_list_view_set_list           (GtdTaskListView        *view,
                                                                 GList                  *list);

gboolean                  gtd_task_list_view_get_readonly       (GtdTaskListView        *view);

void                      gtd_task_list_view_set_readonly       (GtdTaskListView        *view,
                                                                 gboolean                readonly);

GtdTaskList*              gtd_task_list_view_get_task_list      (GtdTaskListView        *view);

void                      gtd_task_list_view_set_task_list      (GtdTaskListView        *view,
                                                                 GtdTaskList            *list);

gboolean                  gtd_task_list_view_get_show_list_name (GtdTaskListView        *view);

void                      gtd_task_list_view_set_show_list_name (GtdTaskListView        *view,
                                                                 gboolean                show_list_name);

gboolean                  gtd_task_list_view_get_show_completed (GtdTaskListView        *view);

void                      gtd_task_list_view_set_show_completed (GtdTaskListView        *view,
                                                                 gboolean                show_completed);

void                      gtd_task_list_view_set_header_func    (GtdTaskListView           *view,
                                                                 GtdTaskListViewHeaderFunc  func,
                                                                 gpointer                   user_data);

void                      gtd_task_list_view_set_sort_func      (GtdTaskListView           *view,
                                                                 GtdTaskListViewSortFunc    func,
                                                                 gpointer                   user_data);

G_END_DECLS

#endif /* GTD_TASK_LIST_VIEW_H */
