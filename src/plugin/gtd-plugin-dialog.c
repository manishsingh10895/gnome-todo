/* gtd-plugin-dialog.c
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

#include "interfaces/gtd-activatable.h"
#include "gtd-manager.h"
#include "gtd-plugin-manager.h"
#include "gtd-plugin-dialog.h"

#include <libpeas/peas.h>

struct _GtdPluginDialog
{
  GtkDialog           parent;

  GtkWidget          *back_button;
  GtkWidget          *frame;
  GtkWidget          *listbox;
  GtkWidget          *stack;
};

G_DEFINE_TYPE (GtdPluginDialog, gtd_plugin_dialog, GTK_TYPE_DIALOG)

static void
back_button_clicked (GtkWidget       *button,
                     GtdPluginDialog *self)
{
  gtk_stack_set_visible_child_name (GTK_STACK (self->stack), "list");
  gtk_widget_hide (button);
}

static void
preferences_button_clicked (GtkWidget       *button,
                            GtdPluginDialog *self)
{
  GtdActivatable *activatable;
  GtkWidget *old_panel;
  GtkWidget *panel;

  /* First, remove the old panel */
  old_panel = gtk_bin_get_child (GTK_BIN (self->frame));

  if (old_panel)
    {
      g_object_ref (old_panel);
      gtk_container_remove (GTK_CONTAINER (self->frame), old_panel);
    }

  /* Second, setup the new panel */
  activatable = g_object_get_data (G_OBJECT (gtk_widget_get_ancestor (button, GTK_TYPE_LIST_BOX_ROW)),
                                   "plugin");

  panel = gtd_activatable_get_preferences_panel (activatable);

  if (panel)
    {
      gtk_container_add (GTK_CONTAINER (self->frame), panel);
      gtk_widget_show (panel);
    }

  /* Last, go to the preferences page */
  gtk_stack_set_visible_child_name (GTK_STACK (self->stack), "config");
  gtk_widget_show (self->back_button);
}

static gboolean
transform_to (GBinding     *binding,
              const GValue *from_value,
              GValue       *to_value,
              gpointer      user_data)
{
  gboolean active;

  active = g_value_get_boolean (from_value);

  if (active)
    active &= gtd_activatable_get_preferences_panel (GTD_ACTIVATABLE (user_data)) != NULL;

  g_value_set_boolean (to_value, active);

  return TRUE;
}

static void
enabled_switch_changed (GtkSwitch       *sw,
                        GParamSpec      *pspec,
                        PeasActivatable *activatable)
{
  gboolean active;

  g_object_get (activatable,
                "active", &active,
                NULL);

  /* We don't want to (de)activate the extension twice */
  if (active == gtk_switch_get_active (sw))
    return;

  if (gtk_switch_get_active (sw))
    peas_activatable_activate (activatable);
  else
    peas_activatable_deactivate (activatable);
}

static GtkWidget*
create_row_for_plugin (GtdPluginDialog *self,
                       PeasPluginInfo  *info,
                       GtdActivatable  *activatable)
{
  GtkBuilder *builder;
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *icon;
  GtkWidget *row;
  GtkWidget *sw;

  /* Builder */
  builder = gtk_builder_new_from_resource ("/org/gnome/todo/ui/plugin-row.ui");

  /* Row */
  row = GTK_WIDGET (gtk_builder_get_object (builder, "row"));

  /* Icon */
  icon = GTK_WIDGET (gtk_builder_get_object (builder, "icon"));
  gtk_image_set_from_icon_name (GTK_IMAGE (icon),
                                peas_plugin_info_get_icon_name (info),
                                GTK_ICON_SIZE_DND);

  /* Name label */
  label = GTK_WIDGET (gtk_builder_get_object (builder, "name_label"));
  gtk_label_set_label (GTK_LABEL (label), peas_plugin_info_get_name (info));

  /* Description label */
  label = GTK_WIDGET (gtk_builder_get_object (builder, "description_label"));
  gtk_label_set_label (GTK_LABEL (label), peas_plugin_info_get_description (info));

  /* Switch */
  sw = GTK_WIDGET (gtk_builder_get_object (builder, "enabled_switch"));

  g_signal_connect (sw,
                    "notify::active",
                    G_CALLBACK (enabled_switch_changed),
                    activatable);

  g_object_bind_property (activatable,
                          "active",
                          sw,
                          "active",
                          G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);

  /* Preferences button */
  button = GTK_WIDGET (gtk_builder_get_object (builder, "preferences_button"));

  g_signal_connect (button,
                    "clicked",
                    G_CALLBACK (preferences_button_clicked),
                    self);

  g_object_bind_property_full (sw,
                               "active",
                               button,
                               "sensitive",
                               G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE,
                               transform_to,
                               NULL,
                               activatable,
                               NULL);

  g_object_set_data (G_OBJECT (row),
                     "plugin",
                     activatable);

  g_object_set_data (G_OBJECT (row),
                     "info",
                     info);

  gtk_widget_show_all (row);
  g_object_ref (row);

  return row;
}

static void
plugin_loaded (GtdPluginManager *manager,
               PeasPluginInfo   *info,
               GtdActivatable   *activatable,
               GtdPluginDialog  *self)
{
  GtkWidget *row;

  if (peas_plugin_info_is_hidden (info))
    return;

  row = create_row_for_plugin (self, info, activatable);

  gtk_container_add (GTK_CONTAINER (self->listbox), row);

  gtk_stack_set_visible_child_name (GTK_STACK (self->stack), "list");
}

static void
plugin_unloaded (GtdPluginManager *manager,
                 PeasPluginInfo   *info,
                 GtdActivatable   *activatable,
                 GtdPluginDialog  *self)
{
  gboolean contains;
  GList *children;
  GList *l;

  contains = FALSE;
  children = gtk_container_get_children (GTK_CONTAINER (self->listbox));

  for (l = children; l != NULL; l = l->next)
    {
      GtdActivatable *row_activatable;

      row_activatable = g_object_get_data (l->data, "plugin");

      if (row_activatable == activatable)
        {
          gtk_container_remove (GTK_CONTAINER (self->listbox), l->data);
          contains = TRUE;
          break;
        }
    }

  /* If there are no extensions left, go back to the 'empty' view */
  if (g_list_length (children) == 0 ||
      (g_list_length (children) == 1 && contains))
    {
      gtk_stack_set_visible_child_name (GTK_STACK (self->stack), "empty");
    }

  g_free (children);
}

static gint
sort_extensions (GtkListBoxRow *row1,
                 GtkListBoxRow *row2,
                 gpointer       user_data)
{
  PeasPluginInfo *info1, *info2;

  info1 = g_object_get_data (G_OBJECT (row1), "info");
  info2 = g_object_get_data (G_OBJECT (row2), "info");

  return g_strcmp0 (peas_plugin_info_get_name (info1), peas_plugin_info_get_name (info2));
}

static void
gtd_plugin_dialog_class_init (GtdPluginDialogClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/todo/ui/plugin-dialog.ui");

  gtk_widget_class_bind_template_child (widget_class, GtdPluginDialog, back_button);
  gtk_widget_class_bind_template_child (widget_class, GtdPluginDialog, frame);
  gtk_widget_class_bind_template_child (widget_class, GtdPluginDialog, listbox);
  gtk_widget_class_bind_template_child (widget_class, GtdPluginDialog, stack);

  gtk_widget_class_bind_template_callback (widget_class, back_button_clicked);
}

static void
gtd_plugin_dialog_init (GtdPluginDialog *self)
{
  GtdPluginManager *plugin_manager;
  GtdManager *manager;

  manager = gtd_manager_get_default ();
  plugin_manager = gtd_manager_get_plugin_manager (manager);

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect (plugin_manager,
                    "plugin-loaded",
                    G_CALLBACK (plugin_loaded),
                    self);

  g_signal_connect (plugin_manager,
                    "plugin-unloaded",
                    G_CALLBACK (plugin_unloaded),
                    self);

  /* Sort extensions by their display name */
  gtk_list_box_set_sort_func (GTK_LIST_BOX (self->listbox),
                              sort_extensions,
                              NULL,
                              NULL);
}

GtkWidget*
gtd_plugin_dialog_new (void)
{
  return g_object_new (GTD_TYPE_PLUGIN_DIALOG,
                       "use-header-bar", 1,
                       NULL);
}
