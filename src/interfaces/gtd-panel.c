/* gtd-panel.c
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

#include "gtd-panel.h"

G_DEFINE_INTERFACE (GtdPanel, gtd_panel, GTK_TYPE_WIDGET)

static void
gtd_panel_default_init (GtdPanelInterface *iface)
{
  /**
   * GtdPanel::name:
   *
   * The identifier name of the panel. It is used as the #GtkStack
   * name, so be sure to use a specific name that won't collide with
   * other plugins.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("name",
                                                            "The name of the panel",
                                                            "The identifier name of the panel",
                                                            NULL,
                                                            G_PARAM_READABLE));

  /**
   * GtdPanel::title:
   *
   * The user-visible title of the panel. It is used as the #GtkStack
   * title.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("title",
                                                            "The title of the panel",
                                                            "The user-visible title of the panel",
                                                            NULL,
                                                            G_PARAM_READABLE));

  /**
   * GtdPanel::header-widgets:
   *
   * A #GList of widgets to be added to the headerbar.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_pointer ("header-widgets",
                                                             "The header widgets",
                                                             "The widgets to be added in the headerbar",
                                                             G_PARAM_READABLE));

  /**
   * GtdPanel::menu:
   *
   * A #GMenu of entries of the window menu.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("menu",
                                                            "The title of the panel",
                                                            "The user-visible title of the panel",
                                                            G_TYPE_MENU,
                                                            G_PARAM_READABLE));

}

const gchar*
gtd_panel_get_name (GtdPanel *panel)
{
  g_return_val_if_fail (GTD_IS_PANEL (panel), NULL);
  g_return_val_if_fail (GTD_PANEL_GET_IFACE (panel)->get_name, NULL);

  return GTD_PANEL_GET_IFACE (panel)->get_name (panel);
}

const gchar*
gtd_panel_get_title (GtdPanel *panel)
{
  g_return_val_if_fail (GTD_IS_PANEL (panel), NULL);
  g_return_val_if_fail (GTD_PANEL_GET_IFACE (panel)->get_title, NULL);

  return GTD_PANEL_GET_IFACE (panel)->get_title (panel);
}

GList*
gtd_panel_get_header_widgets (GtdPanel *panel)
{
  g_return_val_if_fail (GTD_IS_PANEL (panel), NULL);
  g_return_val_if_fail (GTD_PANEL_GET_IFACE (panel)->get_header_widgets, NULL);

  return GTD_PANEL_GET_IFACE (panel)->get_header_widgets (panel);
}

const GMenu*
gtd_panel_get_menu (GtdPanel *panel)
{
  g_return_val_if_fail (GTD_IS_PANEL (panel), NULL);
  g_return_val_if_fail (GTD_PANEL_GET_IFACE (panel)->get_menu, NULL);

  return GTD_PANEL_GET_IFACE (panel)->get_menu (panel);
}
