AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DUI_DATA_DIR=\""$(pkgdatadir)/style"\" \
	-DGOA_API_IS_SUBJECT_TO_CHANGE \
	-I$(srcdir)/provider \
	-I$(srcdir)/notification

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
	views/gtd-list-selector.c \
	views/gtd-list-selector.h \
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
	gtd-plugin-manager.c \
	gtd-plugin-manager.h \
	gtd-task.c \
	gtd-task.h \
	gtd-task-list.c \
	gtd-task-list.h \
	gtd-task-list-item.c \
	gtd-task-list-item.h \
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
