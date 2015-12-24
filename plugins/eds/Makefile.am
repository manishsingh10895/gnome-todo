include $(top_srcdir)/common.am

plugindir = $(datadir)/gnome-todo/plugins

plugin_LTLIBRARIES = libeds.la
plugin_DATA = eds.plugin

BUILT_SOURCES = \
	gtd-plugin-eds-resources.c

libeds_la_SOURCES = \
	gtd-plugin-eds.c \
	gtd-plugin-eds.h \
	gtd-provider-eds.c \
	gtd-provider-eds.h \
	gtd-provider-goa.c \
	gtd-provider-goa.h \
	gtd-provider-local.c \
	gtd-provider-local.h \
	gtd-task-list-eds.c \
	gtd-task-list-eds.h \
	$(BUILT_SOURCES)

libeds_la_CFLAGS = \
	$(GNOME_TODO_PLUGIN_CFLAGS) \
	$(GNOME_TODO_PLUGIN_WARN_CFLAGS) \
	-DGOA_API_IS_SUBJECT_TO_CHANGE \
	-DG_LOG_DOMAIN='"EDS"'

libeds_la_LIBADD = \
	$(GNOME_TODO_LIBS)

libeds_la_LDFLAGS = \
	-Wl -module \
	$(GNOME_TODO_WARN_LDFLAGS)

# generate binary-bundle resources
resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies \
                --sourcedir=$(top_srcdir)/plugins/eds $(top_srcdir)/plugins/eds/eds.gresource.xml)

gtd-plugin-eds-resources.c: eds.gresource.xml $(resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(top_srcdir)/plugins/eds --generate-source $<



EXTRA_DIST = eds.plugin.in
