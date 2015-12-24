/* gtd-provider-popover.c
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

#include "gtd-manager.h"
#include "interfaces/gtd-provider.h"
#include "gtd-provider-popover.h"
#include "gtd-provider-selector.h"
#include "gtd-task-list.h"

#include <glib/gi18n.h>

typedef struct
{
  GtkWidget            *change_location_button;
  GtkWidget            *location_provider_image;
  GtkWidget            *new_list_create_button;
  GtkWidget            *new_list_name_entry;
  GtkWidget            *stack;
  GtkWidget            *provider_selector;

  GtdManager           *manager;
} GtdProviderPopoverPrivate;

struct _GtdProviderPopover
{
  GtkPopover                parent;

  /*<private>*/
  GtdProviderPopoverPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtdProviderPopover, gtd_provider_popover, GTK_TYPE_POPOVER)

enum {
  PROP_0,
  PROP_MANAGER,
  LAST_PROP
};

static void
clear_and_hide (GtdProviderPopover *popover)
{
  GtdProviderPopoverPrivate *priv;
  GList *locations;
  GList *l;

  g_return_if_fail (GTD_IS_PROVIDER_POPOVER (popover));

  priv = popover->priv;

  /* Select the default source again */
  locations = gtd_manager_get_providers (priv->manager);

  for (l = locations; l != NULL; l = l->next)
    {
      if (FALSE)//gtd_provider_get_is_default (l->data))
        {
          gtd_provider_selector_set_selected_provider (GTD_PROVIDER_SELECTOR (priv->provider_selector), l->data);
          break;
        }
    }

  g_list_free (locations);

  /* By clearing the text, Create button will get insensitive */
  gtk_entry_set_text (GTK_ENTRY (priv->new_list_name_entry), "");

  /* Hide the popover at last */
  gtk_widget_hide (GTK_WIDGET (popover));
}

static void
gtd_provider_popover__closed (GtdProviderPopover *popover)
{
  g_return_if_fail (GTD_IS_PROVIDER_POPOVER (popover));

  gtk_stack_set_visible_child_name (GTK_STACK (popover->priv->stack), "main");
}

static void
create_task_list (GtdProviderPopover *popover)
{
  GtdProviderPopoverPrivate *priv;
  GtdTaskList *task_list;
  GtdProvider *provider;
  const gchar *name;

  priv = popover->priv;
  provider = gtd_provider_selector_get_selected_provider (GTD_PROVIDER_SELECTOR (priv->provider_selector));
  name = gtk_entry_get_text (GTK_ENTRY (priv->new_list_name_entry));

  task_list = gtd_task_list_new (provider);
  gtd_task_list_set_name (task_list, name);

  gtd_provider_create_task_list (provider, task_list);
}

static void
gtd_provider_popover__entry_activate (GtdProviderPopover *popover,
                                     GtkEntry          *entry)
{
  if (gtk_entry_get_text_length (entry) > 0)
    {
      create_task_list (popover);
      clear_and_hide (popover);
    }
}

static void
gtd_provider_popover__action_button_clicked (GtdProviderPopover *popover,
                                            GtkWidget         *button)
{
  GtdProviderPopoverPrivate *priv;

  g_return_if_fail (GTD_IS_PROVIDER_POPOVER (popover));

  priv = popover->priv;

  if (button == priv->new_list_create_button)
    create_task_list (popover);

  clear_and_hide (popover);
}

static void
gtd_provider_popover__text_changed_cb (GtdProviderPopover *popover,
                                      GParamSpec        *spec,
                                      GtkEntry          *entry)
{
  GtdProviderPopoverPrivate *priv;

  g_return_if_fail (GTD_IS_PROVIDER_POPOVER (popover));
  g_return_if_fail (GTK_IS_ENTRY (entry));

  priv = popover->priv;

  gtk_widget_set_sensitive (priv->new_list_create_button, gtk_entry_get_text_length (entry) > 0);
}

static void
gtd_provider_popover__change_location_clicked (GtdProviderPopover *popover,
                                              GtkWidget         *button)
{
  GtdProviderPopoverPrivate *priv;

  g_return_if_fail (GTD_IS_PROVIDER_POPOVER (popover));

  priv = popover->priv;

  if (button == priv->change_location_button)
    gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "selector");
  else
    gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "main");
}

static void
gtd_provider_popover__provider_selected (GtdProviderPopover *popover,
                                       GtdProvider        *provider)
{
  GtdProviderPopoverPrivate *priv;

  g_return_if_fail (GTD_IS_PROVIDER_POPOVER (popover));
  g_return_if_fail (GTD_IS_PROVIDER (provider));

  priv = popover->priv;

  if (provider)
    {
      gtk_image_set_from_gicon (GTK_IMAGE (priv->location_provider_image),
                                gtd_provider_get_icon (provider),
                                GTK_ICON_SIZE_BUTTON);
      gtk_widget_set_tooltip_text (priv->change_location_button, gtd_provider_get_name (provider));

      /* Go back immediately after selecting a provider */
      gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "main");

      if (gtk_widget_is_visible (GTK_WIDGET (popover)))
        gtk_widget_grab_focus (priv->new_list_name_entry);
    }
}

static void
gtd_provider_popover_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtd_provider_popover_parent_class)->finalize (object);
}

static void
gtd_provider_popover_constructed (GObject *object)
{
  GtdProviderPopoverPrivate *priv;
  GtdProvider *provider;

  G_OBJECT_CLASS (gtd_provider_popover_parent_class)->constructed (object);

  priv = GTD_PROVIDER_POPOVER (object)->priv;
  provider = gtd_provider_selector_get_selected_provider (GTD_PROVIDER_SELECTOR (priv->provider_selector));

  g_object_bind_property (object,
                          "manager",
                          priv->provider_selector,
                          "manager",
                          G_BINDING_DEFAULT);

  if (provider)
    {
      gtk_image_set_from_gicon (GTK_IMAGE (priv->location_provider_image),
                                gtd_provider_get_icon (provider),
                                GTK_ICON_SIZE_BUTTON);
    }
}

static void
gtd_provider_popover_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GtdProviderPopover *self = GTD_PROVIDER_POPOVER (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      g_value_set_object (value, self->priv->manager);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_provider_popover_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GtdProviderPopover *self = GTD_PROVIDER_POPOVER (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      if (self->priv->manager != g_value_get_object (value))
        {
          self->priv->manager = g_value_get_object (value);
          g_object_notify (object, "manager");
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_provider_popover_class_init (GtdProviderPopoverClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtd_provider_popover_finalize;
  object_class->constructed = gtd_provider_popover_constructed;
  object_class->get_property = gtd_provider_popover_get_property;
  object_class->set_property = gtd_provider_popover_set_property;

  /**
   * GtdProviderPopover::manager:
   *
   * A weak reference to the application's #GtdManager instance.
   */
  g_object_class_install_property (
        object_class,
        PROP_MANAGER,
        g_param_spec_object ("manager",
                             "Manager of this window's application",
                             "The manager of the window's application",
                             GTD_TYPE_MANAGER,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/todo/ui/provider-popover.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderPopover, change_location_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderPopover, location_provider_image);
  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderPopover, new_list_create_button);
  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderPopover, new_list_name_entry);
  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderPopover, stack);
  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderPopover, provider_selector);

  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_popover__action_button_clicked);
  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_popover__change_location_clicked);
  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_popover__closed);
  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_popover__entry_activate);
  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_popover__provider_selected);
  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_popover__text_changed_cb);
}

static void
gtd_provider_popover_init (GtdProviderPopover *self)
{
  self->priv = gtd_provider_popover_get_instance_private (self);

  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget*
gtd_provider_popover_new (void)
{
  return g_object_new (GTD_TYPE_PROVIDER_POPOVER, NULL);
}
