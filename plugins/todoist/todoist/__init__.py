#!/usr/bin/env python3

# todoist.py
#
# Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gi

gi.require_version('Goa',  '1.0')
gi.require_version('Gtd',  '1.0')
gi.require_version('Peas', '1.0')

from gi.repository import Gtd
from gi.repository import Peas
from gi.repository import Gio
from gi.repository import GLib
from gi.repository import GObject
from gi.repository import Gtk
from gi.repository import Goa

import os
import pexpect
from .api import TodoistAPI

from gettext import gettext as _


class GoogleLoginPage(Gtk.Revealer):

    __gsignals__ = {
        'account-selected': (GObject.SignalFlags.RUN_FIRST, None, (Goa.Object,)),
        'cancel': (GObject.SignalFlags.RUN_FIRST, None, ())
    }

    def __init__(self):
        Gtk.Revealer.__init__(self, transition_type=Gtk.RevealerTransitionType.SLIDE_UP,
                                    halign=Gtk.Align.CENTER,
                                    valign=Gtk.Align.CENTER)

        # Build filename
        _ui_file = os.path.join(os.path.dirname(__file__), 'google-accounts.ui')

        self.builder = Gtk.Builder.new_from_file(_ui_file)
        self.listbox = self.builder.get_object('accounts_listbox')
        self.cancel_button = self.builder.get_object('cancel_button')
        self.goa_label = self.builder.get_object('not_listed_account_label')

        self.cancel_button.connect('clicked', self._cancel_button_clicked)
        self.listbox.connect('row-activated', self._row_activated)
        self.goa_label.connect('activate-link', self._add_google_account)

        self.add(self.builder.get_object('google_accounts_frame'))
        self.show_all()

        # Load the GOA client, so we can link the Google
        # account from Online Accounts to Todoist Google
        # authenticator.
        Goa.Client.new(None, self._goa_client_ready)

    def _goa_client_ready(self, source, res, data=None):
        """ Callback for when GOA client is ready """
        self.goa_client = Goa.Client.new_finish(res)

        for obj in self.goa_client.get_accounts():
            self._account_added(obj)

        self.goa_client.connect('account-added', self._account_added)
        self.goa_client.connect('account-removed', self._account_removed)

    def _account_added(self, obj):
        """ Add the GOA account if it's a Google account """
        if obj.get_account().props.provider_type != 'google':
            return

        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=12)
        box.set_border_width(6)
        box.add(Gtk.Image.new_from_icon_name('goa-account-google',
                                             Gtk.IconSize.LARGE_TOOLBAR))
        box.add(Gtk.Label(label=obj.get_account().props.identity))

        row = Gtk.ListBoxRow()
        row.add(box)
        row.show_all()

        row.goa_object = obj
        self.listbox.add(row)

    def _account_removed(self, obj):
        """ Remove the GOA account """
        identity = obj.get_account().props.identity

        for row in self.listbox.get_children():
            if row.goa_object.get_account().props.identity == identity:
                self.listbox.remove(row)

    def _row_activated(self, listbox, row, data=None):
        self.emit('account-selected', row.goa_object)

    def _cancel_button_clicked(self, data=None):
        self.set_reveal_child(False)

    def _add_google_account(self, uri, data):
        pexpect.run('gnome-control-center online-accounts add google')
        return False

class TodoistPreferencesPanel(Gtk.Overlay):
    api = None

    __gsignals__ = {
        'account-logged': (GObject.SignalFlags.RUN_FIRST, None, (Goa.Object,
                                                                 object,))
    }

    def __init__(self, api):
        Gtk.Overlay.__init__(self)

        self.api = api

        # Build filename
        _ui_file = os.path.join(os.path.dirname(__file__), 'preferences-panel.ui')

        self.builder = Gtk.Builder.new_from_file(_ui_file)
        self.welcome_box = self.builder.get_object('welcome_box')
        self.email_entry = self.builder.get_object('email_entry')
        self.password_entry = self.builder.get_object('password_entry')
        self.google_login_button = self.builder.get_object('google_login_button')
        self.google_login_page = GoogleLoginPage()

        self.add(self.welcome_box)
        self.add_overlay(self.google_login_page)

        self.google_login_button.connect('clicked', self.show_account_list)
        self.google_login_page.connect('account-selected', self._account_selected)

    def show_account_list(self, data=None):
        self.google_login_page.set_reveal_child(True)

    def _account_selected(self, panel, goa_object):
        account = goa_object.get_account()
        oauth2 = goa_object.get_oauth2_based()

        oauth2.call_get_access_token(None,
                                     self._access_token_retrieved,
                                     goa_object)

    def _access_token_retrieved(self, source, res, goa_object):
        account = goa_object.get_account()
        (access_token, timeout) = source.call_get_access_token_finish(res)

        user = self.api.login_with_google(account.props.identity, access_token)

        self.emit('account-logged', goa_object, user)


class TodoistPlugin(GObject.Object, Gtd.Activatable):
    user = None
    provider = None
    goa_object = None

    preferences_panel = GObject.Property(type=Gtk.Widget, default=None)

    def __init__(self):
        GObject.Object.__init__(self)
        # Todoist API
        self.api = TodoistAPI()

        # Preferences panel
        self.preferences_panel = TodoistPreferencesPanel(self.api)
        self.preferences_panel.connect('account-logged', self._account_logged)

    def do_activate(self):
        pass

    def do_deactivate(self):
        pass

    def do_get_preferences_panel(self):
        return self.preferences_panel

    def do_get_header_widgets(self):
        return None

    def do_get_panels(self):
        return None

    def do_get_providers(self):
        return None

    def _account_logged(self, panel, goa_object, user):
        if user is not None:
            self.user = user
            self.goa_object = goa_object
            print(user)
