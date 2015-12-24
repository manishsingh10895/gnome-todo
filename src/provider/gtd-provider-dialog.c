/* gtd-provider-dialog.c
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
#include "gtd-provider-dialog.h"
#include "gtd-provider-selector.h"

#include <glib/gi18n.h>

typedef struct
{
  GtkWidget       *headerbar;
  GtkWidget       *provider_selector;

  GtdManager      *manager;
} GtdProviderDialogPrivate;

struct _GtdProviderDialog
{
  GtkDialog                parent;

  /*<private>*/
  GtdProviderDialogPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtdProviderDialog, gtd_provider_dialog, GTK_TYPE_DIALOG)

enum {
  PROP_0,
  PROP_MANAGER,
  LAST_PROP
};

static void
gtd_provider_dialog__provider_selected (GtdProviderDialog *dialog,
                                      GtdProvider       *provider)
{
  GtdProviderDialogPrivate *priv;

  g_return_if_fail (GTD_IS_PROVIDER_DIALOG (dialog));
  g_return_if_fail (GTD_IS_MANAGER (dialog->priv->manager));
  g_return_if_fail (GTD_IS_PROVIDER (provider));

  priv = dialog->priv;

  if (provider)
    gtd_manager_set_default_provider (priv->manager, provider);
}

static void
gtd_provider_dialog_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtd_provider_dialog_parent_class)->finalize (object);
}

static void
gtd_provider_dialog_constructed (GObject *object)
{
  GtdProviderDialogPrivate *priv;

  /* Chain up */
  G_OBJECT_CLASS (gtd_provider_dialog_parent_class)->constructed (object);

  priv = GTD_PROVIDER_DIALOG (object)->priv;

  gtk_window_set_titlebar (GTK_WINDOW (object), priv->headerbar);

  g_object_bind_property (object,
                          "manager",
                          priv->provider_selector,
                          "manager",
                          G_BINDING_DEFAULT);
}

static void
gtd_provider_dialog_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GtdProviderDialog *self = GTD_PROVIDER_DIALOG (object);

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
gtd_provider_dialog_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GtdProviderDialog *self = GTD_PROVIDER_DIALOG (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      gtd_provider_dialog_set_manager (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_provider_dialog_class_init (GtdProviderDialogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtd_provider_dialog_finalize;
  object_class->constructed = gtd_provider_dialog_constructed;
  object_class->get_property = gtd_provider_dialog_get_property;
  object_class->set_property = gtd_provider_dialog_set_property;


  /**
   * GtdProviderDialog::manager:
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
                             G_PARAM_READWRITE));

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/todo/ui/provider-dialog.ui");

  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderDialog, headerbar);
  gtk_widget_class_bind_template_child_private (widget_class, GtdProviderDialog, provider_selector);

  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_dialog__provider_selected);
}

static void
gtd_provider_dialog_init (GtdProviderDialog *self)
{
  self->priv = gtd_provider_dialog_get_instance_private (self);

  gtk_widget_init_template (GTK_WIDGET (self));
}

/**
 * gtd_provider_dialog_new:
 *
 * Creates a new #GtdProviderDialog.
 *
 * Returns: (transfer full): the newly allocated #GtdProviderDialog
 */
GtkWidget*
gtd_provider_dialog_new (void)
{
  return g_object_new (GTD_TYPE_PROVIDER_DIALOG, NULL);
}

/**
 * gtd_provider_dialog_get_manager:
 * @dialog: a #GtdProviderDialog
 *
 * Retrieves @dialog's internal #GtdManager.
 *
 * Returns: (transfer none): the @dialog's #GtdManager
 */
GtdManager*
gtd_provider_dialog_get_manager (GtdProviderDialog *dialog)
{
  g_return_val_if_fail (GTD_IS_PROVIDER_DIALOG (dialog), NULL);

  return dialog->priv->manager;
}

/**
 * gtd_provider_dialog_set_manager:
 * @dialog: a #GtdProviderDialog
 * @manager: a #GtdManager
 *
 * Sets the @dialog's #GtdManager.
 *
 * Returns:
 */
void
gtd_provider_dialog_set_manager (GtdProviderDialog *dialog,
                                GtdManager       *manager)
{
  GtdProviderDialogPrivate *priv;

  g_return_if_fail (GTD_IS_PROVIDER_DIALOG (dialog));

  priv = dialog->priv;

  if (priv->manager != manager)
    {
      priv->manager = manager;
      g_object_notify (G_OBJECT (dialog), "manager");
    }
}
