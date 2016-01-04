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

struct _GtdProviderDialog
{
  GtkDialog        parent;
  GtkWidget       *headerbar;
  GtkWidget       *provider_selector;
};

G_DEFINE_TYPE (GtdProviderDialog, gtd_provider_dialog, GTK_TYPE_DIALOG)

static void
gtd_provider_dialog__provider_selected (GtdProviderDialog *dialog,
                                      GtdProvider       *provider)
{
  GtdManager *manager;

  g_return_if_fail (GTD_IS_PROVIDER_DIALOG (dialog));
  g_return_if_fail (GTD_IS_PROVIDER (provider));

  manager = gtd_manager_get_default ();

  if (provider)
    gtd_manager_set_default_provider (manager, provider);
}

static void
gtd_provider_dialog_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtd_provider_dialog_parent_class)->finalize (object);
}

static void
gtd_provider_dialog_constructed (GObject *object)
{
  GtdProviderDialog *self = GTD_PROVIDER_DIALOG (object);

  /* Chain up */
  G_OBJECT_CLASS (gtd_provider_dialog_parent_class)->constructed (object);

  gtk_window_set_titlebar (GTK_WINDOW (object), self->headerbar);

}

static void
gtd_provider_dialog_class_init (GtdProviderDialogClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtd_provider_dialog_finalize;
  object_class->constructed = gtd_provider_dialog_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/todo/ui/provider-dialog.ui");

  gtk_widget_class_bind_template_child (widget_class, GtdProviderDialog, headerbar);
  gtk_widget_class_bind_template_child (widget_class, GtdProviderDialog, provider_selector);

  gtk_widget_class_bind_template_callback (widget_class, gtd_provider_dialog__provider_selected);
}

static void
gtd_provider_dialog_init (GtdProviderDialog *self)
{
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
