/* gtd-task-list-eds.c
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

#include "gtd-task-list-eds.h"

#include <glib/gi18n.h>

struct _GtdTaskListEds
{
  GtdTaskList         parent;

  ESource            *source;

  GCancellable       *cancellable;
};

G_DEFINE_TYPE (GtdTaskListEds, gtd_task_list_eds, GTD_TYPE_TASK_LIST)

enum {
  PROP_0,
  PROP_SOURCE,
  N_PROPS
};

static void
source_removable_changed (GtdTaskListEds *list)
{
  gtd_task_list_set_is_removable (GTD_TASK_LIST (list),
                                  e_source_get_removable (list->source) ||
                                  e_source_get_remote_deletable (list->source));
}

static void
save_task_list_finished_cb (GObject      *source,
                            GAsyncResult *result,
                            gpointer      user_data)
{
  GtdTaskListEds *list;
  GError *error;

  list = user_data;
  error = NULL;

  gtd_object_set_ready (GTD_OBJECT (list), TRUE);

  e_source_write_finish (E_SOURCE (source),
                         result,
                         &error);

  if (error)
    {
      g_warning ("%s: %s: %s",
                 G_STRFUNC,
                 _("Error saving task list"),
                 error->message);
      g_clear_error (&error);
    }
}

static void
save_task_list (GtdTaskListEds *list)
{
  if (!list->cancellable)
    list->cancellable = g_cancellable_new ();

  gtd_object_set_ready (GTD_OBJECT (list), FALSE);

  e_source_write (list->source,
                  list->cancellable,
                  save_task_list_finished_cb,
                  list);
}
static gboolean
color_to_string (GBinding     *binding,
                 const GValue *from_value,
                 GValue       *to_value,
                 gpointer      user_data)
{
  GdkRGBA *color;
  gchar *color_str;

  color = g_value_get_boxed (from_value);
  color_str = gdk_rgba_to_string (color);

  g_value_set_string (to_value, color_str);

  g_free (color_str);

  return TRUE;
}

static gboolean
string_to_color (GBinding     *binding,
                 const GValue *from_value,
                 GValue       *to_value,
                 gpointer      user_data)
{
  GdkRGBA color;

  if (!gdk_rgba_parse (&color, g_value_get_string (from_value)))
    gdk_rgba_parse (&color, "#ffffff"); /* calendar default colour */

  g_value_set_boxed (to_value, &color);

  return TRUE;
}

static void
gtd_task_list_eds_finalize (GObject *object)
{
  GtdTaskListEds *self = GTD_TASK_LIST_EDS (object);

  g_cancellable_cancel (self->cancellable);

  g_clear_object (&self->cancellable);
  g_clear_object (&self->source);

  G_OBJECT_CLASS (gtd_task_list_eds_parent_class)->finalize (object);
}

static void
gtd_task_list_eds_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtdTaskListEds *self = GTD_TASK_LIST_EDS (object);

  switch (prop_id)
    {
    case PROP_SOURCE:
      g_value_set_object (value, self->source);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_task_list_eds_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtdTaskListEds *self = GTD_TASK_LIST_EDS (object);

  switch (prop_id)
    {
    case PROP_SOURCE:
      gtd_task_list_eds_set_source (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_task_list_eds_class_init (GtdTaskListEdsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtd_task_list_eds_finalize;
  object_class->get_property = gtd_task_list_eds_get_property;
  object_class->set_property = gtd_task_list_eds_set_property;

  /**
   * GtdTaskListEds::source:
   *
   * The #ESource of this #GtdTaskListEds
   */
  g_object_class_install_property (object_class,
                                   PROP_SOURCE,
                                   g_param_spec_object ("source",
                                                        "ESource of this list",
                                                        "The ESource of this list",
                                                        E_TYPE_SOURCE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
gtd_task_list_eds_init (GtdTaskListEds *self)
{
}

GtdTaskListEds*
gtd_task_list_eds_new (GtdProvider *provider,
                       ESource     *source)
{
  return g_object_new (GTD_TYPE_TASK_LIST_EDS,
                       "provider", provider,
                       "source", source,
                       NULL);
}

ESource*
gtd_task_list_eds_get_source (GtdTaskListEds *list)
{
  g_return_val_if_fail (GTD_IS_TASK_LIST_EDS (list), NULL);

  return list->source;
}

void
gtd_task_list_eds_set_source (GtdTaskListEds *list,
                              ESource        *source)
{
  g_return_if_fail (GTD_IS_TASK_LIST_EDS (list));

  if (g_set_object (&list->source, source))
    {
      ESourceSelectable *selectable;
      GdkRGBA color;

      /* Setup color */
      selectable = E_SOURCE_SELECTABLE (e_source_get_extension (source, E_SOURCE_EXTENSION_TASK_LIST));

      if (!gdk_rgba_parse (&color, e_source_selectable_get_color (selectable)))
        gdk_rgba_parse (&color, "#ffffff"); /* calendar default color */

      gtd_task_list_set_color (GTD_TASK_LIST (list), &color);

      g_object_bind_property_full (list,
                                   "color",
                                   selectable,
                                   "color",
                                   G_BINDING_BIDIRECTIONAL,
                                   color_to_string,
                                   string_to_color,
                                   list,
                                   NULL);

      /* Setup tasklist name */
      gtd_task_list_set_name (GTD_TASK_LIST (list), e_source_get_display_name (source));

      g_object_bind_property (source,
                              "display-name",
                              list,
                              "name",
                              G_BINDING_BIDIRECTIONAL);

      /* Save the task list every time something changes */
      g_signal_connect_swapped (source,
                                "notify",
                                G_CALLBACK (save_task_list),
                                list);

      /* Update ::is-removable property */
      gtd_task_list_set_is_removable (GTD_TASK_LIST (list),
                                      e_source_get_removable (source) ||
                                      e_source_get_remote_deletable (source));

      g_signal_connect_swapped (source,
                                "notify::removable",
                                G_CALLBACK (source_removable_changed),
                                list);

      g_signal_connect_swapped (source,
                                "notify::remote-deletable",
                                G_CALLBACK (source_removable_changed),
                                list);

      g_object_notify (G_OBJECT (list), "source");
    }
}
