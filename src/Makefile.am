AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DUI_DATA_DIR=\""$(pkgdatadir)/style"\" \
	-I$(srcdir)/provider \
	-I$(srcdir)/notification \
	-I$(srcdir)/interfaces \
	-I$(srcdir)/plugin \
	-I$(srcdir)/views

bin_PROGRAMS = gnome-todo

BUILT_SOURCES = \
	gtd-resources.c \
	gtd-resources.h \
	gtd-enum-types.c \
	gtd-enum-types.h

ENUM_TYPES = \
	gtd-enums.h

gnome_todo_SOURCES = \
	$(BUILT_SOURCES) \
	interfaces/gtd-activatable.c \
	interfaces/gtd-activatable.h \
	interfaces/gtd-panel.c \
	interfaces/gtd-panel.h \
	interfaces/gtd-provider.c \
	interfaces/gtd-provider.h \
	notification/gtd-notification.c \
	notification/gtd-notification.h \
	notification/gtd-notification-widget.c \
	notification/gtd-notification-widget.h \
	provider/gtd-provider-dialog.c \
	provider/gtd-provider-dialog.h \
	provider/gtd-provider-popover.c \
	provider/gtd-provider-popover.h \
	provider/gtd-provider-row.c \
	provider/gtd-provider-row.h \
	provider/gtd-provider-selector.c \
	provider/gtd-provider-selector.h \
	plugin/gtd-plugin-dialog.c \
	plugin/gtd-plugin-dialog.h \
	plugin/gtd-plugin-manager.c \
	plugin/gtd-plugin-manager.h \
	views/gtd-list-selector.c \
	views/gtd-list-selector.h \
	views/gtd-list-selector-grid.c \
	views/gtd-list-selector-grid.h \
	views/gtd-list-selector-grid-item.c \
	views/gtd-list-selector-grid-item.h \
	views/gtd-list-selector-item.c \
	views/gtd-list-selector-item.h \
	views/gtd-list-selector-list.c \
	views/gtd-list-selector-list.h \
	views/gtd-list-selector-list-item.c \
	views/gtd-list-selector-list-item.h \
	views/gtd-list-selector-panel.c \
	views/gtd-list-selector-panel.h \
	gtd-application.c \
	gtd-application.h \
	gtd-arrow-frame.c \
	gtd-arrow-frame.h \
	gtd-edit-pane.c \
	gtd-edit-pane.h \
	gtd-enums.h \
	gtd-initial-setup-window.c \
	gtd-initial-setup-window.h \
	gtd-manager.c \
	gtd-manager.h \
	gtd-manager-protected.h \
	gtd-object.c \
	gtd-object.h \
	gtd-task.c \
	gtd-task.h \
	gtd-task-list.c \
	gtd-task-list.h \
	gtd-task-list-view.c \
	gtd-task-list-view.h \
	gtd-task-row.c \
	gtd-task-row.h \
	gtd-types.h \
	gtd-window.c \
	gtd-window.h \
	main.c

gnome_todo_CFLAGS = \
	$(GNOME_TODO_CFLAGS) \
	$(GNOME_TODO_WARN_CFLAGS)

gnome_todo_LDFLAGS = \
	-Wl,--export-dynamic \
	$(GNOME_TODO_WARN_LDFLAGS)

gnome_todo_LDADD = \
	$(GNOME_TODO_LIBS)

#
# Export headers to [PREFIX]/include/gnome-todo
#
gnome_todo_includedir = $(includedir)/gnome-todo
gnome_todo_include_HEADERS = \
	interfaces/gtd-activatable.h \
	interfaces/gtd-panel.h \
	interfaces/gtd-provider.h \
	notification/gtd-notification.h \
	gtd-enums.h \
	gtd-manager.h \
	gtd-object.h \
	gtd-task.h \
	gtd-task-list.h \
	gtd-task-list-view.h \
	gtd-window.h \
	gtd-types.h \
	gnome-todo.h

#
# Resources
#
resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(top_srcdir)/data --generate-dependencies $(top_srcdir)/data/todo.gresource.xml)
gtd-resources.c: $(top_srcdir)/data/todo.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(top_srcdir)/data --generate-source --c-name todo $(top_srcdir)/data/todo.gresource.xml
gtd-resources.h: $(top_srcdir)/data/todo.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(top_srcdir)/data --generate-header --c-name todo $(top_srcdir)/data/todo.gresource.xml

#
# Enums
#
gtd-enum-types.c: gtd-enum-types.c.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtd-enum-types.c.template $(ENUM_TYPES)) > $@

gtd-enum-types.h: gtd-enum-types.h.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtd-enum-types.h.template $(ENUM_TYPES)) > $@

CLEANFILES = \
	$(BUILT_SOURCES)

EXTRA_DIST = \
    $(resource_files) \
    gtd-enum-types.h.template \
    gtd-enum-types.c.template

-include $(top_srcdir)/git.mk


#==================================
# GObject introspection
#==================================
if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = \
	--add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir)

introspection_sources = \
	interfaces/gtd-activatable.c \
	interfaces/gtd-activatable.h \
	interfaces/gtd-panel.c \
	interfaces/gtd-panel.h \
	interfaces/gtd-provider.c \
	interfaces/gtd-provider.h \
	notification/gtd-notification.c \
	notification/gtd-notification.h \
	gtd-enums.h \
	gtd-manager.c \
	gtd-manager.h \
	gtd-object.c \
	gtd-object.h \
	gtd-task.c \
	gtd-task.h \
	gtd-task-list.c \
	gtd-task-list.h \
	gtd-task-list-view.c \
	gtd-task-list-view.h \
	gtd-window.c \
	gtd-window.h \
	gtd-types.h \
	$(NULL)

Gtd-1.0.gir: gnome-todo
Gtd_1_0_gir_INCLUDES = Gio-2.0 GObject-2.0 Gtk-3.0
Gtd_1_0_gir_CFLAGS = $(gnome_todo_CFLAGS)
Gtd_1_0_gir_PROGRAM = $(builddir)/gnome-todo
Gtd_1_0_gir_SCANNERFLAGS = \
	-n Gtd \
	--identifier-prefix Gtd  \
	--symbol-prefix gtd
Gtd_1_0_gir_FILES = $(introspection_sources)
INTROSPECTION_GIRS += Gtd-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += \
	$(gir_DATA) \
	$(typelib_DATA)
endif
