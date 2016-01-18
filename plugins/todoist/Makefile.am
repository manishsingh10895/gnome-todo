include $(top_srcdir)/common.am

EXTRA_DIST = $(plugin_DATA)

todoistplugindir = $(plugindir)/todoist
todoistplugin_DATA = \
	todoist.plugin
nobase_todoistplugin_DATA =  \
	todoist/__init__.py \
	todoist/preferences-panel.ui \
	todoist/google-accounts.ui \
	todoist/models.py \
	todoist/api.py \
	todoist/managers/biz_invitations.py \
	todoist/managers/collaborators.py \
	todoist/managers/collaborator_states.py \
	todoist/managers/filters.py \
	todoist/managers/generic.py \
	todoist/managers/__init__.py \
	todoist/managers/invitations.py \
	todoist/managers/items.py \
	todoist/managers/labels.py \
	todoist/managers/live_notifications.py \
	todoist/managers/locations.py \
	todoist/managers/notes.py \
	todoist/managers/projects.py \
	todoist/managers/reminders.py \
	todoist/managers/user.py

EXTRA_DIST = \
	$(resource_files) \
	todoist.plugin.in

GITIGNOREFILES = \
	todoist/__pycache__ \
	todoist/managers/__pycache__
